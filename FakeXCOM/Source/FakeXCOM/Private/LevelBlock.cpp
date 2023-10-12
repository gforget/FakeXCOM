// Fill out your copyright notice in the Description page of Project Settings.

#include "FakeXCOM/Public/LevelBlock.h"

#include "LevelEditor.h"
#include "Selection.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ALevelBlock::ALevelBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
#if WITH_EDITOR
	FLevelEditorModule& levelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(FName(TEXT("LevelEditor")));
	levelEditor.OnActorSelectionChanged().AddUObject(this, &ALevelBlock::OnActorSelected);
	
	//USelection::SelectionChangedEvent.AddUObject(this, &ALevelBlock::OnObjectSelected);
	//USelection::SelectObjectEvent.AddUObject(this, &ALevelBlock::OnObjectSelected);
#endif
}

// Called when the game starts or when spawned
void ALevelBlock::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelBlock::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateNodePathVisualisation();
}

#if WITH_EDITOR
void ALevelBlock::OnActorSelected(const TArray<UObject*>& objectsSelected, bool value)
{
	for (int i=0; i<objectsSelected.Num();i++)
	{
		if (objectsSelected[i] == this)
		{
			GenerateNodePathVisualisation();
		}
		else if (!objectsSelected[i]->IsA(ALevelBlock::StaticClass()))
		{
			UKismetSystemLibrary::FlushPersistentDebugLines(this);
		}
	}
}
#endif

void ALevelBlock::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateNodePathVisualisation();
}

// void ALevelBlock::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
// {
// 	Super::PostEditChangeChainProperty(PropertyChangedEvent);
// 	GenerateNodePathVisualisation();
// }

void ALevelBlock::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateNodePathVisualisation();
}

void ALevelBlock::Destroyed()
{
	Super::Destroyed();
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void ALevelBlock::GenerateNodePathVisualisation()
{
#if WITH_EDITOR
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
	
	UWorld* World = GetWorld();
	float Radius = 20.0f;
	int32 Segments = 12;
	FColor Color = FColor::Cyan;
	float Duration = 9999.9f; 
	float Thickness = 0.0f;
	FVector ActorLocation = GetActorLocation();
	for (int i=0; i<NodePathPositions.Num(); i++)
	{
		DrawDebugSphere(World, ActorLocation + NodePathPositions[i], Radius, Segments, Color, false, Duration, 0, Thickness);
	}
#endif
}

// Called every frame
void ALevelBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

