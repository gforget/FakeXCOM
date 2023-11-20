﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "UI3DManager.h"

#include "Cover3DIcon.h"
#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"
#include "UnitManager.h"

UUI3DManager::UUI3DManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUI3DManager::BeginPlay()
{
	Super::BeginPlay();
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
}

void UUI3DManager::Initialize()
{
	if (TBTacticalGameMode)
	{
		TBTacticalGameMode->UnitManager->OnUnitSelectedEvent.AddDynamic(this, &UUI3DManager::OnUnitSelected);
		TBTacticalGameMode->UnitManager->OnUnitOrderedToMoveEvent.AddDynamic(this, &UUI3DManager::OnUnitOrderedToMove);
		TBTacticalGameMode->MainController->MouseSceneSelectionComponent->OnMouseOverActorEvent.AddDynamic(this, &UUI3DManager::OnMouseOverActor);
	}
}

void UUI3DManager::OnUnitSelected(AUnit* Unit)
{
	ClearDistanceLimitUI();
	CreateDistanceLimitUI(Unit);
}

void UUI3DManager::OnUnitOrderedToMove(AUnit* Unit)
{
	ClearPath3DIcons();
	ClearDistanceLimitUI();
	ConnectCover3DIconsToUnit(Unit->IdUnit);
}

void UUI3DManager::OnMouseOverActor(AActor* Actor, FVector HitLocation)
{
	if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(Actor))
	{
		UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation);
		if (ChosenNodePath && (!CurrentMouseOverNodePath || CurrentMouseOverNodePath->IdNode != ChosenNodePath->IdNode))
		{
			CurrentMouseOverNodePath = ChosenNodePath;
			ClearPath3DIcons();

			if (UTilePathFinder* TilePathFinderPtr = TBTacticalGameMode->TilePathFinder)
			{
				GenericStack<UNodePath*> PathStack = TilePathFinderPtr->GetPathToDestination(
						TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit()->TileMovementComponent->LocatedNodePath,
						CurrentMouseOverNodePath
						);
				
				while(!PathStack.IsEmpty())
				{
					const UNodePath* CurrentNodePath = PathStack.Pop();
					AddPath3DIcon(CurrentNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.6f), CurrentNodePath->GetComponentRotation());
				}
			}
		
			//Create Select 3d Icon
			ClearSelect3DIcon();
			AddSelect3DIcon(CurrentMouseOverNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.5f), CurrentMouseOverNodePath->GetComponentRotation());
			
			//Create Cover 3D Icon
			ClearCover3DIcons();
			for (int i=0; i<CurrentMouseOverNodePath->AllCoverInfos.Num(); i++)
			{
				AddCover3DIcon(CurrentMouseOverNodePath->AllCoverInfos[i].IconPosition,CurrentMouseOverNodePath->AllCoverInfos[i].IconRotation, CurrentMouseOverNodePath->AllCoverInfos[i].FullCover);
			}
		}
	}
}

void UUI3DManager::CreateDistanceLimitUI(AUnit* Unit)
{
	if (TBTacticalGameMode && BaseDistanceLimitIconClass)
	{
		int BaseDistance;
		int LongDistance;
		TArray<UNodePath*> AllLongDistanceNode;
		TArray<UNodePath*> AllBaseDistanceNode;
		
		TBTacticalGameMode->TilePathFinder->GetNodeDistanceLimitForUnit(Unit,
			AllBaseDistanceNode,
			AllLongDistanceNode,
			BaseDistance,
			LongDistance);

		for (int i=0; i<AllBaseDistanceNode.Num(); i++)
		{
			if (TrySpawnIcon(AllBaseDistanceNode[i], BaseDistance))
			{
				AllPathDistance3DIcons.Add(GetWorld()->SpawnActor<AActor>(BaseDistanceLimitIconClass,
					AllBaseDistanceNode[i]->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
					AllBaseDistanceNode[i]->GetComponentRotation()
					));
			}
		}
		
		for (int i=0; i<AllLongDistanceNode.Num(); i++)
		{
			if (TrySpawnIcon(AllLongDistanceNode[i], LongDistance))
			{
				AllPathDistance3DIcons.Add(GetWorld()->SpawnActor<AActor>(LongDistanceLimitIconClass,
                	AllLongDistanceNode[i]->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
                	AllLongDistanceNode[i]->GetComponentRotation()
                	));
			}
		}
		
	}
}
bool UUI3DManager::TrySpawnIcon(const UNodePath* NodePath, int DistanceLimit)
{
	bool bSpawnIcon = false;

	//0 to 3 index are all vertical neighbour, which are the only one we are interested in
	for (int i=0; i<4; i++)
	{
		if (!NodePath->AllNeighboursConnectionInfo[i])
		{
			bSpawnIcon = true;
			break;
		}
		
		if (NodePath->AllNeighboursConnectionInfo[i])
		{
			if (NodePath->AllNeighboursConnectionInfo[i]->NbSteps > DistanceLimit)
			{
				bSpawnIcon = true;
				break;
			}
		}
	}

	return bSpawnIcon;
}

