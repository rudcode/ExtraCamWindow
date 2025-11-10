#pragma once

#include "CoreMinimal.h"
#include "CineCameraSceneCaptureComponent.h"
#include "Camera/CameraActor.h"
#include "RenderWidget.h"

#include "ExtraCamWindowComponent.generated.h"
/**
 *
 */
UCLASS(HideCategories = (Transform, Collision, Object, Physics, SceneComponent, PostProcessVolume, Projection, Rendering, PlanarReflection), ClassGroup = "ExtraCamWindow", EditInlineNew, meta = (BlueprintSpawnableComponent), Blueprintable)
class EXTRACAMWINDOW_API UExtraCamWindowComponent : public UCineCaptureComponent2D
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	bool ExtraCamWindowEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	bool LockToPlayerCam = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	bool LockMouseFocusToExtraWindow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	TEnumAsByte<EMouseCursor::Type> CursorInWindow = EMouseCursor::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	FText WindowTitle = FText::FromString(TEXT("Extra Cam Window"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	FVector2D InitialWindowRes = FVector2D(1280, 720);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ExtraCamWindow, meta = (ExposeOnSpawn = "true"))
	TEnumAsByte<EWindowMode::Type> WindowMode = EWindowMode::Windowed;

	UFUNCTION(BlueprintCallable, Category = ExtraCamWindow)
	bool AddWidgetToExtraCam(UUserWidget* inWidget, int32 zOrder = -1);

	UFUNCTION(BlueprintCallable, Category = ExtraCamWindow)
	bool RemoveWidgetFromExtraCam(UUserWidget* inWidget);

private:
	void BeginPlay() override;

	void OnConstruction(const FTransform& Transform);
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TSharedPtr<FSceneViewport> SceneViewport = nullptr;
	TSharedPtr<SWindow>		   ExtraWindow = nullptr;
	TSharedPtr<SOverlay>	   ViewportOverlayWidget = nullptr;

	bool		   StandaloneGame = false;
	URenderWidget* RenderTargetWidget = nullptr;
};
