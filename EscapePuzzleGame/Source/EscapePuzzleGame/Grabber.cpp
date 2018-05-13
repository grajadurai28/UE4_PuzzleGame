// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "Engine/World.h"
#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandleComponent();
	SetupInputComponent();
	
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) return;

	if (PhysicsHandle->GrabbedComponent) 
	{
		/// Get raycast start and end point
		LineTraceStartEnd LineTraceSE = GetLineTraceStartEnd();

		PhysicsHandle->SetTargetLocation(LineTraceSE.EndPoint);
	}
}

void UGrabber::Grab()
{
	FHitResult Hit = GetFirstPhysicsBodyInReach();
	if (!PhysicsHandle)return;
	if (Hit.GetActor()) 
	{
		/// Get the component hit and attach it to physics handle
		auto ComponentToGrab = Hit.GetComponent();
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

void UGrabber::Release()
{
	if (!PhysicsHandle)return;
	if (PhysicsHandle->GrabbedComponent) {

		UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GrabbedComponent;

		//GrabbedComponent = PhysicsHandle->GrabbedComponent;
		PhysicsHandle->ReleaseComponent();		

		/// Throwing the grabbed component
		FVector Direction = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetActorForwardVector();
		if (GrabbedComponent)
		{
			GrabbedComponent->AddImpulse(
				Direction * 1000.f, //TODO Find the proper location to go to
				NAME_None,
				true
			);
		}

	}

}

void UGrabber::Throw()
{
	
}

/// Find the physics handle component
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("%s does not have a PhysicsHandleComponent"), *GetOwner()->GetName())
	}
}

/// Find the input component
void UGrabber::SetupInputComponent()
{	
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (!InputComponent) {
		UE_LOG(LogTemp, Error, TEXT("%s does not have a InputComponent"), *GetOwner()->GetName())
	}
	else {
		///Bind the input actions
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

/// Get object hit by the raycast
const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	LineTraceStartEnd LineTraceSE = GetLineTraceStartEnd();

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("")),
		false,
		GetOwner()
	);

	/// Raycast to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		LineTraceSE.StartPoint,
		LineTraceSE.EndPoint,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
}

/// Get the start and end point for raycast
const LineTraceStartEnd UGrabber::GetLineTraceStartEnd()
{
	LineTraceStartEnd LineTraceSE;

	FRotator PlayerVPRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT LineTraceSE.StartPoint,
		OUT PlayerVPRotation);

	LineTraceSE.EndPoint = LineTraceSE.StartPoint + PlayerVPRotation.Vector() * Reach;

	return LineTraceSE;
}






