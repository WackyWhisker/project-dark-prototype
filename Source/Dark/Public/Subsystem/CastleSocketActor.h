// Copyright @ Christian Reichel

#pragma once

#include "Components/ArrowComponent.h"
#include "Components/TextRenderComponent.h"
#include "CastleSocketActor.generated.h"

UCLASS()
class ACastleSocketActor : public AActor
{
	GENERATED_BODY()

public:
	ACastleSocketActor();

	// Visual representation of socket
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SocketMesh;

	// Arrow showing connection direction
	UPROPERTY(VisibleAnywhere)
	UArrowComponent* DirectionArrow;

	// Text render component displaying the socket name
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* SocketNameText;
    
	// The socket's name matching the JSON data
	UPROPERTY(EditInstanceOnly, Category = "Socket")
	FName SocketName;

#if WITH_EDITOR
	// Make the socket visible in editor only
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};