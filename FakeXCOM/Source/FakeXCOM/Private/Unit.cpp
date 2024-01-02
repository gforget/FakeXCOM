// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"
#include "AbilitySystemComponent.h"
#include "AIBrainComponent.h"
#include "Gun.h"
#include "LevelBlock.h"
#include "NodePath.h"
#include "UnitAttributeSet.h"
#include "TBTacticalGameMode.h"
#include "TileMovementComponent.h"
#include "TilePathFinder.h"
#include "UI3DManager.h"
#include "UnitAbility.h"
#include "UnitManager.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AUnit::AUnit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleComponent->SetCapsuleHalfHeight(88.0f);
	CapsuleComponent->SetCapsuleRadius(34.0f);
	CapsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
	
	RootComponent = CapsuleComponent;
	RootComponent->SetMobility(EComponentMobility::Movable);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SkeletalMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	TileMovementComponent = CreateDefaultSubobject<UTileMovementComponent>(TEXT("Tile Movement Component"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));

	AIBrainComponent = CreateDefaultSubobject<UAIBrainComponent>(TEXT("AI Brain Component"));
}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	
	if (GunClass)
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		Gun->AttachToComponent(SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
		Gun->SetOwner(this);
	}
	else
	{
		DebugScreen("No Gun Set to this soldier", FColor::Red);
	}

	SightSurroundDefendingAnchor.Add(SightStartingAnchor);
	SightSurroundDefendingAnchor.Add(FVector(SightStartingAnchor.X + DistanceSurroundDefendingAnchor, SightStartingAnchor.Y, SightStartingAnchor.Z));
	SightSurroundDefendingAnchor.Add(FVector(SightStartingAnchor.X, SightStartingAnchor.Y + DistanceSurroundDefendingAnchor, SightStartingAnchor.Z));
	SightSurroundDefendingAnchor.Add(FVector(SightStartingAnchor.X - DistanceSurroundDefendingAnchor, SightStartingAnchor.Y, SightStartingAnchor.Z));
	SightSurroundDefendingAnchor.Add(FVector(SightStartingAnchor.X, SightStartingAnchor.Y - DistanceSurroundDefendingAnchor, SightStartingAnchor.Z));

	SightSurroundTargetingAnchor.Add(SightStartingAnchor);
	SightSurroundTargetingAnchor.Add(FVector(SightStartingAnchor.X + DistanceSurroundTargetingAnchor, SightStartingAnchor.Y, SightStartingAnchor.Z));
	SightSurroundTargetingAnchor.Add(FVector(SightStartingAnchor.X, SightStartingAnchor.Y + DistanceSurroundTargetingAnchor, SightStartingAnchor.Z));
	SightSurroundTargetingAnchor.Add(FVector(SightStartingAnchor.X - DistanceSurroundTargetingAnchor, SightStartingAnchor.Y, SightStartingAnchor.Z));
	SightSurroundTargetingAnchor.Add(FVector(SightStartingAnchor.X, SightStartingAnchor.Y - DistanceSurroundTargetingAnchor, SightStartingAnchor.Z));
}

UUnitAbility* AUnit::GetAbilityFromHandle(FGameplayAbilitySpecHandle AbilityHandle) const
{
	if (AbilitySystemComponent)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(AbilityHandle))
		{
			if (UGameplayAbility* AbilityInstance = AbilitySpec->Ability)
			{
				return Cast<UUnitAbility>(AbilityInstance);
			}
		}
	}
	return nullptr;
}
void AUnit::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateEditorAnchorPositionVisualisation();	
}

void AUnit::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateEditorAnchorPositionVisualisation();
}

void AUnit::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateEditorAnchorPositionVisualisation();
}

void AUnit::Destroyed()
{
	Super::Destroyed();
	UKismetSystemLibrary::FlushPersistentDebugLines(this);
}

void AUnit::GenerateEditorAnchorPositionVisualisation() const
{
#if WITH_EDITOR
	if (const UWorld* World = GetWorld())
	{
		if (World->WorldType == EWorldType::EditorPreview)
		{
			UKismetSystemLibrary::FlushPersistentDebugLines(this);
			const FVector ActorLocation = GetActorLocation();
			
			// HealtBar Anchor
			DrawDebugSphere(GetWorld(), ActorLocation + HealthBarAnchor, 5.0f, 12, FColor::Cyan, true, 0.0f, 0, 0.0f);

			// SightStartingPosition Anchor
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor, 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);

			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X + DistanceSurroundTargetingAnchor, SightStartingAnchor.Y, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X, SightStartingAnchor.Y + DistanceSurroundTargetingAnchor, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X - DistanceSurroundTargetingAnchor, SightStartingAnchor.Y, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X, SightStartingAnchor.Y - DistanceSurroundTargetingAnchor, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);

			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X + DistanceSurroundDefendingAnchor, SightStartingAnchor.Y, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X, SightStartingAnchor.Y + DistanceSurroundDefendingAnchor, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X - DistanceSurroundDefendingAnchor, SightStartingAnchor.Y, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X, SightStartingAnchor.Y - DistanceSurroundDefendingAnchor, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
		}
	}
