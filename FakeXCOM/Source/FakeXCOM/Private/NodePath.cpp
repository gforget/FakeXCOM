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
	
	//Connect the nodepath to all the neighbour nodepath around
	for (int i=0;i<DirectionVectors.Num();i++)
	{
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		
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

				//Get the node path closest to the raycast hit location
				if (UNodePath* ChosenNodePath = NeighbourLevelBlock->GetClosestNodePathFromLocation(HitResult.Location))
				{
					if (TryConnectNeighbour(i, DirectionVectors, foundBlock, foundVoid, ChosenNodePath, LevelBlockPtr, NeighbourLevelBlock))
					{
						AllNeighbours.Add(ChosenNodePath);
						nbNeighbour = AllNeighbours.Num();	
					}
				}
			}
		}
		else
		{
			if (i<4) // check if vertical
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

bool UNodePath::TryConnectNeighbour(int IndexDirection, TArray<FVector>& DirectionVectors, TArray<bool>& foundBlock, TArray<bool>& foundVoid, UNodePath* ChosenNodePath, ALevelBlock* LevelBlockPtr, ALevelBlock* NeighbourLevelBlock)
{
	//clearance
	if (IndexDirection >= 4) // if diagonal
	{
		int index1 = IndexDirection-4; // movement clockwise for the index
		int index2 = IndexDirection==7 ? 0 : IndexDirection-3; // the clock reset to 0 at the end

		// there is one neighbour of that diagonal tht is either a full block or a void, you should not connect to that node
		if (foundBlock[index1] || foundVoid[index1] || foundBlock[index2] || foundVoid[index2])
		{
			return false;
		}

		// in diagonal, you never connect to a lower or higher node
		if (GetComponentLocation().Z != ChosenNodePath->GetComponentLocation().Z)
		{
			return false;
		}
	}

	// slope clearance
	if ((NeighbourLevelBlock->IsSlope != LevelBlockPtr->IsSlope))
	{
		if (IndexDirection < 4) // if vertical
		{
			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			
			FVector LevelBlockPosition = GetComponentLocation() + (LevelBlockPtr->GetActorUpVector() * 50.0f);
			FVector DirectionUnitVector = DirectionVectors[IndexDirection];
			DirectionUnitVector.Normalize();
			
			if (LevelBlockPtr->IsSlope && !NeighbourLevelBlock->IsSlope) // Slope to non-slope
			{
				if (GetComponentLocation().Z < ChosenNodePath->GetComponentLocation().Z)
				{
					FVector2D DirectionToNode = FVector2D(ChosenNodePath->GetComponentLocation()) - FVector2D(GetComponentLocation());
					DirectionToNode.Normalize();
					
					FVector2D DirectionToTop =  FVector2D(LevelBlockPtr->GetActorLocation() + LevelBlockPtr->TopSlopePosition) - FVector2D(GetComponentLocation());
					DirectionToTop.Normalize();

					float DotProduct = DirectionToNode.Dot(DirectionToTop);
					if (DotProduct < 0.95f)
					{
						return false;
					}
				}
				else
				{
					FVector2D DirectionToNode = FVector2D(ChosenNodePath->GetComponentLocation()) - FVector2D(GetComponentLocation());
					DirectionToNode.Normalize();
					
					FVector2D DirectionToBottom =  FVector2D(LevelBlockPtr->GetActorLocation() + LevelBlockPtr->BottomSlopePosition) - FVector2D(GetComponentLocation());
					DirectionToBottom.Normalize();

					float DotProduct = DirectionToNode.Dot(DirectionToBottom);
					if (DotProduct < 0.95f)
					{
						return false;
					}
				}
			}
			else //Non-Slope to Slope
			{
				//check if the entrance of the slope is next to this node
				FVector Start = LevelBlockPosition + DirectionUnitVector*50.0f;
				FVector End = Start - LevelBlockPtr->GetActorUpVector()*51.0f;
		
				bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
				if (HitResult.Distance < 50.0f || !bHit)
				{
					return false;
				}		
			}
		}
		else
		{
			return false; //diagonal don't connect to slope
		}
	}

	return true;
}


// Called every frame
void UNodePath::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

