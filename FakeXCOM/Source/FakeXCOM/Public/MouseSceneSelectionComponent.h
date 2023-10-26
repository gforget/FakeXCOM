// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseSceneSelectionComponent.generated.h"
class ATBTacticalGameMode;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FAKEXCOM_API UMouseSceneSelectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMouseSceneSelectionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mouse Scene Selection Properties")
	bool bDebugMouseLineTrace = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mouse Scene Selection Properties")
	bool bDebugShowActorNameReturned = false;
	
private:
	void LeftClickSelection();
	void RightClickSelection();
	
	AActor* SelectActorFromMousePosition(FVector& HitPosition);

	UPROPERTY()
	UInputComponent* InputComponentPtr;
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
};
