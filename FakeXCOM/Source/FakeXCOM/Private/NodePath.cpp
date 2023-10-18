// Fill out your copyright notice in the Description page of Project Settings.
#include "NodePath.h"
#include "LevelBlock.h"

// Sets default values for this component's properties
UNodePath::UNodePath()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNodePath::Initialize()
{
	nbNeighbour = 0;
	ALevelBlock* LevelBlockPtr = Cast<ALevelBlock>(GetOwner());
	
	FVector LevelBlockPosition = WorldLocation + (LevelBlockPtr->GetActorUpVector() * 50.0f);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	//Get Neighbour in all 8 directions
	TArray<FVector> DirectionVectors;

	//vertical direction
	DirectionVectors.Add(LevelBlockPtr->GetActorForwardVector() * 100.0f);
	DirectionVectors.Add(LevelBlockPtr->GetActorRightVector() * 100.0f);
	DirectionVectors.Add(LevelBlockPtr->GetActorForwardVector() * -100.0f);
	DirectionVectors.Add(LevelBlockPtr->GetActorRightVector() * -100.0f);

	//diagonal direction, diagonal distance is greater
	FVector diag1 = (LevelBlockPtr->GetActorForwardVector() + LevelBlockPtr->GetActorRightVector());
	diag1.Normalize();
	FVector diag2 = (LevelBlockPtr->GetActorForwardVector() - LevelBlockPtr->GetActorRightVector());
	diag2.Normalize();
	FVector diag3 = (-LevelBlockPtr->GetActorForwardVector() + LevelBlockPtr->GetActorRightVector());
	diag3.Normalize();
	FVector diag4 = (-LevelBlockPtr->GetActorForwardVector() - LevelBlockPtr->GetActorRightVector());
	diag4.Normalize();
	
	DirectionVectors.Add( diag1 * 141.42f);
	DirectionVectors.Add(diag2 * 141.42f);
	DirectionVectors.Add(diag3 * 141.42f);
	DirectionVectors.Add(diag4 * 141.42f);

	for (int i=0;i<DirectionVectors.Num();i++)
	{
		FVector Start = LevelBlockPosition + DirectionVectors[i];
		FVector End = Start - LevelBlockPtr->GetActorUpVector()*151.0f;
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
		if (bHit)
		{
			if (ALevelBlock* NeighbourLevelBlock = Cast<ALevelBlock>(HitResult.GetActor()))
			{
				DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 0.0f, 0.0f, 10);
				//If a nodepath was generated, then there is nothing blocking the path to it and the node is just one level above
				if (UNodePath* NeighbourNodePath = NeighbourLevelBlock->FindComponentByClass<UNodePath>())
				{
					AllNeighbours.Add(NeighbourNodePath);
					nbNeighbour = AllNeighbours.Num();
				}
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, true, 0.0f, 0.0f, 10);
		}
	}
	
	// for (int i=0;i<DirectionVectors.Num();i++)
	// {
	// 	FVector End = Start + DirectionVectors[i];
	// 	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	// 	if (bHit)
	// 	{
	// 		if (ALevelBlock* NeighbourLevelBlock = Cast<ALevelBlock>(HitResult.GetActor()))
	// 		{
	// 			//If a nodepath was generated, then there is nothing blocking the path to it and the node is just one level above
	// 			if (UNodePath* NeighbourNodePath = NeighbourLevelBlock->FindComponentByClass<UNodePath>())
	// 			{
	// 				AllNeighbours.Add(NeighbourNodePath);
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		//check if there is a valid levelblock at the same level or one level bellow
	// 		FVector End2 = End - LevelBlockPtr->GetActorUpVector() * 150.0f; //150 because node can connect to one level bellow
	// 		bool bHit2 = GetWorld()->LineTraceSingleByChannel(HitResult, End, End2, ECC_Visibility, CollisionParams);
	// 		if (bHit2)
	// 		{
	// 			if (ALevelBlock* NeighbourLevelBlock = Cast<ALevelBlock>(HitResult.GetActor()))
	// 			{
	// 				//If a nodepath was generated, then there is nothing blocking the path to it
	// 				if (UNodePath* NeighbourNodePath = NeighbourLevelBlock->FindComponentByClass<UNodePath>())
	// 				{
	// 					AllNeighbours.Add(NeighbourNodePath);
	// 				}
	// 			}
	// 		}
	// 	}
	// }

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

