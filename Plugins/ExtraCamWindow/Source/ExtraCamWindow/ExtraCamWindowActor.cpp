#include "ExtraCamWindowActor.h"

#include "IExtraCamWindowPlugin.h"

AExtraCamWindowActor::AExtraCamWindowActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	CineCameraComponent = CreateDefaultSubobject<UCineCameraComponent>(TEXT("CineCamera"));
	CineCameraComponent->SetupAttachment(RootComponent);

	ExtraCamComponent = CreateDefaultSubobject<UExtraCamWindowComponent>(TEXT("ExtraCamWindowComponent"));
	ExtraCamComponent->SetupAttachment(CineCameraComponent);
}
