// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Unit.generated.h"

class UTileMovementComponent;
class AGun;
class ATBTacticalGameMode;
class UArrowComponent;
class UCapsuleComponent;

UCLASS()
class FAKEXCOM_API AUnit : public AActor
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	
	UPROPERTY()
	AGun* Gun;

	UPROPERTY(BlueprintReadOnly)
	int IdUnit = 0;

	UPROPERTY(EditDefaultsOnly, Category="Game UI")
	FVector HealthBarAnchor = FVector(0.0f, 0.0f, 88.0f);

	UPROPERTY(BlueprintReadOnly)
	FString UnitName = "";
	
	void Initialize();
	
private:
	void GenerateHealthBarAnchorPositionVisualisation() const;
};
