// Fill out your copyright notice in the Description page of Project Settings.

#include "TBTacticalGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LevelBlock.h"
#include "NodePath.h"
#include "Soldier.h"
#include "TilePathFinder.h"

void ATBTacticalGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	//Setup TilePathFinder
	TilePathFinder = NewObject<UTilePathFinder>(GetTransientPackage(), UTilePathFinder::StaticClass());

	//For some reason, assigning pointer generated here on other object result on them being deleted or not recognise later
	TArray<AActor*> AllActors;
	const UWorld* WorldPtr = GEditor->GetEditorWorldContext().World();
	UGameplayStatics::GetAllActorsOfClass(WorldPtr,AActor::StaticClass(),AllActors);

	if (WorldPtr && AllActors.Num() > 0)
	{
		int CurrentIdUnit = 0;
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
						ASoldier* SoldierPtr = GetWorld()->SpawnActor<ASoldier>(SoldierClass, StartingNodePtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f), FRotator(0.0f, 90.0f, 0.0f));
						SoldierPtr->IdUnit = CurrentIdUnit;
						CurrentIdUnit++;
					}
				}
			}
		}
	}
	
	bInitialized = true;
}
