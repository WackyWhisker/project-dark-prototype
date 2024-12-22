// Copyright @ Christian Reichel

#include "Component/DkHealthComponent.h"
#include "GameFramework/Character.h"
#include "Interface/DkDeathHandlerInterface.h"
#include "Subsystem/DkGameStateSubsystem.h"

UDkHealthComponent::UDkHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    bIsDead = false;
}

void UDkHealthComponent::InitializeComponent()
{
    Super::InitializeComponent();
    CurrentHealth = MaxHealth; 
}

void UDkHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Setup damage handling
    if (AActor* Owner = GetOwner())
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UDkHealthComponent::HandleTakeAnyDamage);
        
        // Cache the mesh and material for hit flash
        if (ACharacter* Character = Cast<ACharacter>(Owner))
        {
            OwnerMesh = Character->GetMesh();
            if (OwnerMesh)
            {
                OriginalMaterial = OwnerMesh->GetMaterial(0);
            }
        }
    }
    
    // Subscribe to game state changes
    if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
    {
        GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &UDkHealthComponent::HandleGameStateChanged);
    }
}

void UDkHealthComponent::PlayHitFlash()
{
	// Early out if we don't have required components
	if (!OwnerMesh || !OriginalMaterial) return;
   
	// Create dynamic material instance if needed
	UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
	if (!DynamicMat) return;
   
	// Set flash intensity and color
	DynamicMat->SetScalarParameterValue(FlashIntensityParam, 1.0f);
	DynamicMat->SetVectorParameterValue(FlashColorParam, FlashColor);
	OwnerMesh->SetMaterial(0, DynamicMat);
   
	// Clear any existing timer
	if (GetWorld()->GetTimerManager().IsTimerActive(HitFlashTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(HitFlashTimerHandle);
	}
   
	// Reset after delay
	GetWorld()->GetTimerManager().SetTimer(
		HitFlashTimerHandle,
		[this, DynamicMat]()
		{
			if (DynamicMat)
			{
				DynamicMat->SetScalarParameterValue(FlashIntensityParam, 0.0f);
			}
		},
		HitFlashDuration,
		false
	);
}

void UDkHealthComponent::TakeDamage(float DamageAmount)
{
    if (DamageAmount <= 0.0f || bIsDead || !bCanTakeDamage) {return;}

    bCanTakeDamage = false;
    
    CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
    
    // Play hit flash effect
    PlayHitFlash();
    
    OnHealthChanged.Broadcast(CurrentHealth, -DamageAmount);
    
    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;
        OnHealthDepleted.Broadcast();

        if (AActor* Owner = GetOwner())
        {
            if (Owner->Implements<UDkDeathHandlerInterface>())
            {
                IDkDeathHandlerInterface::Execute_HandleDeath(Owner);
            }
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        DamageCooldownTimer,
        this,
        &UDkHealthComponent::ResetDamageCooldown,
        DamageCooldownDuration,
        false
    );
}

void UDkHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || bIsDead) {return;}

	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	float ActualHealAmount = CurrentHealth - OldHealth;

	if (ActualHealAmount > 0.0f)
	{
		OnHealthChanged.Broadcast(CurrentHealth, ActualHealAmount);
	}
}

bool UDkHealthComponent::IsAlive() const
{
	return !bIsDead;
}

void UDkHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	TakeDamage(Damage);
	ApplyDamagePushback(DamagedActor, Damage, DamageCauser);
}

void UDkHealthComponent::ApplyDamagePushback(AActor* DamagedActor, float Damage, AActor* DamageCauser)
{
	if (!DamagedActor || !DamageCauser)
		return;

	ACharacter* OwningCharacter = Cast<ACharacter>(DamagedActor);
	if (!OwningCharacter)
		return;

	FVector PushDirection = (DamagedActor->GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
	OwningCharacter->LaunchCharacter(PushDirection * DamagePushbackStrength, false, false);
}

void UDkHealthComponent::HandleGameStateChanged(EDkGameState NewState, EDkGameState OldState)
{
	if (NewState == EDkGameState::Respawning)
	{
		// Register for reset when entering reset state
		if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
		{
			//GameStateSubsystem->RegisterForReset(this);
			//bIsRegisteredForReset = true;
            
			// Reset health
			CurrentHealth = MaxHealth;
			bIsDead = false;
			bCanTakeDamage = true;
            
			// Broadcast the health change
			OnHealthChanged.Broadcast(CurrentHealth, MaxHealth - CurrentHealth);
            
			// Notify reset complete immediately since we're done
			//GameStateSubsystem->NotifyResetComplete(this);
			//bIsRegisteredForReset = false;
		}
	}
}

void UDkHealthComponent::ResetDamageCooldown()
{
	bCanTakeDamage = true;
}

void UDkHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
    
	// Clean up subscription
	if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
	{
		GameStateSubsystem->OnGameStateChanged.RemoveDynamic(this, &UDkHealthComponent::HandleGameStateChanged);
        
		// If we're registered for reset, notify completion
		if (bIsRegisteredForReset)
		{
			GameStateSubsystem->NotifyResetComplete(this);
			bIsRegisteredForReset = false;
		}
	}
}



