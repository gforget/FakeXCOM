// Fill out your copyright notice in the Description page of Project Settings.


#include "Select3DIcon.h"

// Sets default values
ASelect3DIcon::ASelect3DIcon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BaseDistanceIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Distance Icon"));
	LongDistanceIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Long Distance Icon"));

	// Create a RootComponent as a SceneComponent
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	RootComponent->SetMobility(EComponentMobility::Movable);

	BaseDistanceIcon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	LongDistanceIcon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ASelect3DIcon::BeginPlay()
{
	Super::BeginPlay();
	BaseDistanceIcon->SetVisibility(false);
	LongDistanceIcon->SetVisibility(false);
}