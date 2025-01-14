// Copyright @ Christian Reichel


#include "Npc/DkNpc.h"


// Sets default values
ADkNpc::ADkNpc()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADkNpc::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADkNpc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADkNpc::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

