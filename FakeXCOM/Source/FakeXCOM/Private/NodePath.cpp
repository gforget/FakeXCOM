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
	
	FVector LevelBlockPosition = GetComponentLocation() + (LevelBlockPtr->GetActorUpVector() * 50.0f);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;

	//Get Neighbour in all 8 directions
	TArray<FVector> DirectionVectors;

	//vertical direction
	DirectionVectors.Add(LevelBlockPtr->GetActorForwardVector() * 100.0f); // top
	DirectionVectors.Add(LevelBlockPtr->GetActorRightVector() * 100.0f); //right
	DirectionVectors.Add(-LevelBlockPtr->GetActorForwardVector() * 100.0f); //bottom
	DirectionVectors.Add(-LevelBlockPtr->GetActorRightVector() * 100.0f); //left

	//diagonal direction, diagonal distance is greater. Need to normalize to get the correct distance
	FVector diag1 = (LevelBlockPtr->GetActorForwardVector() + LevelBlockPtr->GetActorRightVector()); // top-right
	diag1.Normalize();
	FVector diag2 = (-LevelBlockPtr->GetActorForwardVector() + LevelBlockPtr->GetActorRightVector()); // bottom-right
	diag2.Normalize();
	FVector diag3 = (-LevelBlockPtr->GetActorForwardVector() - LevelBlockPtr->GetActorRightVector()); // bottom-left
	diag3.Normalize();
	FVector diag4 = (LevelBlockPtr->GetActorForwardVector() - LevelBlockPtr->GetActorRightVector()); // top left
	diag4.Normalize();
	
	DirectionVectors.Add( diag1 * 141.42f);
	DirectionVectors.Add(diag2 * 141.42f);
	DirectionVectors.Add(diag3 * 141.42f);
	DirectionVectors.Add(diag4 * 141.42f);

	TArray<bool> foundBlock;
	foundBlock.Init(false, 4);
	
	TArray<bool> foundVoid;
	foundVoid.Init(false, 4);
	
	for (int i=0;i<DirectionVectors.Num();i++)
	{
		FVector Start = LevelBlockPosition + DirectionVectors[i];
		FVector End = Start - LevelBlockPtr->GetActorUpVector()*151.0f;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
		
		if (bHit)
		{
			if (ALevelBlock* NeighbourLevelBlock = Cast<ALevelBlock>(HitResult.GetActor()))
			{
				//check if there is a at least one level of block on that direction up from the floor level of that block
				if (HitResult.Distance <= 50 && i<4)
				{
					foundBlock[i] = true;
				}
				
				TArray<UNodePath*> AllNodePaths;
				NeighbourLevelBlock->GetComponents<UNodePath>(AllNodePaths);

				if (AllNodePaths.Num() > 0)
				{
					
					// Get the closest node path to the hit, this will be the targeted neighbour
					TArray<UNodePath*> SortedNodePaths = AllNodePaths;
					FVector HitLocation = HitResult.Location;
					
					SortedNodePaths.Sort([HitLocation](const UNodePath& A, const UNodePath& B) {
						return FVector::DistSquared2D(A.GetComponentLocation(), HitLocation) < FVector::DistSquared2D(B.GetComponentLocation(), HitLocation);
					});

					bool bConnectNeighbour = true;
					if (i >= 4) // check if diagonal
					{
							int index1 = i-4; // movement clockwise for the index
							int index2 = i==7 ? 0 : i-3; // the clock reset to 0 at the end

							// there is one neighbour of that diagonal tht is either a full block or a void
							if (foundBlock[index1] || foundVoid[index1] || foundBlock[index2] || foundVoid[index2])
							{
								bConnectNeighbour = false;
							}

							if (GetComponentLocation().Z != SortedNodePaths[0]->GetComponentLocation().Z)
							{
								bConnectNeighbour = false;
							}
					}
					
					if (bConnectNeighbour)
					{
						AllNeighbours.Add(SortedNodePaths[0]);
						nbNeighbour = AllNeighbours.Num();	
					}
				}
			}
		}
		else
		{
			if (i<4)
			{
				foundVoid[i] = true;	
			}
		}
	}
}

// Called when the game starts
void UNodePath::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UNodePath::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

