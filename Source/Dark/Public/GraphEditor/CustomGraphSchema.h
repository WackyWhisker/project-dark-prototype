// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "CustomGraphSchema.generated.h"


UCLASS()
class DARK_API UCustomGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	// Add actions to the graph editor context menu
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;

	// Add actions to context menu when right-clicking a node
	virtual void GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const override;

	// Determine if pins can be connected
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
};