void UUI3DManager::ClearDistanceLimitUI()
{
	if (AllPathDistance3DIcons.Num() > 0)
	{
		for (int i=0; i<AllPathDistance3DIcons.Num(); i++)
		{
			AllPathDistance3DIcons[i]->Destroy();
		}
		AllPathDistance3DIcons.Empty();
	}
}

void UUI3DManager::ClearCover3DIcons()
{
	if (AllMouseOverCover3DIcon.Num() > 0)
	{
		for (int i=0; i<AllMouseOverCover3DIcon.Num(); i++)
		{
			AllMouseOverCover3DIcon[i]->Destroy();
		}
		AllMouseOverCover3DIcon.Empty();
	}
}

void UUI3DManager::AddCover3DIcon(FVector Location, FRotator Rotation, bool bFullCover)
{
	if (Cover3DIconClass)
	{
		ACover3DIcon* Cover3DIcon = GetWorld()->SpawnActor<ACover3DIcon>(Cover3DIconClass,
		Location,
		Rotation);

		if (bFullCover)
		{
			Cover3DIcon->FullShield->SetVisibility(true);
		}
		else
		{
			Cover3DIcon->HalfShield->SetVisibility(true);
		}
					
		AllMouseOverCover3DIcon.Add(Cover3DIcon);
	}
}

void UUI3DManager::ConnectCover3DIconsToUnit(int IdUnit)
{
	if (!AllAssignCover3DIcon.Contains(IdUnit))
	{
		AllAssignCover3DIcon.Add(IdUnit, FAssignCover3DIcon());
	}
					
	if (AllAssignCover3DIcon[IdUnit].Entries.Num() > 0)
	{
		for (int i=0; i<AllAssignCover3DIcon[IdUnit].Entries.Num(); i++)
		{
			AllAssignCover3DIcon[IdUnit].Entries[i]->Destroy();
		}
		AllAssignCover3DIcon[IdUnit].Entries.Empty();
	}
					
	AllAssignCover3DIcon[IdUnit].Entries.Append(AllMouseOverCover3DIcon);
	AllMouseOverCover3DIcon.Empty();
}

void UUI3DManager::ClearSelect3DIcon()
{
	if (Select3DIcon)
	{
		Select3DIcon->Destroy();
		Select3DIcon = nullptr;
	}	
}

void UUI3DManager::AddSelect3DIcon(FVector Location, FRotator Rotation)
{
	if (Select3DIconClass)
	{
		Select3DIcon = GetWorld()->SpawnActor<AActor>(Select3DIconClass, Location, Rotation);
	}
}

void UUI3DManager::ClearPath3DIcons()
{
	if (AllPath3DIcons.Num() > 0)
	{
		for (int i=0; i<AllPath3DIcons.Num();i++)
		{
			AllPath3DIcons[i]->Destroy();
		}
		AllPath3DIcons.Empty();
	}
}

void UUI3DManager::AddPath3DIcon(FVector Location, FRotator Rotation)
{
	if (Path3DIconClass)
	{
		AllPath3DIcons.Add(GetWorld()->SpawnActor<AActor>(Path3DIconClass, Location,Rotation));
	}
}



