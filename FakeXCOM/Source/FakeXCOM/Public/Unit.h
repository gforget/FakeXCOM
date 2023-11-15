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

	void Initialize();
};
