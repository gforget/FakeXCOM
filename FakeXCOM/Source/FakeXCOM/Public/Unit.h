// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "UnitManager.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

class UTileMovementComponent;
class AGun;
class ATBTacticalGameMode;
class UArrowComponent;
class UCapsuleComponent;
class UUnitAttributeSet;
class UUnitAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitRanOutOfActionsDelegate, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitHealthChangeDelegate, AUnit*, Unit);

UCLASS()
class FAKEXCOM_API AUnit : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostActorCreated() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void Destroyed() override;
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category="Cover")
	float LowCoverDefenceBonus = 20.0f;

	UPROPERTY(EditDefaultsOnly, Category="Cover")
	float FullCoverDefenceBonus = 40.0f;

	UPROPERTY(EditDefaultsOnly, Category="Cover")
	float LowGroundDisadvantage = 20.0f;
	
	UPROPERTY(BlueprintAssignable)
	FUnitRanOutOfActionsDelegate OnUnitRanOutOfActionsEvent;
	
	UPROPERTY(BlueprintAssignable)
	FUnitHealthChangeDelegate OnUnitHealthChangeEvent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<int> DefaultAbilitiesIndexes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> OwnedAbilitiesClasses;

	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	TArray<FGameplayAbilitySpecHandle> OwnedAbilitiesHandle;
	
	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	TArray<UUnitAbility*> OwnedAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Unit Properties")
	TEnumAsByte<EFaction> Faction = EFaction::XCOM;

	UPROPERTY(EditDefaultsOnly, Category="Unit Properties")
	FVector SightStartingAnchor;

	UPROPERTY()
	TArray<FVector> SightSurroundDefendingAnchor;

	UPROPERTY()
	TArray<FVector> SightSurroundTargetingAnchor;
	
	UPROPERTY(EditDefaultsOnly, Category="Unit Properties")
	float DistanceSurroundTargetingAnchor = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category="Unit Properties")
	float DistanceSurroundDefendingAnchor = 25.0f;
	
	UPROPERTY()
	const UUnitAttributeSet* UnitAttributeSet;
	
	UPROPERTY(EditDefaultsOnly)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* ArrowComponent;
	
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTileMovementComponent* TileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties")
	TSubclassOf<AGun> GunClass;

	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY(BlueprintReadOnly)
	AGun* Gun;

	UPROPERTY(BlueprintReadOnly)
	int IdUnit = 0;

	UPROPERTY(EditDefaultsOnly, Category="Game UI")
	FVector HealthBarAnchor = FVector(0.0f, 0.0f, 88.0f);

	UPROPERTY(BlueprintReadOnly)
	FString UnitName = "";
	
	void Initialize();

	void CallRanOutOfActions();
	void CallHealthChanged();
	
	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetTargetCoverDefenceBonus(AUnit* Target);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetHeightAdvantageBonus(AUnit* Target);
	
private:
	void GenerateEditorAnchorPositionVisualisation() const;
	
	UUnitAbility* GetAbilityFromHandle(FGameplayAbilitySpecHandle AbilityHandle) const;
};
