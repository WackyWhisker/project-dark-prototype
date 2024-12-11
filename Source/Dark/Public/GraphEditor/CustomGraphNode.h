// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class UCustomNode;

class SCustomGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SCustomGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UCustomNode* InNode);

	// Override appearance functions
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

private:
	// Node Colors
	const FLinearColor NodeBorderColor = FLinearColor(0.5f, 0.5f, 0.5f);    // Outer frame color
	const FLinearColor NodeBodyColor = FLinearColor(0.5f, 0.5f, 0.5f);     // Main body fill color
	const FLinearColor TitleBorderColor = FLinearColor(0.5f, 0.5f, 0.5f);   // Title section border
	const FLinearColor TitleFillColor = FLinearColor(0.5f, 0.5f, 0.5f);     // Title section fill
	const FLinearColor TitleTextColor = FLinearColor::White;  
};