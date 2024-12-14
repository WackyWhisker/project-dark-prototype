// Copyright @ Christian Reichel

#include "GraphEditor/CustomGraph.h"

#if WITH_EDITOR
UCustomGraph::UCustomGraph()
{
	// Initialize any custom properties here if needed
}
#endif

void UCustomGraph::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	// Add custom serialization code here if needed
}