// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SoldierAnimInstance.generated.h"

class AUnit;
class UTileMovementComponent;

/**
 * 
 */
UCLASS()
class FAKEXCOM_API USoldierAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsDead;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float Speed;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void UpdateAnimation(float DeltaTime);

	virtual void NativeBeginPlay() override;

private:
	UPROPERTY()
	AUnit* OwningUnit;

	UPROPERTY()
	UTileMovementComponent* TileMovementComponentPtr;
};
