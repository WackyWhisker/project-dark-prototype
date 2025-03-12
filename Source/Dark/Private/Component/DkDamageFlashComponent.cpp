// Copyright @ Christian Reichel


#include "Component/DkDamageFlashComponent.h"


// Sets default values for this component's properties
UDkDamageFlashComponent::UDkDamageFlashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDkDamageFlashComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDkDamageFlashComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDkDamageFlashComponent::PlayFlash()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayFlash() Called"));
	UE_LOG(LogTemp, Warning, TEXT("Dynamic Material Instances Count: %d"), DynamicMaterialInstances.Num());

	for (UMaterialInstanceDynamic* MaterialInstance : DynamicMaterialInstances)
	{
		if (MaterialInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Processing Material Instance"));
          
			// Set the flash parameters
			MaterialInstance->SetVectorParameterValue(FlashColorParameter, FlashColor);
			MaterialInstance->SetScalarParameterValue(FlashIntensityParameter, 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("FLASH"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Null Material Instance"));
		}
	}
    
	// Clear any existing timer
	GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);
    
	// Set timer to reset the flash
	GetWorld()->GetTimerManager().SetTimer(
		FlashTimerHandle,
		[this]()
		{
			for (UMaterialInstanceDynamic* MaterialInstance : DynamicMaterialInstances)
			{
				if (MaterialInstance)
				{
					MaterialInstance->SetScalarParameterValue(
						FlashIntensityParameter, 
						0.0f
					);
				}
			}
		},
		FlashDuration,
		false
	);
}

void UDkDamageFlashComponent::SetupMesh(UMeshComponent* MeshComponent)
{
	if (!MeshComponent) return;
    
	// Clear existing instances
	DynamicMaterialInstances.Empty();
    
	// Create dynamic material instances for all materials on the mesh
	for (int32 i = 0; i < MeshComponent->GetNumMaterials(); i++)
	{
		if (UMaterialInterface* Material = MeshComponent->GetMaterial(i))
		{
			UMaterialInstanceDynamic* DynamicInstance = 
				UMaterialInstanceDynamic::Create(Material, this);
			DynamicMaterialInstances.Add(DynamicInstance);
			MeshComponent->SetMaterial(i, DynamicInstance);
		}
	}
}

