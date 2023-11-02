// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"

#include "TileMovementComponent.h"

void USoldierAnimInstance::UpdateAnimation(float DeltaTime)
{
	if (TileMovementComponentPtr)
	{
		Speed = TileMovementComponentPtr->CurrentVelocity;
		IsMoving = Speed > 0.0f;
		DebugScreen(FString::Printf(TEXT("%f"), Speed), FColor::Cyan);
	}
}

void USoldierAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if (const APawn* OwningPawn = TryGetPawnOwner())
	{
		TileMovementComponentPtr = OwningPawn->FindComponentByClass<UTileMovementComponent>();
		if (!TileMovementComponentPtr)
		{
			DebugScreen("No TileMovementComponent on the unit", FColor::Red);
		}
	}
}
