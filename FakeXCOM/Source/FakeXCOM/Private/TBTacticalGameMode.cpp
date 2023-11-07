// Fill out your copyright notice in the Description page of Project Settings.

#include "TBTacticalGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LevelBlock.h"
#include "NodePath.h"
#include "Soldier.h"
#include "TilePathFinder.h"
#include "TileMovementComponent.h"

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
		
		for (int i=0; i<AllActors.Num(); i++)
		{
			if (const ALevelBlock* LevelBlockPtr = Cast<ALevelBlock>(AllActors[i]))
			{
				if (LevelBlockPtr->bIsStartingPosition)
				{
					TArray<UNodePath*> AllNodePaths;
					LevelBlockPtr->GetComponents<UNodePath>(AllNodePaths);
					const UNodePath* StartingNodePtr = AllNodePaths[LevelBlockPtr->NodePathIndex];
					
					if (SoldierClass)
					{
						GetWorld()->SpawnActor<ASoldier>(SoldierClass, StartingNodePtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f), FRotator(0.0f, 90.0f, 0.0f));
					}
				}
			}
		}
	}
	
	bInitialized = true;
}
