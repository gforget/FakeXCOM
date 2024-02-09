// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"

#include "Managers/UnitManager.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

class UTileMovementComponent;
class AGun;
class ATBTacticalGameMode;
class UArrowComponent;
class UAIBrainComponent;
class UCapsuleComponent;
class UUnitAttributeSet;
class UUnitAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitRanOutOfActionsDelegate, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitHealthChangeDelegate, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitIsDeadDelegate, AUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitOrderedToMoveDelegate, AUnit*, Unit);

//This struct is define here and not in the ability, because storing value at runtime in the ability
//seem unreliable (variables seem to get reset randomly).

USTRUCT(BlueprintType)
struct NO_API FUnitAbilityInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bIsDisabled;

	UPROPERTY(BlueprintReadWrite)
	bool bIsHidden;
	
	UPROPERTY(BlueprintReadWrite)
	float RangeValue;

	UPROPERTY(BlueprintReadWrite)
	TArray<AUnit*> AllAvailableUnitTargets;

	UPROPERTY(BlueprintReadWrite)
	TMap<int, float> TargetUnitsMinDamage;

	UPROPERTY(BlueprintReadWrite)
	TMap<int, float> TargetUnitsMaxDamage;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<int, float> TargetUnitsHitChances;

	UPROPERTY(BlueprintReadWrite)
	TMap<int, float> TargetUnitsCritChance;
	
	FUnitAbilityInfoStruct()
	{
		bIsDisabled = false;
		bIsHidden = false;
		RangeValue = 0.0f;
		AllAvailableUnitTargets = TArray<AUnit*>();
		TargetUnitsMinDamage = TMap<int, float>();
		TargetUnitsMaxDamage = TMap<int, float>();
		TargetUnitsHitChances = TMap<int, float>();
		TargetUnitsCritChance = TMap<int, float>();
	}
	
	FUnitAbilityInfoStruct(
		bool _bIsDisabled,
		bool _bIsHidden,
		float _RangeValue,
		const TArray<AUnit*>& _AllAvailableTargets,
		const TMap<int, float>& _TargetMinDamages,
		const TMap<int, float>& _TargetMaxDamages,
		const TMap<int, float>& _TargetsHitChances,
		const TMap<int, float>& _TargetsCritChances
		)
	{
		bIsDisabled = _bIsDisabled;
		bIsHidden = _bIsHidden;
		RangeValue = _RangeValue;
		AllAvailableUnitTargets = _AllAvailableTargets;
		TargetUnitsMinDamage = _TargetMinDamages;
		TargetUnitsMaxDamage = _TargetMaxDamages;
		TargetUnitsHitChances = _TargetsHitChances;
		TargetUnitsCritChance = _TargetsCritChances;
	}
};

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

#if WITH_EDITOR
	virtual void PostActorCreated() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif
	
	virtual void Destroyed() override;
	
public:
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Cover")
	float LowCoverDefenceBonus = 20.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Cover")
	float FullCoverDefenceBonus = 40.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Cover")
	float LowGroundDisadvantage = 20.0f;
	
	UPROPERTY(BlueprintAssignable)
	FUnitRanOutOfActionsDelegate OnUnitRanOutOfActionsEvent;
	
	UPROPERTY(BlueprintAssignable)
	FUnitHealthChangeDelegate OnUnitHealthChangeEvent;

	UPROPERTY(BlueprintAssignable)
	FUnitIsDeadDelegate OnUnitIsDeadEvent;

	UPROPERTY(BlueprintAssignable)
	FUnitOrderedToMoveDelegate OnUnitOrderedToMoveEvent;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	FString DefaultAbilityId = "Shoot";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> OwnedAbilitiesClasses;

	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	TMap<FString, FGameplayAbilitySpecHandle> OwnedAbilitiesHandle;
	
	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	TMap<FString, UUnitAbility*> OwnedAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Unit Properties")
	TEnumAsByte<EFaction> Faction = EFaction::XCOM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Unit Properties")
	float ZGroundOffset = 88.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Unit Properties")
	FVector SightStartingAnchor;

	UPROPERTY()
	TArray<FVector> SightSurroundDefendingAnchor;

	UPROPERTY()
	TArray<FVector> SightSurroundTargetingAnchor;

	UPROPERTY()
	TArray<FVector> SightSurroundFlankTargetingAnchor;

	UPROPERTY(EditDefaultsOnly, Category="Unit Properties")
	float DistanceSurroundFlankTargetingAnchor = 100.0f;
	
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UAIBrainComponent* AIBrainComponent;
	
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

	UPROPERTY(BlueprintReadWrite, category = "Data")
	TMap<FString, FUnitAbilityInfoStruct> UnitAbilityInfos;
	
	void Initialize();

	void CallRanOutOfActions();
	void CallHealthChanged();
	
	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetTargetCoverDefenceBonus(AUnit* TargetUnit, UNodePath* UnitNode, UNodePath* TargetNode);

	UFUNCTION(BlueprintPure, Category="Main Functions")
	float GetHeightAdvantageBonus(AUnit* Target);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void SetIsDead(bool Val);
	
	UFUNCTION(BlueprintPure, Category="Main Functions")
	bool GetIsDead();
	
	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void MoveToNodePath(UNodePath* TargetNodePath);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	void DestroyCapsule();

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	bool TryActivateAbilityByID(FString AbilityID, bool ForceActivation = false);

	UFUNCTION(BlueprintCallable, Category="Main Functions")
	bool CheckAbilityById(FString AbilityID);
	
private:

	UPROPERTY()
	bool bIsDead = false;
	
	void GenerateEditorAnchorPositionVisualisation() const;
	
	UUnitAbility* GetAbilityFromHandle(FGameplayAbilitySpecHandle AbilityHandle) const;

	void MovementActionCost(const UNodePath* Destination);
	
};
