// Copyright @ Christian Reichel

#include "Enemy/DkEnemyBase.h"
#include "Component/DkDamageFlashComponent.h"
#include "Components/WidgetComponent.h"
#include "Hud/DkEnemyHealthWidget.h"
#include "Component/DkHealthComponent.h"

ADkEnemyBase::ADkEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    //Target UI
    TargetReticleWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetReticleWidget"));
    TargetReticleWidget->SetupAttachment(RootComponent);
    TargetReticleWidget->SetVisibility(false);
    TargetReticleWidget->SetWidgetSpace(EWidgetSpace::Screen);
    TargetReticleWidget->SetRelativeLocation(FVector(0, 0, 100));

    // Health UI
    HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidgetComponent"));
    HealthWidgetComponent->SetupAttachment(RootComponent);
    HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    HealthWidgetComponent->SetRelativeLocation(FVector(0, 0, 120));

    //Adding flash component
    FlashComponent = CreateDefaultSubobject<UDkDamageFlashComponent>(TEXT("FlashComponent"));
}

void ADkEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    
    // Setup target reticle
    if (TargetReticleWidget)
    {
        TargetReticleWidget->SetWidgetClass(TargetReticleWidgetClass);
        TargetReticleWidget->SetVisibility(false);
    }

    // Setup health widget
    if (HealthWidgetComponent && HealthWidgetClass)
    {
        HealthWidgetComponent->SetWidgetClass(HealthWidgetClass);
        HealthWidget = Cast<UDkEnemyHealthWidget>(HealthWidgetComponent->GetWidget());
        
        // Bind to health component events
        if (UDkHealthComponent* HealthComp = GetComponentByClass<UDkHealthComponent>())
        {
            HealthComp->OnHealthChanged.AddDynamic(this, &ADkEnemyBase::OnHealthChangedHandler);
            HealthComp->OnBindingChanged.AddDynamic(this, &ADkEnemyBase::OnBindingChangedHandler);
        }
    }

    if (FlashComponent)
    {
        FlashComponent->SetupMesh(GetMesh());
    }
}

void ADkEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ADkEnemyBase::ToggleTargetReticle(bool bShowTargetReticle)
{
    TargetReticleWidget->SetVisibility(bShowTargetReticle);
}

bool ADkEnemyBase::CanBeTargeted_Implementation() const
{
    return IDkTargetableInterface::CanBeTargeted_Implementation();
}

void ADkEnemyBase::OnTargeted_Implementation()
{
    IDkTargetableInterface::OnTargeted_Implementation();
    ToggleTargetReticle(true);
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

void ADkEnemyBase::OnHealthChangedHandler(float Health, float HealthDelta, float MaxHealth)
{
    UpdateHealthWidget();
}

void ADkEnemyBase::OnBindingChangedHandler(float CurrentBinding, float MaxBinding)
{
    UpdateHealthWidget();
}

void ADkEnemyBase::UpdateHealthWidget()
{
    if (UDkHealthComponent* HealthComp = GetComponentByClass<UDkHealthComponent>())
    {
        if (HealthWidget)
        {
            HealthWidget->UpdateBars(
                HealthComp->CurrentHealth,
                HealthComp->MaxHealth,
                HealthComp->GetCurrentBinding(),
                HealthComp->GetMaxBinding()
            );
        }
    }
}