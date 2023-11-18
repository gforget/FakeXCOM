// Fill out your copyright notice in the Description page of Project Settings.

#include "TBTacticalGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LevelBlock.h"
#include "LevelUI.h"
#include "NodePath.h"
#include "TBTacticalMainController.h"
#include "TilePathFinder.h"
#include "UI3DManager.h"
#include "Unit.h"
#include "Blueprint/UserWidget.h"

ATBTacticalGameMode::ATBTacticalGameMode()
{
	UI3DManagerComponent = CreateDefaultSubobject<UUI3DManager>(TEXT("UI 3D Manager Component"));
}

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
		int CurrentIdUnit = -1;
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
					if (UnitClass)
					{
						CurrentIdUnit++;
						AUnit* UnitPtr = GetWorld()->SpawnActor<AUnit>(UnitClass, StartingNodePtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f), FRotator(0.0f, 90.0f, 0.0f));
						UnitPtr->IdUnit = CurrentIdUnit;
						UnitPtr->UnitName = TEXT("Soldier " + FString::FromInt(CurrentIdUnit));
						UnitPtr->Initialize();
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
		
		SelectUnit(CurrentIdUnit);
	}
	
	bInitialized = true;
}

void ATBTacticalGameMode::SelectNextUnit()
{
	//TODO: create a loop to select soldier who turn didn't finish and can perform action this turn
	//Soldier are never removed from the list, even when dead
	
	SelectedUnitId++;
	if (SelectedUnitId >= AllUnitReference.Num())
	{
		SelectedUnitId = 0;
	}

	SelectUnit(SelectedUnitId);
}

void ATBTacticalGameMode::SelectPreviousUnit()
{
	SelectedUnitId--;
	if (SelectedUnitId < 0)
	{
		SelectedUnitId = AllUnitReference.Num()-1;
	}
	SelectUnit(SelectedUnitId);
}

AUnit* ATBTacticalGameMode::SelectUnit(int UnitId)
{
	DebugScreen("New Soldier Selected !", FColor::Yellow);

	SelectedUnitId = UnitId;
	MainController->GoToActor(AllUnitReference[SelectedUnitId]);
	OnUnitSelectedEvent.Broadcast(AllUnitReference[SelectedUnitId]);
	return AllUnitReference[SelectedUnitId];
}

AUnit* ATBTacticalGameMode::GetCurrentlySelectedUnit()
{
	if (SelectedUnitId != -1)
	{
		return AllUnitReference[SelectedUnitId];	
	}
	
	return nullptr;
}
