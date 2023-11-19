// Fill out your copyright notice in the Description page of Project Settings.


#include "UI3DManager.h"

#include "Cover3DIcon.h"
#include "DebugHeader.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
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
	}
}

void UUI3DManager::OnUnitSelected(AUnit* Unit)
{
	ClearDistanceLimitUI();
	CreateDistanceLimitUI(Unit);
}

void UUI3DManager::CreateDistanceLimitUI(AUnit* Unit)
{
	if (TBTacticalGameMode && BaseDistanceLimitIconClass)
	{
		TArray<UNodePath*> AllLongDistanceNode;
		TArray<UNodePath*> AllBaseDistanceNode;
		
		TBTacticalGameMode->TilePathFinder->GetNodeDistanceLimitForUnit(Unit, AllBaseDistanceNode, AllLongDistanceNode);

		for (int i=0; i<AllBaseDistanceNode.Num(); i++)
		{
			AllPathDistance3DIcons.Add(GetWorld()->SpawnActor<AActor>(BaseDistanceLimitIconClass,
				AllBaseDistanceNode[i]->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
				AllBaseDistanceNode[i]->GetComponentRotation()
				));
		}
		
		for (int i=0; i<AllLongDistanceNode.Num(); i++)
		{
			AllPathDistance3DIcons.Add(GetWorld()->SpawnActor<AActor>(LongDistanceLimitIconClass,
				AllLongDistanceNode[i]->GetComponentLocation() + FVector(0.0f,0.0f,0.5f),
				AllLongDistanceNode[i]->GetComponentRotation()
				));
		}
	}
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



