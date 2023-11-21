// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Select3DIcon.generated.h"

UCLASS()
class FAKEXCOM_API ASelect3DIcon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASelect3DIcon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Icon Properties")
	UStaticMeshComponent* BaseDistanceIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Icon Properties")
	UStaticMeshComponent* LongDistanceIcon;
};
