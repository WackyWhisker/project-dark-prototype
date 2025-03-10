#include "LevelObject/DkDamageTrigger.h"

void ADkDamageTrigger::BeginPlay()
{
    Super::BeginPlay();
    OnActorBeginOverlap.AddDynamic(this, &ADkDamageTrigger::HandleActorBeginOverlap);
    OnActorEndOverlap.AddDynamic(this, &ADkDamageTrigger::HandleActorEndOverlap);
}

void ADkDamageTrigger::HandleActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!OtherActor) return;

    if (UDkHealthComponent* HealthComp = OtherActor->FindComponentByClass<UDkHealthComponent>())
    {
        ActorsInTrigger.AddUnique(OtherActor);

        if (bInstantKill)
        {
            HealthComp->TakeDamage(HealthComp->GetMaxHealth());
            return;
        }

        HealthComp->TakeDamage(BaseDamage);

        if (bApplyDamageOverTime && !GetWorldTimerManager().IsTimerActive(DamageTimerHandle))
        {
            GetWorldTimerManager().SetTimer(DamageTimerHandle, this, &ADkDamageTrigger::ApplyDamageOverTime, DamageInterval, true);
        }
    }
}

void ADkDamageTrigger::HandleActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!OtherActor) return;
    
    ActorsInTrigger.Remove(OtherActor);

    if (ActorsInTrigger.Num() == 0)
    {
        GetWorldTimerManager().ClearTimer(DamageTimerHandle);
    }
}

void ADkDamageTrigger::ApplyDamageOverTime()
{
    // Create a copy of the array to iterate over
    TArray<AActor*> ActorsToProcess = ActorsInTrigger;
    ActorsInTrigger.Empty();

    for (AActor* Actor : ActorsToProcess)
    {
        if (!Actor) 
        {
            continue;
        }

        if (UDkHealthComponent* HealthComp = Actor->FindComponentByClass<UDkHealthComponent>())
        {
            HealthComp->TakeDamage(BaseDamage);
            // If the actor is still valid after taking damage, keep it in the trigger
            ActorsInTrigger.AddUnique(Actor);
        }
    }

    if (ActorsInTrigger.Num() == 0)
    {
        GetWorldTimerManager().ClearTimer(DamageTimerHandle);
    }
}
