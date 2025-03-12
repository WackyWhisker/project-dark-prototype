// Copyright @ Christian Reichel

#include "Enemy/DkEnemyBase.h"
#include "Component/DkDamageFlashComponent.h"
#include "Components/WidgetComponent.h"
#include "Hud/DkEnemyHealthWidget.h"
#include "Component/DkHealthComponent.h"
#include "Components/SphereComponent.h"

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
    HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::World);
    HealthWidgetComponent->SetRelativeLocation(FVector(0, 0, 120));
    HealthWidgetComponent->SetVisibility(false);

    //Adding flash component
    FlashComponent = CreateDefaultSubobject<UDkDamageFlashComponent>(TEXT("FlashComponent"));

    // Create and setup the detection sphere
    DetectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionSphere"));
    DetectionSphere->SetupAttachment(RootComponent);
    DetectionSphere->SetCollisionProfileName(TEXT("AimDetectable")); 
    DetectionSphere->SetSphereRadius(DetectionRadius);
    DetectionSphere->SetHiddenInGame(true);
    DetectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
        
        // Defer visibility setting to ensure it happens after widget initialization
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            if (HealthWidgetComponent)
            {
                HealthWidgetComponent->SetVisibility(false);
            }
        });
        
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
    
    // Cache the player camera manager
    if (GetWorld() && GetWorld()->GetFirstPlayerController())
    {
        CachedPlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
    }
    
    TimeSinceLastWidgetUpdate = 0.0f;
}

void ADkEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (HealthWidgetComponent && HealthWidgetComponent->IsVisible())
    {
        TimeSinceLastWidgetUpdate += DeltaTime;
        
        if (TimeSinceLastWidgetUpdate >= WidgetUpdateInterval)
        {
            TimeSinceLastWidgetUpdate = 0.0f;
            
            if (CachedPlayerCameraManager)
            {
                FVector CameraLocation = CachedPlayerCameraManager->GetCameraLocation();
                FVector Direction = CameraLocation - HealthWidgetComponent->GetComponentLocation();
                Direction.Normalize();
                
                FRotator NewRotation = Direction.Rotation();
                NewRotation.Roll = 0.0f;
                NewRotation.Pitch = 0.0f;
                
                HealthWidgetComponent->SetWorldRotation(NewRotation);
            }
            // If cached manager is invalid, try to get it again
            else if (GetWorld() && GetWorld()->GetFirstPlayerController())
            {
                CachedPlayerCameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
            }
        }
    }
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
    
    // Show widget on damage and start/reset timer
    if (HealthDelta < 0 && HealthWidgetComponent)
    {
        HealthWidgetComponent->SetVisibility(true);
        
        // Clear existing timer and set new one
        GetWorld()->GetTimerManager().ClearTimer(HideHealthWidgetTimer);
        GetWorld()->GetTimerManager().SetTimer(
            HideHealthWidgetTimer, 
            [this](){ HealthWidgetComponent->SetVisibility(false); }, 
            3.0f, 
            false);
    }
}

void ADkEnemyBase::OnBindingChangedHandler(float CurrentBinding, float MaxBinding)
{
    UpdateHealthWidget();
    
    // Reset timer for binding damage too
    if (CurrentBinding > 0 && HealthWidgetComponent)
    {
        HealthWidgetComponent->SetVisibility(true);
        
        // Clear existing timer and set new one
        GetWorld()->GetTimerManager().ClearTimer(HideHealthWidgetTimer);
        GetWorld()->GetTimerManager().SetTimer(
            HideHealthWidgetTimer, 
            [this](){ HealthWidgetComponent->SetVisibility(false); }, 
            3.0f, 
            false);
    }
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

