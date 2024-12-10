// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "CustomGraph.generated.h"


UCLASS()
class DARK_API UCustomGraph : public UEdGraph
{
	GENERATED_BODY()
	
public:
	UCustomGraph();

	// Optional: Add custom serialization if needed
	virtual void Serialize(FArchive& Ar) override;
};
