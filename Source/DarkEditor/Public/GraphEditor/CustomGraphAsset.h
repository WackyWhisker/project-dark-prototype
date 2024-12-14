// Copyright @ Christian Reichel

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CustomGraphAsset.generated.h"

class UEdGraph;

UCLASS()
class DARKEDITOR_API UCustomGraphAsset : public UObject
{
	GENERATED_BODY()

public:
	UCustomGraphAsset();

	// The actual graph that contains nodes and connections
	UPROPERTY()
	UEdGraph* EdGraph;
};
#endif

