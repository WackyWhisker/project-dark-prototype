// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DkDamageFlashComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARK_API UDkDamageFlashComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDkDamageFlashComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	// Call this to play the flash effect
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void PlayFlash();
    
	// Setup the component with the mesh to apply the effect to
	UFUNCTION(BlueprintCallable, Category = "Effects")
	void SetupMesh(UMeshComponent* MeshComponent);
	
protected:
	virtual void BeginPlay() override;

	// Array of material instances to flash
	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DynamicMaterialInstances;
    
	// Timer handle for resetting the flash
	FTimerHandle FlashTimerHandle;
    
	// Duration of the flash effect
	UPROPERTY(EditAnywhere, Category = "Effects")
	float FlashDuration = 0.2f;
    
	// Color of the flash
	UPROPERTY(EditAnywhere, Category = "Effects")
	FLinearColor FlashColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
    
	// Name of the color parameter in the material
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName FlashColorParameter = "FlashColor";
    
	// Name of the intensity parameter in the material
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName FlashIntensityParameter = "FlashIntensity";

	

	
};
