// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DkAnimNotifyAttackWindow.generated.h"

UENUM()
enum class EAttackWindowAction : uint8
{
	Open,
	Close
};

UCLASS()
class DARK_API UDkAnimNotifyAttackWindow : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere)
	EAttackWindowAction WindowAction;
};