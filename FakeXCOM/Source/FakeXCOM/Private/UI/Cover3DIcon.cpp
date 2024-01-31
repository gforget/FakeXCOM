// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "UI/Cover3DIcon.h"

ACover3DIcon::ACover3DIcon()
{
	PrimaryActorTick.bCanEverTick = false;
	HalfShieldIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Half Shield Icon"));
	FullShieldIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Icon"));

	// Create a RootComponent as a SceneComponent
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	RootComponent->SetMobility(EComponentMobility::Movable);

	HalfShieldIcon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FullShieldIcon->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ACover3DIcon::BeginPlay()
{
	Super::BeginPlay();
	HalfShieldIcon->SetVisibility(false);
	FullShieldIcon->SetVisibility(false);
}

