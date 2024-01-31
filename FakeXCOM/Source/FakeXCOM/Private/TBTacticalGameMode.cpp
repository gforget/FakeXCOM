// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "TBTacticalGameMode.h"

#include "ActorsObject/LevelBlock.h"
#include "ActorsObject/Unit.h"
#include "Blueprint/UserWidget.h"
#include "Controller/MouseSceneSelectionComponent.h"
#include "Controller/TBTacticalMainController.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/TargetManager.h"
#include "Managers/TurnManager.h"
#include "Managers/UI3DManager.h"
#include "Managers/UnitAbilityManager.h"
#include "Managers/UnitManager.h"
#include "Pathfinder/NodePath.h"
#include "Pathfinder/TilePathFinder.h"
#include "UI/LevelUI.h"

ATBTacticalGameMode::ATBTacticalGameMode()
{
	UI3DManagerComponent = CreateDefaultSubobject<UUI3DManager>(TEXT("UI 3D Manager Component"));
	FactionManagerComponent = CreateDefaultSubobject<UFactionManager>(TEXT("Faction Manager Component"));
	TurnManagerComponent = CreateDefaultSubobject<UTurnManager>(TEXT("Turn Manager Component"));
}

void ATBTacticalGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Setup LevelUI
	if (LevelUIClass)
	{
		LevelUIRef = CreateWidget<ULevelUI>(GetWorld(), LevelUIClass);
		LevelUIRef->AddToViewport();
	}
	
	//Setup UObject
	UnitAbilityManager = NewObject<UUnitAbilityManager>(GetTransientPackage(), UUnitAbilityManager::StaticClass());
	
	TilePathFinder = NewObject<UTilePathFinder>(GetTransientPackage(), UTilePathFinder::StaticClass());
	TilePathFinder->SubscribeGameModeEvent(this);
	
	UnitManager = NewObject<UUnitManager>(GetTransientPackage(), UUnitManager::StaticClass());

	TargetManager = NewObject<UTargetManager>(GetTransientPackage(), UTargetManager::StaticClass());

	//Initialize component and manager
	MainController->MouseSceneSelectionComponent->Initialize();
	UnitManager->Initialize(this);
	TargetManager->Initialize(this);
	UI3DManagerComponent->Initialize();
	LevelUIRef->Initialization();
	
	//For some reason, assigning pointer generated here on other object result on them being deleted or not recognise later
	TArray<AActor*> AllActors;

#if WITH_EDITOR
	const UWorld* WorldPtr = GEditor->GetEditorWorldContext().World();
#else
	const UWorld* WorldPtr = GetWorld();
#endif
	
	UGameplayStatics::GetAllActorsOfClass(WorldPtr,AActor::StaticClass(),AllActors);

	int FirstUnitToSelectId = -1;
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
				
				if (LevelBlockPtr->bIsSpawningPosition)
				{
					for (int j=0; j<LevelBlockPtr->SpawningNodePathIndexes.Num(); j++)
					{
						const UNodePath* StartingNodePtr = AllNodePaths[LevelBlockPtr->SpawningNodePathIndexes[j]];
						
						if (LevelBlockPtr->UnitClasses[j] && j < LevelBlockPtr->SpawningRotation.Num())
						{
							CurrentIdUnit++;
							AUnit* UnitPtr = GetWorld()->SpawnActor<AUnit>(LevelBlockPtr->UnitClasses[j],
								StartingNodePtr->GetComponentLocation() + FVector(0.0f,0.0f,88.0f),
								FRotator(0.0f, LevelBlockPtr->SpawningRotation[j], 0.0f))
							;
							
							UnitPtr->IdUnit = CurrentIdUnit;
							
							if (UnitPtr->Faction == EFaction::XCOM)
							{
								if (FirstUnitToSelectId == -1)
								{
									FirstUnitToSelectId = CurrentIdUnit;
								}
								UnitPtr->UnitName = TEXT("Soldier " + FString::FromInt(CurrentIdUnit));
							}
							else if (UnitPtr->Faction == EFaction::Alien)
							{
								UnitPtr->UnitName = TEXT("Alien Soldier " + FString::FromInt(CurrentIdUnit));
							}
							
							UnitPtr->Initialize();
						}
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
		
		bInitialized = true;
		if (FirstUnitToSelectId != -1)
		{
			UnitManager->SelectUnit(FirstUnitToSelectId);
			TurnManagerComponent->OnTurnStartedEvent.Broadcast(TurnManagerComponent->GetSelectedFaction());
		}
	}
}


