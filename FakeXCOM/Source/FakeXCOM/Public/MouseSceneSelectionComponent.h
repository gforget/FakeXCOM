// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cover3DIcon.h"
#include "Components/ActorComponent.h"
#include "MouseSceneSelectionComponent.generated.h"
class ATBTacticalGameMode;
class ASoldier;
class UNodePath;

USTRUCT()
struct NO_API FAssignCover3DIcon
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<ACover3DIcon*> Entries;

	FAssignCover3DIcon(){}
};

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse Scene Selection Properties")
	TSubclassOf<AActor> Select3DIconClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mouse Scene Selection Properties")
	TSubclassOf<ACover3DIcon> Cover3DIconClass;
	
private:
	void LeftClickSelection();
	void RightClickSelection();
	
	AActor* SelectActorFromMousePosition(FVector& HitPosition, bool bDebugShowActorNameReturned = false);

	UPROPERTY()
	UInputComponent* InputComponentPtr;
	
	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;

	UPROPERTY()
	ASoldier* SelectedSoldier;

	UPROPERTY()
	UNodePath* CurrentMouseOverNodePath;

	UPROPERTY()
	AActor* Select3DIcon;

	UPROPERTY()
	TArray<ACover3DIcon*> AllMouseOverCover3DIcon;

	UPROPERTY()
	TMap<int, FAssignCover3DIcon> AllAssignCover3DIcon;
};
