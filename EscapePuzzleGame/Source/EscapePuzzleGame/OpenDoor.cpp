// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	//Get the owner
	Owner = GetOwner();

	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("Assign the PressurePlate in the editor"));
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



		if (GetTotalMassOfActorsOnTrigger()>=TriggerMassThreshold) 
		{
			if (!IsDoorOpen)
			{
				OnOpen.Broadcast();
				IsDoorOpen = true;
			}
		}
		else //Close the door once the delay is complete
		{		
			if (IsDoorOpen) {
				OnClose.Broadcast();
				IsDoorOpen = false;
			}
		}

}

float UOpenDoor::GetTotalMassOfActorsOnTrigger()
{
	float TotalMass = 0.0f;

	TArray<UPrimitiveComponent*> OverlappingComponents;

	if (!PressurePlate) { return TotalMass; }

	PressurePlate->GetOverlappingComponents(OUT OverlappingComponents);

	for (const auto* OComponent : OverlappingComponents)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping Actor: %s, Mass: %f"), *(OComponent->GetOwner()->GetName()), OComponent->GetMass())
		TotalMass += OComponent->GetMass();
	}
	UE_LOG(LogTemp, Warning, TEXT("Mass: %f"), TotalMass);
	return TotalMass;
}

