// Fill out your copyright notice in the Description page of Project Settings.


#include "UI3DManager.h"

#include "Cover3DIcon.h"
#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "NodePath.h"
#include "Select3DIcon.h"
#include "TBTacticalGameMode.h"
#include "TBTacticalMainController.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"
#include "UnitAbilityManager.h"
#include "UnitAttributeSet.h"
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
		TBTacticalGameMode->MainController->MouseSceneSelectionComponent->OnMouseOverActorEvent.AddDynamic(this, &UUI3DManager::OnMouseOverActor);
		TBTacticalGameMode->UnitAbilityManager->OnAbilitySelectionModeChangeEvent.AddDynamic(this, &UUI3DManager::OnAbilitySelectionModeChangeEvent);
	}
}

void UUI3DManager::SubscribeToUnitEvent(AUnit* Unit)
{
	Unit->OnUnitOrderedToMoveEvent.AddDynamic(this, &UUI3DManager::OnUnitOrderedToMove);
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
	if (!TBTacticalGameMode->TilePathFinder->bCanMoveUnit)
	{
		return;
	}
	
	if (ALevelBlock* SelectedLevelBlock = Cast<ALevelBlock>(Actor))
	{
		UNodePath* ChosenNodePath = SelectedLevelBlock->GetClosestNodePathFromLocation(HitLocation);
		if (ChosenNodePath && (!CurrentMouseOverNodePath || CurrentMouseOverNodePath->IdNode != ChosenNodePath->IdNode))
		{
			CurrentMouseOverNodePath = ChosenNodePath;
			ClearPath3DIcons();
			ClearSelect3DIcon();
			ClearCover3DIcons();
			
			const AUnit* SelectedUnit = TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit();
			const int BaseDistance = SelectedUnit->UnitAttributeSet->GetMaxMoveDistancePerAction();
			const int AllowedDistance = BaseDistance*SelectedUnit->UnitAttributeSet->GetActions();
			
			if (ChosenNodePath->NbSteps == -1 || ChosenNodePath->NbSteps > AllowedDistance)
			{
				return;
			}
			
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
			AddSelect3DIcon(
				CurrentMouseOverNodePath->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
				CurrentMouseOverNodePath->GetComponentRotation(),
				ChosenNodePath->NbSteps <= BaseDistance && AllowedDistance > BaseDistance
				);
			
			//Create Cover 3D Icon
			for (int i=0; i<CurrentMouseOverNodePath->AllCoverInfos.Num(); i++)
			{
				AddCover3DIcon(CurrentMouseOverNodePath->AllCoverInfos[i].IconPosition,
					CurrentMouseOverNodePath->AllCoverInfos[i].IconRotation,
					CurrentMouseOverNodePath->AllCoverInfos[i].FullCover);
			}
		}
	}
}

void UUI3DManager::OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue)
{
	if (AbilitySelectionModeValue)
	{
		ClearPath3DIcons();
		ClearSelect3DIcon();
		ClearCover3DIcons();
		ClearDistanceLimitUI();
	}
	else
	{
		CreateDistanceLimitUI(TBTacticalGameMode->UnitManager->GetCurrentlySelectedUnit());
	}
}

void UUI3DManager::CreateDistanceLimitUI(AUnit* Unit)
{
	if (TBTacticalGameMode && BaseDistanceLimitIconClass && LongDistanceLimitIconClass)
	{
		int BaseDistance;
		int LongDistance;
		TArray<UNodePath*> AllBaseDistanceNode;
		TArray<UNodePath*> AllLongDistanceNode;
		
		TBTacticalGameMode->TilePathFinder->GetNodeDistanceLimitForUnit(Unit,
			AllBaseDistanceNode,
			AllLongDistanceNode,
			BaseDistance,
			LongDistance);
		
		if (Unit->UnitAttributeSet->GetActions() == 2)
		{
			SpawnDistanceIcons(AllBaseDistanceNode, BaseDistance, BaseDistanceLimitIconClass);
			SpawnDistanceIcons(AllLongDistanceNode, LongDistance, LongDistanceLimitIconClass);
		}
		else if (Unit->UnitAttributeSet->GetActions() == 1)
		{
			SpawnDistanceIcons(AllBaseDistanceNode, BaseDistance, LongDistanceLimitIconClass);
		}
	}
}

void UUI3DManager::SpawnDistanceIcons(TArray<UNodePath*> AllNodes, int Distance, TSubclassOf<AActor> IconSubClass)
{
	for (int i=0; i<AllNodes.Num(); i++)
	{
		TArray<bool> CornerToSpawn;
		if (TrySpawnDistanceIcon(AllNodes[i], Distance, CornerToSpawn))
		{
			for (int j=0; j<CornerToSpawn.Num(); j++)
			{
				if (CornerToSpawn[j])
				{
					FRotator IconRotation = AllNodes[i]->GetComponentRotation();
					IconRotation.Yaw = 0.0f;
				
					AActor* Icon =GetWorld()->SpawnActor<AActor>(IconSubClass,
						AllNodes[i]->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
						IconRotation);

					//0 = down, 90 = left, 180 = top, 270 = right
					Icon->AddActorLocalRotation(FRotator(0.0f,90.0f + (90.0f*j),0.0f));
				
					AllPathDistance3DIcons.Add(Icon);
				}
			}
		}
	}
}

bool UUI3DManager::TrySpawnDistanceIcon(const UNodePath* NodePath, int DistanceLimit, TArray<bool>& CornerToSpawn)
{
	bool bSpawnIcon = false;
	CornerToSpawn.Init(false, 4);
	
	//0 to 3 index are all vertical neighbour, which are the only one we are interested in
	for (int i=0; i<4; i++)
	{
		if (!NodePath->AllNeighboursConnectionInfo[i])
		{
			bSpawnIcon = true;
			CornerToSpawn[i] = true;
		}
		
		if (NodePath->AllNeighboursConnectionInfo[i])
		{
			if (NodePath->AllNeighboursConnectionInfo[i]->NbSteps > DistanceLimit ||
				NodePath->AllNeighboursConnectionInfo[i]->bIsBlocked)
			{
				bSpawnIcon = true;
				CornerToSpawn[i] = true;
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
			Cover3DIcon->FullShieldIcon->SetVisibility(true);
		}
		else
		{
			Cover3DIcon->HalfShieldIcon->SetVisibility(true);
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

void UUI3DManager::AddSelect3DIcon(const FVector& Location, const FRotator& Rotation, bool bIsBaseDistance)
{
	if (Select3DIconClass)
	{
		Select3DIcon = GetWorld()->SpawnActor<ASelect3DIcon>(Select3DIconClass, Location, Rotation);
		if (bIsBaseDistance)
		{
			Select3DIcon->BaseDistanceIcon->SetVisibility(true);
		}
		else
		{
			Select3DIcon->LongDistanceIcon->SetVisibility(true);
		}
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



