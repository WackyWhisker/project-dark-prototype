#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"
#include "Data/RoomData.h"
#include "RoomSocketActor.generated.h"

UCLASS(Placeable)
class DARK_API ARoomSocketActor : public AActor
{
	GENERATED_BODY()

public:
	ARoomSocketActor();

	// Visual representation of socket
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SocketMesh;

	// Arrow showing connection direction
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* DirectionArrow;

	// Text render component displaying the socket type
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* SocketTypeText;

	// The type of socket (entry or exit)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Socket")
	ERoomSocketType SocketType;

#if WITH_EDITOR
	// Make the socket visible in editor only
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};