// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "CustomNode.generated.h"


UCLASS()
class DARK_API UCustomNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UCustomNode();

	// Override basic node functions
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;

	// Add a custom property that we'll be able to edit
	UPROPERTY(EditAnywhere, Category = "Node")
	FString NodeName;
};
