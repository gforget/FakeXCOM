﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierAnimInstance.h"

#include "TileMovementComponent.h"
#include "Unit.h"

void USoldierAnimInstance::UpdateAnimation(float DeltaTime)
{
	if (TileMovementComponentPtr)
	{
		Speed = TileMovementComponentPtr->CurrentVelocity;
		bIsMoving = Speed > 0.0f;
	}
	
	if (OwningUnit)
    {
    	bIsDead = OwningUnit->GetIsDead();
    }
}

void USoldierAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	if (AActor* OwningActor = GetOwningActor())
	{
		OwningUnit = Cast<AUnit>(OwningActor);
		
		TileMovementComponentPtr = OwningActor->FindComponentByClass<UTileMovementComponent>();
		if (!TileMovementComponentPtr)
		{
			DebugScreen("No TileMovementComponent on the unit", FColor::Red);
		}
	}
}
