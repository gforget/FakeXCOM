// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UI3DManager.generated.h"

class ACover3DIcon;
class ASelect3DIcon;
class ATBTacticalGameMode;
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
class FAKEXCOM_API UUI3DManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUI3DManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "3D Icons Classes")
	TSubclassOf<ACover3DIcon> Cover3DIconClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "3D Icons Classes")
	TSubclassOf<ASelect3DIcon> Select3DIconClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "3D Icons Classes")
	TSubclassOf<AActor> Path3DIconClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "3D Icons Classes")
	TSubclassOf<AActor> BaseDistanceLimitIconClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "3D Icons Classes")
	TSubclassOf<AActor> LongDistanceLimitIconClass;

	void Initialize();
	void SubscribeToUnitEvent(AUnit* Unit);
	void ClearCover3DIcons();
	void AddCover3DIcon(FVector Location, FRotator Rotation, bool bFullCover);
	void ConnectCover3DIconsToUnit(int IdUnit);

	void ClearSelect3DIcon();
	void AddSelect3DIcon(const FVector& Location, const FRotator& Rotation, bool bIsBaseDistance);
	
	void ClearPath3DIcons();
	void AddPath3DIcon(FVector Location, FRotator Rotation);

private:
	UPROPERTY()
	UNodePath* CurrentMouseOverNodePath;
	
	UPROPERTY()
	ASelect3DIcon* Select3DIcon;

	UPROPERTY()
	TArray<AActor*> AllPathDistance3DIcons;
	
	UPROPERTY()
	TArray<AActor*> AllPath3DIcons;
	
	UPROPERTY()
	TArray<ACover3DIcon*> AllMouseOverCover3DIcon;

	UPROPERTY()
	TMap<int, FAssignCover3DIcon> AllAssignCover3DIcon;

	UPROPERTY()
	ATBTacticalGameMode*  TBTacticalGameMode;
	
	UFUNCTION()
	void OnUnitSelected(AUnit* Unit);
	
	UFUNCTION()
	void OnUnitOrderedToMove(AUnit* Unit);
	
	UFUNCTION()
	void OnMouseOverActor(AActor* Actor, FVector HitLocation);

	UFUNCTION()
	void OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue);
	
	void CreateDistanceLimitUI(AUnit* Unit);
	void ClearDistanceLimitUI();
	void SpawnDistanceIcons(TArray<UNodePath*> AllNodes, int Distance, TSubclassOf<AActor> IconSubClass);
	bool TrySpawnDistanceIcon(const UNodePath* NodePath, int DistanceLimit, TArray<bool>& CornerToSpawn);
};
