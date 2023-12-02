// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetButton.h"
UTargetButton::UTargetButton()
{
	OnClicked.AddDynamic(this, &UTargetButton::OnClick);
}

UTargetButton::~UTargetButton()
{
	UnbindEvents();
}

void UTargetButton::OnClick()
{
}

void UTargetButton::UnbindEvents()
{
}
