// Copyright @ Christian Reichel

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"

class FCustomConnectionDrawingPolicy : public FConnectionDrawingPolicy
{
public:
	FCustomConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj);

	// FConnectionDrawingPolicy interface
	virtual void DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params) override;

protected:
	UEdGraph* GraphObj;
};
#endif