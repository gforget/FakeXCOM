// Fill out your copyright notice in the Description page of Project Settings.


#include "TBTacticalGameMode.h"

#include "LevelBlock.h"
#include "TilePathFinder.h"
#include "NodePath.h"
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
			if (ALevelBlock* LevelBlock = Cast<ALevelBlock>(AllActors[i]))
			{
				if (LevelBlock->bIsStartingPosition)
				{
					TArray<UNodePath*> AllNodePaths;
					LevelBlock->GetComponents<UNodePath>(AllNodePaths);
					
					TilePathFinder->StartingNode = AllNodePaths[LevelBlock->NodePathIndex];
				}
			}
		}
	}
	
	DrawDebugSphere(GetWorld(), TilePathFinder->StartingNode->GetComponentLocation(), 20.0f, 12, FColor::Cyan, true, 0.0f, 0, 0.0f);
	bInitialized = true;
}
