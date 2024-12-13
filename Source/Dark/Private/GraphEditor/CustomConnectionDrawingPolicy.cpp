// Copyright @ Christian Reichel

#include "GraphEditor/CustomConnectionDrawingPolicy.h"
#include "Rendering/DrawElements.h"

FCustomConnectionDrawingPolicy::FCustomConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
    : FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
    , GraphObj(InGraphObj)
{
}

void FCustomConnectionDrawingPolicy::DrawConnection(int32 LayerId, const FVector2D& Start, const FVector2D& End, const FConnectionParams& Params)
{
    const FVector2D& P0 = Start;
    const FVector2D& P1 = End;

    // Draw the main line
    FSlateDrawElement::MakeDrawSpaceSpline(
        DrawElementsList,
        LayerId,
        P0,
        FVector2D(0.0f, 0.0f),
        P1,
        FVector2D(0.0f, 0.0f),
        Params.WireThickness,
        ESlateDrawEffect::None,
        Params.WireColor
    );

    // Calculate arrow properties
    const float ArrowSize = 10.0f * ZoomFactor;
    const FVector2D Direction = (End - Start).GetSafeNormal();
    const FVector2D Normal(-Direction.Y, Direction.X);

    // Draw arrow at start
    {
        TArray<FVector2D> ArrowPoints;
        ArrowPoints.Add(Start);
        ArrowPoints.Add(Start + (Direction * ArrowSize) + (Normal * ArrowSize * 0.5f));
        ArrowPoints.Add(Start);
        ArrowPoints.Add(Start + (Direction * ArrowSize) - (Normal * ArrowSize * 0.5f));

        FSlateDrawElement::MakeLines(
            DrawElementsList,
            LayerId,
            FPaintGeometry(),
            ArrowPoints,
            ESlateDrawEffect::None,
            Params.WireColor,
            true,
            Params.WireThickness
        );
    }

    // Draw arrow at end
    {
        TArray<FVector2D> ArrowPoints;
        ArrowPoints.Add(End);
        ArrowPoints.Add(End - (Direction * ArrowSize) + (Normal * ArrowSize * 0.5f));
        ArrowPoints.Add(End);
        ArrowPoints.Add(End - (Direction * ArrowSize) - (Normal * ArrowSize * 0.5f));

        FSlateDrawElement::MakeLines(
            DrawElementsList,
            LayerId,
            FPaintGeometry(),
            ArrowPoints,
            ESlateDrawEffect::None,
            Params.WireColor,
            true,
            Params.WireThickness
        );
    }
}