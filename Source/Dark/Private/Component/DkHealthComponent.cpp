// Copyright @ Christian Reichel


#include "Component/DkHealthComponent.h"

#include "GameFramework/Character.h"
#include "Interface/DkDeathHandlerInterface.h"
#include "Subsystem/DkGameStateSubsystem.h"


// Sets default values for this component's properties
UDkHealthComponent::UDkHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	// Set default values
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
    
	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UDkHealthComponent::HandleTakeAnyDamage);
	}
    
	// Subscribe to game state changes
	if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
	{
		GameStateSubsystem->OnGameStateChanged.AddDynamic(this, &UDkHealthComponent::HandleGameStateChanged);
	}
}

void UDkHealthComponent::TakeDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || bIsDead || !bCanTakeDamage) {return;}

	bCanTakeDamage = false;
    
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
    
	OnHealthChanged.Broadcast(CurrentHealth, -DamageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, 
		GetOwner()->GetName() + "'s Current health: " + FString::SanitizeFloat(CurrentHealth));
    
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		OnHealthDepleted.Broadcast();

		// Use interface instead of type checking
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
	if (NewState == EDkGameState::Resetting)
	{
		// Register for reset when entering reset state
		if (UDkGameStateSubsystem* GameStateSubsystem = GetWorld()->GetSubsystem<UDkGameStateSubsystem>())
		{
			GameStateSubsystem->RegisterForReset(this);
			bIsRegisteredForReset = true;
            
			// Reset health
			CurrentHealth = MaxHealth;
			bIsDead = false;
			bCanTakeDamage = true;
            
			// Broadcast the health change
			OnHealthChanged.Broadcast(CurrentHealth, MaxHealth - CurrentHealth);
            
			// Notify reset complete immediately since we're done
			GameStateSubsystem->NotifyResetComplete(this);
			bIsRegisteredForReset = false;
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



