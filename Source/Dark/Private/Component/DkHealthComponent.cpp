// Copyright @ Christian Reichel


#include "Component/DkHealthComponent.h"


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
	AActor* Owner = GetOwner();
	if (Owner)
		Owner->OnTakeAnyDamage.AddDynamic(this, &UDkHealthComponent::HandleTakeAnyDamage);
	
}

void UDkHealthComponent::TakeDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || bIsDead) {return;}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, -DamageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Emerald, this->GetOwner()->GetName() + "'s Current health: " + FString::SanitizeFloat(CurrentHealth));
	
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
		GetOwner()->Destroy(); //TODO: Get rid of this temp destroy asap
	}
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
}



