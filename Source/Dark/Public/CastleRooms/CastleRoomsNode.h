// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "CastleRoomsNode.generated.h"

UCLASS()
class DARK_API UCastleRoomsNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	// Public Methods
	UCastleRoomsNode();

	// Core node functionality
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetNodeContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;

public:
	// Public Properties
	UPROPERTY(EditAnywhere, Category = "Room")
	FString RoomName;

	UPROPERTY(EditAnywhere, Category = "Room")
	FText RoomDescription;

protected:
	// Protected Methods

protected:
	// Protected Properties

private:
	// Private Methods

private:
	// Private Properties
};