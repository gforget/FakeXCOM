// Fill out your copyright notice in the Description page of Project Settings.


#include "Cover3DIcon.h"

ACover3DIcon::ACover3DIcon()
{
	PrimaryActorTick.bCanEverTick = false;
	HalfShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Half Shield Icon"));
	FullShield = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Icon"));

	// Create a RootComponent as a SceneComponent
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	RootComponent->SetMobility(EComponentMobility::Movable);

	HalfShield->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FullShield->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACover3DIcon::BeginPlay()
{
	Super::BeginPlay();
	HalfShield->SetVisibility(false);
	FullShield->SetVisibility(false);
}

