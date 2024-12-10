// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "CastleRoomsSchema.generated.h"

UCLASS()
class DARK_API UCastleRoomsSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	// Public Methods
	// Core schema behaviors
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
    
	// Helper functions for node creation
	virtual class UEdGraphNode* CreateNode(UEdGraph* Graph, const FVector2D& Location, TSubclassOf<UEdGraphNode> NodeClass) const;

public:
	// Public Properties

protected:
	// Protected Methods

protected:
	// Protected Properties

private:
	// Private Methods

private:
	// Private Properties
};