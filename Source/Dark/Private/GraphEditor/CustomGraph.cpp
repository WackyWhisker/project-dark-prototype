// Copyright @ Christian Reichel


#include "GraphEditor/CustomGraph.h"

UCustomGraph::UCustomGraph()
{
	// Initialize any custom properties here if needed
}

void UCustomGraph::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	// Add custom serialization code here if needed
}
