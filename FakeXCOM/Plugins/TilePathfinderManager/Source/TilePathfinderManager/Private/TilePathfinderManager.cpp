// Copyright Epic Games, Inc. All Rights Reserved.
#if WITH_EDITOR

#include "TilePathfinderManager.h"

#include "DebugHeader.h"
#include "LevelBlock.h"
#include "LevelEditor.h"
#include "NodePath.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#define LOCTEXT_NAMESPACE "FTilePathfinderManagerModule"

void FTilePathfinderManagerModule::StartupModule()
{
	//Adding the menu PathfinderManager in the editor
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule> (TEXT("LevelEditor"));
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

	MenuExtender->AddMenuBarExtension(
	  "Help", 
	  EExtensionHook::After,
	  nullptr,
	  FMenuBarExtensionDelegate::CreateRaw(this, &FTilePathfinderManagerModule::AddMenu)
	);

	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

	//UI for selecting level block
	LevelEditorModule.OnActorSelectionChanged().AddRaw(this, &FTilePathfinderManagerModule::OnActorSelected);
	
}

void FTilePathfinderManagerModule::AddMenu(FMenuBarBuilder& MenuBuilder)
{
	MenuBuilder.AddPullDownMenu(
	  LOCTEXT("MenuLocKey", "Tile Pathfinder Manager"),
	  LOCTEXT("MenuTooltipKey", "Opens menu for My Plugin"),
	  FNewMenuDelegate::CreateRaw(this, &FTilePathfinderManagerModule::FillMenu),
	  FName(TEXT("TilePathfinderManager")),
	  FName(TEXT("TilePathfinderManager")));
}

void FTilePathfinderManagerModule::FillMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FText::FromString(TEXT("Generate Pathfinding Graph")),
		FText::FromString(TEXT("Generate the pathfinding graph using the LevelBlock on the map")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FTilePathfinderManagerModule::OnPathfinderMenuButtonClicked)
		);
}

void FTilePathfinderManagerModule::OnPathfinderMenuButtonClicked()
{
	int CurrentNodeId = 0;
	TArray<AActor*> AllActors;
	UWorld* WorldPtr = GEditor->GetEditorWorldContext().World();
	UGameplayStatics::GetAllActorsOfClass(WorldPtr,AActor::StaticClass(),AllActors);

	if (WorldPtr && AllActors.Num() > 0)
	{
		UKismetSystemLibrary::FlushPersistentDebugLines(AllActors[0]);
		
		//delete all nodepath present on the map
		for (int i=0; i<AllActors.Num(); i++)
		{
			if (ALevelBlock* LevelBlock = Cast<ALevelBlock>(AllActors[i]))
			{
				TArray<UNodePath*> AllNodePaths;
				LevelBlock->GetComponents<UNodePath>(AllNodePaths);
				for(int j=0; j<AllNodePaths.Num(); j++)
				{
					// Remove the component from the actor
					AllNodePaths[j]->UnregisterComponent();
					AllNodePaths[j]->DestroyComponent();
				}
			}
		}
		
		//generate nodepath
		TArray<UNodePath*> AllNodePathGenerated;
		for (int i=0; i<AllActors.Num(); i++)
		{
			if (ALevelBlock* LevelBlock = Cast<ALevelBlock>(AllActors[i]))
			{
				for (int j=0; j<LevelBlock->NodePathPositions.Num(); j++)
				{
					//Check if there is no block on top of the block
					FHitResult HitResult;
					FCollisionQueryParams CollisionParams;
					FVector Start = LevelBlock->GetActorLocation() + LevelBlock->NodePathPositions[j] + (LevelBlock->GetActorUpVector() * 110.0f);
					FVector End = Start - (LevelBlock->GetActorUpVector() * 90.0f);

					bool bHit = WorldPtr->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
					if (!bHit)
					{
						//add a nodepath on the levelblock
						if (UClass* ComponentClass = UNodePath::StaticClass())
						{
							FString name = "NodePath Component ";
							name.Append(FString::FromInt(j));
							
							UNodePath* NodePathComponent = NewObject<UNodePath>(LevelBlock, FName(name));
							NodePathComponent->IdNode = CurrentNodeId;
							NodePathComponent->CreationMethod = EComponentCreationMethod::Native;
							LevelBlock->AddInstanceComponent(NodePathComponent);
							NodePathComponent->SetWorldLocation(LevelBlock->GetActorLocation() + LevelBlock->NodePathPositions[j]);
							NodePathComponent->RegisterComponent();
							
							AllNodePathGenerated.Add(NodePathComponent);
							CurrentNodeId++;
						}
					}
				}
			}
		}
		
		//Initialize all the nodepath after they have been generated
		for (int i=0; i<AllNodePathGenerated.Num(); i++)
		{
			AllNodePathGenerated[i]->Initialize();
		}
	}
}

void FTilePathfinderManagerModule::OnActorSelected(const TArray<UObject*>& objectsSelected, bool value)
{
	if (objectsSelected.Num() > 0)
	{
		const UWorld* WorldPtr = GEditor->GetEditorWorldContext().World();
		if (!WorldPtr) return;
		
		UKismetSystemLibrary::FlushPersistentDebugLines(objectsSelected[0]);
		for (int i=0; i<objectsSelected.Num();i++)
		{
			
			if (objectsSelected[i]->IsA(ALevelBlock::StaticClass()))
			{
				const ALevelBlock* LevelBlockPtr = Cast<ALevelBlock>(objectsSelected[i]);
				
				TArray<UNodePath*> AllNodePaths;
				LevelBlockPtr->GetComponents<UNodePath>(AllNodePaths);
				
				for(int j=0; j<AllNodePaths.Num(); j++)
				{
					for (int k=0; k<AllNodePaths[j]->AllConnectedNeighbours.Num(); k++)
					{
						if (AllNodePaths[j]->AllConnectedNeighbours[k])
						{
							DrawDebugLine(WorldPtr, AllNodePaths[j]->GetComponentLocation() + FVector(0.0f, 0.0f, 25.0f), AllNodePaths[j]->AllConnectedNeighbours[k]->GetComponentLocation() + FVector(0.0f, 0.0f, 25.0f), FColor::Blue, true, 0.0f, 0.0f, 2.0f);
						}
						else
						{
							DebugScreen(TEXT("Neighbour node not cleaned up"), FColor::Red);
						}
					}
				}
				
				FVector ActorLocation = LevelBlockPtr->GetActorLocation();
				for (int j=0; j<LevelBlockPtr->NodePathPositions.Num(); j++)
				{
					DrawDebugSphere(objectsSelected[i]->GetWorld(), ActorLocation + LevelBlockPtr->NodePathPositions[j], 20.0f, 12, FColor::Cyan, true, 0.0f, 0, 0.0f);
				}
			}
		}
	}
}

void FTilePathfinderManagerModule::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTilePathfinderManagerModule, TilePathfinderManager)

#endif