// Fill out your copyright notice in the Description page of Project Settings.
#include "NodePath.h"

// Sets default values for this component's properties
UNodePath::UNodePath()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UNodePath::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UNodePath::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

