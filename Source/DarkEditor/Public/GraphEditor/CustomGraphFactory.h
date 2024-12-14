// Copyright @ Christian Reichel

#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CustomGraphFactory.generated.h"


UCLASS()
class DARKEDITOR_API UCustomGraphFactory : public UFactory
{
	GENERATED_BODY()

public:
	UCustomGraphFactory();

	// Called when creating a new asset in the content browser
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
#endif
