// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MouseSceneSelectionComponent.generated.h"
class ATBTacticalGameMode;
class AUnit;
class UNodePath;
class UTilePathFinder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLeftClickSelectActorEvent, AActor*, Actor, FVector, HitLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRightClickSelectActorEvent, AActor*, Actor, FVector, HitLocation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMouseOverActorEvent, AActor*, Actor, FVector, HitLocation);

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
	void Initialize();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mouse Scene Selection Properties")
	bool bDebugMouseLineTrace = false;

	UPROPERTY(BlueprintAssignable)
	FLeftClickSelectActorEvent OnLeftClickSelectActorEvent;

	UPROPERTY(BlueprintAssignable)
	FRightClickSelectActorEvent OnRightClickSelectActorEvent;

	UPROPERTY(BlueprintAssignable)
	FMouseOverActorEvent OnMouseOverActorEvent;
	
private:
	void LeftClickSelection();
	void RightClickSelection();
	
	AActor* SelectActorFromMousePosition(FVector& HitPosition, bool bDebugShowActorNameReturned = false) const;

	UPROPERTY()
	UInputComponent* InputComponentPtr;
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY()
	UTilePathFinder* TilePathFinder;

	UPROPERTY()
	AActor* CurrentMouseOverActor;
};
