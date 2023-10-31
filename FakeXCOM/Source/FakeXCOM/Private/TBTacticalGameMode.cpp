// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTacticalGameMode.h"

#include "LevelBlock.h"
#include "TilePathFinder.h"
#include "NodePath.h"
#include "Soldier.h"
#include "TileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void ATBTacticalGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	//Setup TilePathFinder
	TilePathFinder = NewObject<UTilePathFinder>(GetTransientPackage(), UTilePathFinder::StaticClass());
	TArray<AActor*> AllActors;
	const UWorld* WorldPtr = GEditor->GetEditorWorldContext().World();
	UGameplayStatics::GetAllActorsOfClass(WorldPtr,AActor::StaticClass(),AllActors);

	if (WorldPtr && AllActors.Num() > 0)
	{
		UKismetSystemLibrary::FlushPersistentDebugLines(AllActors[0]);
		
		//delete all nodepath present on the map
		for (int i=0; i<AllActors.Num(); i++)
		{
			if (const ALevelBlock* LevelBlock = Cast<ALevelBlock>(AllActors[i]))
			{
				if (LevelBlock->bIsStartingPosition)
				{
					TArray<UNodePath*> AllNodePaths;
					LevelBlock->GetComponents<UNodePath>(AllNodePaths);
					UNodePath* StartingNodePtr = AllNodePaths[LevelBlock->NodePathIndex];
					
					if (SoldierClass)
					{
						const ASoldier* SoldierPtr = GetWorld()->SpawnActor<ASoldier>(SoldierClass, StartingNodePtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f), FRotator(0.0f, 90.0f, 0.0f));
						SoldierPtr->TileMovementComponent->LocatedNodePath = StartingNodePtr;
					}
				}
			}
		}
	}
	
	bInitialized = true;
}
