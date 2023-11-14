// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUI.h"

void ULevelUI::SetMessage(const FText& NewMessage)
{
	UE_LOG(LogTemp, Warning, TEXT("Message set: %s"), *NewMessage.ToString());
}