// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
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

	// Break pin connections
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
    
	// Handle breaking pin connections
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
    
	// Handle breaking pin connections
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const override;

	// Handle breaking a single pin connection
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;

	// Get the default color for a node
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
};