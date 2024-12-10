// Copyright @ Christian Reichel


#include "GraphEditor/CustomGraphFactory.h"
#include "GraphEditor/CustomGraphAsset.h"
#include "GraphEditor/CustomGraphSchema.h"

UCustomGraphFactory::UCustomGraphFactory()
{
	// This factory creates new assets
	bCreateNew = true;
    
	// Open the editor after creating a new asset
	bEditAfterNew = true;
    
	// This factory creates CustomGraphAsset objects
	SupportedClass = UCustomGraphAsset::StaticClass();
}

UObject* UCustomGraphFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	// Create the actual asset
	UCustomGraphAsset* NewAsset = NewObject<UCustomGraphAsset>(InParent, Name, Flags);
    
	if (NewAsset)
	{
		// Create the EdGraph that will contain our nodes
		NewAsset->EdGraph = NewObject<UEdGraph>(
			NewAsset,                          // Outer object
			FName(*FString::Printf(TEXT("%s_Graph"), *Name.ToString())),  // Graph name
			RF_Transactional                   // Flags for undo/redo support
		);
        
		// Set which schema to use for this graph
		NewAsset->EdGraph->Schema = UCustomGraphSchema::StaticClass();
	}
    
	return NewAsset;
}