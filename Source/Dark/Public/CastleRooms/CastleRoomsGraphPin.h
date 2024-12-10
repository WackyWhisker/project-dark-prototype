// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"

class SCastleRoomsGraphPin : public SGraphPin
{
public:
	// Public Methods
	SLATE_BEGIN_ARGS(SCastleRoomsGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

public:
	// Public Properties

protected:
	// Protected Methods
	// Override to customize pin appearance
	virtual FSlateColor GetPinColor() const override;
	virtual const FSlateBrush* GetPinIcon() const override;

protected:
	// Protected Properties

private:
	// Private Methods

private:
	// Private Properties
};