// Copyright @ Christian Reichel

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "CustomGraph.generated.h"


UCLASS()
class DARKEDITOR_API UCustomGraph : public UEdGraph
{
	GENERATED_BODY()
    
public:
#if WITH_EDITOR
	UCustomGraph();
#endif

	// Serialization functions must not be inside preprocessor blocks
	virtual void Serialize(FArchive& Ar) override;
};