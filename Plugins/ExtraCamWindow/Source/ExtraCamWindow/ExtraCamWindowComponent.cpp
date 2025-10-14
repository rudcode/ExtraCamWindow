#include "ExtraCamWindowComponent.h"
#include "Slate/SGameLayerManager.h"
#include "Widgets/SViewport.h"
#include "Slate/SceneViewport.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetInputLibrary.h"
#include "Camera/CameraComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/Engine.h"
#include "ExtraCamViewport.h"

UExtraCamWindowComponent::UExtraCamWindowComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = false;
	CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;

	EditorVisualizer = CreateDefaultSubobject<UCameraComponent>(TEXT("EditorVisualizer"));
	EditorVisualizer->SetupAttachment(this);
	EditorVisualizer->bIsEditorOnly = true;
	EditorVisualizer->SetHiddenInGame(true);
}

void UExtraCamWindowComponent::BeginPlay()
{
	if (!ExtraCamWindowEnabled)
		return;

	auto renderer = FSlateApplication::Get().GetRenderer();

	SAssignNew(ExtraWindow, SWindow)
		.ClientSize(InitialWindowRes)
		.SizingRule(ESizingRule::UserSized)
		.UseOSWindowBorder(true)
		.Title(WindowTitle)
		.FocusWhenFirstShown(LockMouseFocusToExtraWindow)
		.CreateTitleBar(true);
	FSlateApplication::Get().AddWindow(ExtraWindow.ToSharedRef(), true);

	ViewportOverlayWidget = SNew(SOverlay);

	TSharedRef<SGameLayerManager> LayerManagerRef = SNew(SGameLayerManager)
														.SceneViewport(GEngine->GameViewport->GetGameViewport())
														.Visibility(EVisibility::Visible)
														.Cursor(CursorInWindow)
															[ViewportOverlayWidget.ToSharedRef()];

	TSharedPtr<SExtraCamViewport> Viewport = SNew(SExtraCamViewport)
												 .RenderDirectlyToWindow(false) // true crashes some stuff because HMDs need the rendertarget tex for distortion etc..
												 .EnableGammaCorrection(false)
												 .EnableStereoRendering(false) // not displaying on an HMD
												 .Cursor(CursorInWindow)
													 [LayerManagerRef];

	Viewport->OnWindowModeChanged.AddLambda([this]() {
		switch (ExtraWindow->GetWindowMode())
		{
			case EWindowMode::Windowed:
				ExtraWindow->SetWindowMode(EWindowMode::WindowedFullscreen);
				break;
			case EWindowMode::WindowedFullscreen:
				ExtraWindow->SetWindowMode(EWindowMode::Windowed);
				break;
			default:
				break;
		}
	});

	SceneViewport = MakeShareable(new FSceneViewport(GEngine->GameViewport, Viewport));

	Viewport->SetViewportInterface(SceneViewport.ToSharedRef());

	ExtraWindow->SetWindowMode(WindowMode);
	ExtraWindow->SetContent(Viewport.ToSharedRef());
	ExtraWindow->ShowWindow();

	SceneViewport->CaptureMouse(LockMouseFocusToExtraWindow);
	SceneViewport->SetUserFocus(LockMouseFocusToExtraWindow);
	SceneViewport->LockMouseToViewport(LockMouseFocusToExtraWindow);

	if (this->GetWorld()->WorldType == EWorldType::Game)
		StandaloneGame = true;
	else
		StandaloneGame = false;

	TextureTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, InitialWindowRes.X, InitialWindowRes.Y);
	RenderTargetWidget = CreateWidget<URenderWidget>(GetWorld()->GetFirstPlayerController(), URenderWidget::StaticClass());
	RenderTargetWidget->TextureTarget->SetBrushResourceObject(TextureTarget);
	AddWidgetToExtraCam(RenderTargetWidget);

	SceneViewport->SetOnSceneViewportResizeDel(FOnSceneViewportResize::CreateLambda([this](FVector2D NewViewportSize) {
		TextureTarget->ResizeTarget(NewViewportSize.X, NewViewportSize.Y);
	}));

	Super::BeginPlay();
}

void UExtraCamWindowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ExtraCamWindowEnabled)
		return;

	if (LockToPlayerCam)
	{
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		if (playerController != nullptr)
		{
			FVector	 camLoc;
			FRotator camRot;
			playerController->GetPlayerViewPoint(camLoc, camRot);
			SetWorldLocationAndRotation(camLoc, camRot);
		}
	}
}

bool UExtraCamWindowComponent::AddWidgetToExtraCam(UUserWidget* inWidget, int32 zOrder /* = -1 */)
{
	if (ViewportOverlayWidget.IsValid() == false)
		return false;

	ViewportOverlayWidget->AddSlot(zOrder)
		[inWidget->TakeWidget()];

	return true;
}

bool UExtraCamWindowComponent::RemoveWidgetFromExtraCam(UUserWidget* inWidget)
{
	if (ViewportOverlayWidget.IsValid() == false)
		return false;

	return ViewportOverlayWidget->RemoveSlot(inWidget->TakeWidget());
}

void UExtraCamWindowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!ExtraCamWindowEnabled)
		return;

	if (ExtraWindow.Get() != nullptr)
	{
		if (StandaloneGame == false)
			ExtraWindow->RequestDestroyWindow();
		else
			ExtraWindow->DestroyWindowImmediately();
	}

	ExtraWindow.Reset();
	ViewportOverlayWidget.Reset();
	SceneViewport.Reset();

	RenderTargetWidget = nullptr;
}

#if WITH_EDITOR
void UExtraCamWindowComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	EditorVisualizer->SetProjectionMode(ProjectionType);
	EditorVisualizer->SetFieldOfView(FOVAngle);
	EditorVisualizer->SetOrthoWidth(OrthoWidth);
	EditorVisualizer->PostProcessBlendWeight = PostProcessBlendWeight;
	EditorVisualizer->PostProcessSettings = PostProcessSettings;
}
#endif