// Fill out your copyright notice in the Description page of Project Settings.

#include "TBTacticalGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LevelBlock.h"
#include "LevelUI.h"
#include "NodePath.h"
#include "Soldier.h"
#include "TilePathFinder.h"
#include "Blueprint/UserWidget.h"

void ATBTacticalGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Setup LevelUI
	if (LevelUIClass)
	{
		LevelUI = CreateWidget<ULevelUI>(GetWorld(), LevelUIClass);
		LevelUI->AddToViewport();
	}
	
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
				TArray<UNodePath*> AllNodePaths;
				LevelBlockPtr->GetComponents<UNodePath>(AllNodePaths);
				
				if (LevelBlockPtr->bIsStartingPosition)
				{
					const UNodePath* StartingNodePtr = AllNodePaths[LevelBlockPtr->NodePathIndex];
					if (SoldierClass)
					{
						ASoldier* SoldierPtr = GetWorld()->SpawnActor<ASoldier>(SoldierClass, StartingNodePtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f), FRotator(0.0f, 90.0f, 0.0f));
						SoldierPtr->IdUnit = CurrentIdUnit;
						SoldierPtr->Initialize();
						CurrentIdUnit++;
					}
				}
				
				//Set Top and Bottom position
				for (int j=0; j<AllNodePaths.Num(); j++)
				{
					if (AllNodePaths[j]->GetComponentLocation().X > TopPosition.X)
					{
						TopPosition.X = AllNodePaths[j]->GetComponentLocation().X;
					}
						
					if (AllNodePaths[j]->GetComponentLocation().Y > TopPosition.Y)
					{
						TopPosition.Y = AllNodePaths[j]->GetComponentLocation().Y;
					}
						
					if (AllNodePaths[j]->GetComponentLocation().X < BottomPosition.X)
					{
						BottomPosition.X = AllNodePaths[j]->GetComponentLocation().X;
					}
						
					if (AllNodePaths[j]->GetComponentLocation().Y < BottomPosition.Y)
					{
						BottomPosition.Y = AllNodePaths[j]->GetComponentLocation().Y;
					}
				}
			}
		}
	}
	
	bInitialized = true;
}

ASoldier* ATBTacticalGameMode::GetNextSoldier()
{
	//TODO: create a loop to select soldier who turn didn't finish and can perform action this turn
	//Soldier are never removed from the list, even when dead
	
	SelectedSoldierId++;
	if (SelectedSoldierId >= AllSoldierReference.Num())
	{
		SelectedSoldierId = 0;
	}
	
	return AllSoldierReference[SelectedSoldierId];
}

ASoldier* ATBTacticalGameMode::GetPreviousSoldier()
{
	SelectedSoldierId--;
	if (SelectedSoldierId < 0)
	{
		SelectedSoldierId = AllSoldierReference.Num()-1;
	}
	
	return AllSoldierReference[SelectedSoldierId];
}
