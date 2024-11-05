// Copyright @ Christian Reichel


#include "Enemy/DkEnemyBase.h"
#include "Components/WidgetComponent.h"

ADkEnemyBase::ADkEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	//Target UI
	// Create the widget component
	TargetReticleWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetReticleWidget"));
	TargetReticleWidget->SetupAttachment(RootComponent);
    
	// Configure the widget
	TargetReticleWidget->SetWidgetSpace(EWidgetSpace::Screen); // Makes it face the camera
	//TargetReticleWidget->SetDrawSize(FVector2D(250, 250)); // Set your desired size
	TargetReticleWidget->SetRelativeLocation(FVector(0, 0, 100)); // Adjust position above enemy
}

void ADkEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	if (TargetReticleWidget)
	{
		TargetReticleWidget->SetWidgetClass(TargetReticleWidgetClass);
		TargetReticleWidget->SetVisibility(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Something not set correctly on the widget class"));
	}
	
}

void ADkEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ADkEnemyBase::ToggleTargetReticle(bool bShowTargetReticle)
{
	TargetReticleWidget->SetVisibility(bShowTargetReticle);
	UE_LOG(LogTemp, Warning, TEXT("Trying to set visibility"));
	
}

bool ADkEnemyBase::CanBeTargeted_Implementation() const
{
	return IDkTargetableInterface::CanBeTargeted_Implementation();
}

void ADkEnemyBase::OnTargeted_Implementation()
{
	IDkTargetableInterface::OnTargeted_Implementation();
	ToggleTargetReticle(true);
	UE_LOG(LogTemp, Warning, TEXT("It was I, %s, being chosen as target"), *GetName());
}

void ADkEnemyBase::OnUntargeted_Implementation()
{
	IDkTargetableInterface::OnUntargeted_Implementation();
	ToggleTargetReticle(false);
}

FVector ADkEnemyBase::GetTargetLocation_Implementation() const
{
	return IDkTargetableInterface::GetTargetLocation_Implementation();
}




