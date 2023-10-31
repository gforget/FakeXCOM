// Fill out your copyright notice in the Description page of Project Settings.


#include "Soldier.h"
#include "NodePath.h"
#include "TileMovementComponent.h"

// Sets default values
ASoldier::ASoldier()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TileMovementComponent = CreateDefaultSubobject<UTileMovementComponent>(TEXT("Tile Movement Component"));

	// if (UCharacterMovementComponent* YourComponent = FindComponentByClass<UCharacterMovementComponent>())
	// {
	//
	// 	YourComponent->DestroyComponent();
	// 	YourComponent->UnregisterComponent();
	// }
}

// Called when the game starts or when spawned
void ASoldier::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASoldier::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// if (bChangeDestination)
	// {
	// 	UNodePath* NodePathPtr = Path.Pop();
	// 	if (Path.Size() == 0) LocatedNodePath = NodePathPtr;
	// 	
	// 	Destination = NodePathPtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f);
	// 	MovementDirection = Destination - GetActorLocation();
	// 	MovementDirection.Normalize();
	// 	bChangeDestination = false;
	// 	
	// 	DrawDebugSphere(GetWorld(), Destination, 10.0f, 12, FColor::Red, false, 5.0f, 0, 1.0f);
	// }
	//
	// const FVector Delta = Destination - GetActorLocation();
	// DebugScreen(FString::Printf(TEXT("%f"), Delta.SizeSquared()), FColor::Green);
	//
	// if (Delta.SizeSquared() > 10.0f)
	// {
	// 	SetActorLocation(GetActorLocation() + MovementDirection*200.0f*DeltaTime);
	// }
	// else
	// {
	// 	if (Path.Size() > 0)
	// 	{
	// 		bChangeDestination = true;
	// 	}
	// }
}

// void ASoldier::FollowPath(const GenericStack<UNodePath*>& NewPath)
// {
// 	Path.Clear();
// 	Path = NewPath;
// 	bChangeDestination = true;
// }

