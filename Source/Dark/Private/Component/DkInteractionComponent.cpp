// Copyright @ Christian Reichel


#include "Component/DkInteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "Character/DkCharacter.h"
#include "Components/SphereComponent.h"
#include "Interface/DkInteractableInterface.h"


// Sets default values for this component's properties
UDkInteractionComponent::UDkInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Create and set up sphere collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(InteractionRadius);
	SphereCollision->SetCollisionProfileName(TEXT("Trigger"));
	SphereCollision->SetGenerateOverlapEvents(true);
    
	// Set up attachment in constructor
	SphereCollision->SetupAttachment(GetOwner() ? GetOwner()->GetRootComponent() : nullptr);
}

void UDkInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
    
	if (!PlayerRef)
	{
		PlayerRef = Cast<ADkCharacter>(GetOwner());
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
		// 	FString::Printf(TEXT("PlayerRef Set: %s"), PlayerRef ? TEXT("Yes") : TEXT("No")));
	}

	if (PlayerRef)
	{
		PlayerCameraRef = PlayerRef->FindComponentByClass<UCameraComponent>();
		// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
		// 	FString::Printf(TEXT("CameraRef Set: %s"), PlayerCameraRef ? TEXT("Yes") : TEXT("No")));

		// Manually attach the sphere
		if (SphereCollision && PlayerRef->GetRootComponent())
		{
			SphereCollision->AttachToComponent(
				PlayerRef->GetRootComponent(),
				FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
            
			// Debug message for sphere
			// GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
			// 	FString::Printf(TEXT("Sphere Radius: %f"), InteractionRadius));
		}
	}
}
void UDkInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Debug draw the sphere
	/*if (PlayerRef)
	{
		DrawDebugSphere(
			GetWorld(),
			PlayerRef->GetActorLocation(),
			InteractionRadius,
			32,
			FColor::Green,
			false,
			-1,
			0,
			1
		);
	}*/

	UpdateClosestInteractable();
}

void UDkInteractionComponent::UpdateClosestInteractable()
{
	if (!PlayerCameraRef || !PlayerRef)
	{
		// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Missing PlayerCameraRef or PlayerRef!"));
		// return;
	}

	TArray<AActor*> OverlappingActors;
	SphereCollision->GetOverlappingActors(OverlappingActors);
    
	// Debug overlap count
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, 
	// 	FString::Printf(TEXT("Overlapping Actors: %d"), OverlappingActors.Num()));

	// First filter for interface implementation
	TArray<AActor*> InteractableActors;
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->Implements<UDkInteractableInterface>())
		{
			if (IDkInteractableInterface::Execute_CanInteract(Actor, PlayerRef))
			{
				InteractableActors.Add(Actor);
			}
		}
	}

	// Debug interactable count
	// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Blue, 
	// 	FString::Printf(TEXT("Interactable Actors: %d"), InteractableActors.Num()));

	TArray<AActor*> ValidTargets;
	if (FilterInteractablesByViewCone(InteractableActors, ValidTargets))
	{
		FocusedActor = ValidTargets[0];
        
		// // Debug message for currently focused interactable
		// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, 
		// 	FString::Printf(TEXT("Speak to %s"), *FocusedActor->GetName()));
	}
	else
	{
		FocusedActor = nullptr;
		// GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Orange, TEXT("No valid target in view cone"));
	}
}

bool UDkInteractionComponent::FilterInteractablesByViewCone(const TArray<AActor*>& PotentialTargets, 
    TArray<AActor*>& OutValidTargets)
{
    if (!PlayerCameraRef || !PlayerRef) return false;

    FVector CameraLocation = PlayerCameraRef->GetComponentLocation();
    FVector CameraForward = PlayerCameraRef->GetForwardVector();
    
    float CosMaxAngle = FMath::Cos(FMath::DegreesToRadians(MaxViewConeAngle));

    struct FTargetDot
    {
        AActor* Actor;
        float DotProduct;

        FTargetDot(AActor* InActor, float InDot)
            : Actor(InActor), DotProduct(InDot) {}
    };

    TArray<FTargetDot> ValidTargetsWithDot;

    for (AActor* Target : PotentialTargets)
    {
        if (!Target) continue;

        FVector DirectionToTarget = (Target->GetActorLocation() - CameraLocation).GetSafeNormal();
        float DotProduct = FVector::DotProduct(CameraForward, DirectionToTarget);

        if (DotProduct >= CosMaxAngle)
        {
            ValidTargetsWithDot.Add(FTargetDot(Target, DotProduct));
        }
    }

    // Sort by dot product (highest/most centered first)
    ValidTargetsWithDot.Sort([](const FTargetDot& A, const FTargetDot& B) {
        return A.DotProduct > B.DotProduct;
    });

    OutValidTargets.Empty();
    for (const FTargetDot& TargetDot : ValidTargetsWithDot)
    {
        OutValidTargets.Add(TargetDot.Actor);
    }
    return OutValidTargets.Num() > 0;
}


