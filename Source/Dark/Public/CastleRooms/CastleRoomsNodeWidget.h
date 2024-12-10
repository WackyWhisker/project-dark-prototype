// Copyright @ Christian Reichel


#pragma once

#include "CoreMinimal.h"
#include "CastleRoomsNode.h"
#include "SGraphNode.h"


class SCastleRoomsNodeWidget : public SGraphNode
{
public:
	// Public Methods
	SLATE_BEGIN_ARGS(SCastleRoomsNodeWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UCastleRoomsNode* InNode);

public:
	// Public Properties

protected:
	// Protected Methods
	// Override the main node body creation
	virtual TSharedRef<SWidget> CreateNodeContentArea() override;
    
	// Title area
	virtual TSharedRef<SWidget> CreateTitleWidget(TSharedPtr<SNodeTitle> NodeTitle) override;
    
	// Pins
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

protected:
	// Protected Properties
	TSharedPtr<SNodeTitle> NodeTitle;

private:
	// Private Methods

private:
	// Private Properties
};
