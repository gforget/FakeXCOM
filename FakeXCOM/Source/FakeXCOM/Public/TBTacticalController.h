// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBTacticalController.generated.h"

/**
 * 
 */
UCLASS()
class FAKEXCOM_API ATBTacticalController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
