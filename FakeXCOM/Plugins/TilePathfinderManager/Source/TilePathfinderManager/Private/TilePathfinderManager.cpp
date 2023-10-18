// Copyright Epic Games, Inc. All Rights Reserved.

#include "TilePathfinderManager.h"

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
	MenuBuilder.AddMenuEntry(FText::FromString(TEXT("Option 1")),
		FText::FromString(TEXT("Safely delete unused asset under folder")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FTilePathfinderManagerModule::OnPathfinderMenuButtonClicked)
		);
}

void FTilePathfinderManagerModule::OnPathfinderMenuButtonClicked()
{
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
				// Check if the component exists in the actor
				if (UNodePath* ComponentToRemove = LevelBlock->FindComponentByClass<UNodePath>())
				{
					// Remove the component from the actor
					ComponentToRemove->UnregisterComponent();
					ComponentToRemove->DestroyComponent();
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
					FHitResult HitResult;
					FCollisionQueryParams CollisionParams;
					FVector Start = LevelBlock->GetActorLocation() + LevelBlock->NodePathPositions[j];
					FVector End = Start + (LevelBlock->GetActorUpVector() * 50.0f);

					bool bHit = WorldPtr->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

					if (bHit)
					{
						//DrawDebugLine(WorldPtr, Start, End, FColor::Red, true, 0.0f, 0.0f, 10);
					}
					else
					{
						//DrawDebugLine(WorldPtr, Start, End, FColor::Green, true, 0.0f, 0.0f, 10);
						
						//add a nodepath on the levelblock
						if (UClass* ComponentClass = UNodePath::StaticClass())
						{
							UNodePath* NodePathComponent = NewObject<UNodePath>(LevelBlock, FName("NodePathComponent"));
							NodePathComponent->RegisterComponent();
							LevelBlock->AddInstanceComponent(NodePathComponent);
							NodePathComponent->WorldLocation = LevelBlock->GetActorLocation() + LevelBlock->NodePathPositions[j];
							AllNodePathGenerated.Add(NodePathComponent);
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
		UKismetSystemLibrary::FlushPersistentDebugLines(objectsSelected[0]);
		for (int i=0; i<objectsSelected.Num();i++)
		{
			if (objectsSelected[i]->IsA(ALevelBlock::StaticClass()))
			{
				UWorld* World = objectsSelected[i]->GetWorld();
				float Radius = 20.0f;
				int32 Segments = 12;
				FColor Color = FColor::Cyan;
				float Duration = 0.0f; 
				float Thickness = 0.0f;
				FVector ActorLocation = Cast<ALevelBlock>(objectsSelected[i])->GetActorLocation();
				for (int j=0; j<Cast<ALevelBlock>(objectsSelected[i])->NodePathPositions.Num(); j++)
				{
					DrawDebugSphere(World, ActorLocation + Cast<ALevelBlock>(objectsSelected[i])->NodePathPositions[j], Radius, Segments, Color, true, Duration, 0, Thickness);
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