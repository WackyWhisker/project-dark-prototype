#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "Component/DkHealthComponent.h"
#include "DkDamageTrigger.generated.h"

UCLASS()
class DARK_API ADkDamageTrigger : public ATriggerVolume
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    
    UFUNCTION()
    void HandleActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
    
    UFUNCTION()
    void HandleActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta=(AllowPrivateAccess = "true"))
    float BaseDamage = 10.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta=(AllowPrivateAccess = "true"))
    bool bInstantKill = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|DoT", meta=(AllowPrivateAccess = "true"))
    bool bApplyDamageOverTime = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|DoT", meta=(AllowPrivateAccess = "true", EditCondition = "bApplyDamageOverTime"))
    float DamageInterval = 1.0f;

    FTimerHandle DamageTimerHandle;
    TArray<AActor*> ActorsInTrigger;

    void ApplyDamageOverTime();
};