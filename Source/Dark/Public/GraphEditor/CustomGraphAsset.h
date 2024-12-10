// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CustomGraphAsset.generated.h"

class UEdGraph;

UCLASS()
class DARK_API UCustomGraphAsset : public UObject
{
	GENERATED_BODY()

public:
	UCustomGraphAsset();

	// The actual graph that contains nodes and connections
	UPROPERTY()
	UEdGraph* EdGraph;
};
