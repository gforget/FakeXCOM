// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUI.generated.h"

/**
 * 
 */
UCLASS()
class FAKEXCOM_API ULevelUI : public UUserWidget
{
	GENERATED_BODY()
public:
	// You can add functions or variables here
	UFUNCTION(BlueprintCallable, Category = "MyWidgetFunctions")
	void SetMessage(const FText& NewMessage);

protected:
	// You can add variables here

	private:
	// You can add private functions or variables here
};