#endif
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UAbilitySystemComponent* AUnit::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUnit::Initialize()
{
	// Define the line trace parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	
	// Perform the line trace
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = GetActorLocation() + FVector(0.0f,0.0f, -100.0f);

	if (bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		if (HitResult.GetActor())
		{
			if (ALevelBlock* LevelBlockPtr = Cast<ALevelBlock>(HitResult.GetActor()))
			{
				TArray<UNodePath*> AllNodePaths;
				LevelBlockPtr->GetComponents<UNodePath>(AllNodePaths);
				UNodePath* StartingNodePtr = LevelBlockPtr->GetClosestNodePathFromLocation(HitResult.Location);
				TileMovementComponent->LocatedNodePath = StartingNodePtr;
				StartingNodePtr->bIsBlocked = true;
			}
		}
	}

	//Adding Reference to this unit
	TBTacticalGameMode->UnitManager->AddUnitToManager(IdUnit, this);
	TBTacticalGameMode->LevelUIRef->SubscribeToUnitEvent(this);
	TBTacticalGameMode->UI3DManagerComponent->SubscribeToUnitEvent(this);
	
	const UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (IsValid(ASC))
	{
		UnitAttributeSet = ASC->GetSet<UUnitAttributeSet>();
		for (int i=0; i<OwnedAbilitiesClasses.Num();i++)
		{
			const FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(OwnedAbilitiesClasses[i], 0, -1));
			UUnitAbility* CurrentAbility = GetAbilityFromHandle(Handle);
			OwnedAbilities.Add(CurrentAbility);
			OwnedAbilitiesHandle.Add(Handle);
			CurrentAbility->OnAbilityAssigned(TBTacticalGameMode, IdUnit);
		}
	}
}

void AUnit::CallRanOutOfActions()
{
	OnUnitRanOutOfActionsEvent.Broadcast(this);
}

void AUnit::CallHealthChanged()
{
	OnUnitHealthChangeEvent.Broadcast(this);
}

float AUnit::GetTargetCoverDefenceBonus(AUnit* Target)
{
	const TArray<FCoverInfo> AllCoverInfo = Target->TileMovementComponent->LocatedNodePath->AllCoverInfos;

	for (int i=0; i<AllCoverInfo.Num(); i++)
	{
		FVector2D CoverPosition = FVector2D(AllCoverInfo[i].IconPosition.X, AllCoverInfo[i].IconPosition.Y);
		FVector2D TargetPosition = FVector2D(Target->GetActorLocation().X, Target->GetActorLocation().Y);
		
		FVector2D DeltaTargetToCoverNormalized = CoverPosition - TargetPosition;
		DeltaTargetToCoverNormalized.Normalize();

		FVector2D DeltaUnitToTargetNormalized = TargetPosition - FVector2D(GetActorLocation().X, GetActorLocation().Y);
		DeltaUnitToTargetNormalized.Normalize();

		const float DotProduct = FVector2D::DotProduct(DeltaTargetToCoverNormalized, DeltaUnitToTargetNormalized);
		if (DotProduct < 0.0f)
		{
			return AllCoverInfo[i].FullCover ? Target->FullCoverDefenceBonus: Target->LowCoverDefenceBonus; 
		}
	}

	return 0.0f;
}

float AUnit::GetHeightAdvantageBonus(AUnit* Target)
{
	float TargetZPosition = Target->GetActorLocation().Z;
	float UnitZPosition = GetActorLocation().Z;
	
	return UnitZPosition - TargetZPosition > 100.0f ? Target->LowGroundDisadvantage : 0.0f;
}

void AUnit::SetIsDead(bool Val)
{
	bIsDead = Val;
	if (bIsDead)
	{
		CapsuleComponent->DestroyComponent(false);
		OnUnitIsDeadEvent.Broadcast(this);
	}
}

bool AUnit::GetIsDead()
{
	return bIsDead;
}

void AUnit::MoveToNodePath(UNodePath* TargetNodePath)
{
	if (!TargetNodePath->bIsBlocked && TBTacticalGameMode->TilePathFinder->bCanMoveUnit)
	{
		const int BaseDistance = UnitAttributeSet->GetMaxMoveDistancePerAction();
		const int AllowedDistance = BaseDistance*UnitAttributeSet->GetActions();
					
		GenericStack<UNodePath*> Path = TBTacticalGameMode->TilePathFinder->GetPathToDestination(
			TileMovementComponent->LocatedNodePath,
			TargetNodePath);
					
		if (Path.Num() > 0
			&& TargetNodePath->NbSteps != -1
			&& TargetNodePath->NbSteps <= AllowedDistance
			&& UnitAttributeSet->GetActions() > 0)
		{
			OnUnitOrderedToMoveEvent.Broadcast(this);
			MovementActionCost(TargetNodePath);
			
			TBTacticalGameMode->TilePathFinder->MoveUnit(this, Path);
		}
	}
}

void AUnit::MovementActionCost(const UNodePath* Destination)
{
	const int BaseDistance = UnitAttributeSet->GetMaxMoveDistancePerAction();
	const int ActionCost = Destination->NbSteps > BaseDistance ? 2 : 1;
	UnitAttributeSet->SetActions(UnitAttributeSet->GetActions()-ActionCost);
}

