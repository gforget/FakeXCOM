// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "ActorsObject/Unit.h"

#include "AbilitySystemComponent.h"
#include "TBTacticalGameMode.h"

#include "ActorsObject/Gun.h"
#include "ActorsObject/LevelBlock.h"
#include "AI/AIBrainComponent.h"
#include "AttributeSets/UnitAttributeSet.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameplayAbilities/UnitAbility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/UI3DManager.h"
#include "Managers/UnitManager.h"
#include "Pathfinder/NodePath.h"
#include "Pathfinder/TilePathFinder.h"
#include "Pathfinder/TileMovementComponent.h"
#include "Utility/DebugHeader.h"

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

	SightSurroundFlankTargetingAnchor.Add(SightStartingAnchor);
	SightSurroundFlankTargetingAnchor.Add(FVector(SightStartingAnchor.X + DistanceSurroundFlankTargetingAnchor, SightStartingAnchor.Y, SightStartingAnchor.Z));
	SightSurroundFlankTargetingAnchor.Add(FVector(SightStartingAnchor.X, SightStartingAnchor.Y + DistanceSurroundFlankTargetingAnchor, SightStartingAnchor.Z));
	SightSurroundFlankTargetingAnchor.Add(FVector(SightStartingAnchor.X - DistanceSurroundFlankTargetingAnchor, SightStartingAnchor.Y, SightStartingAnchor.Z));
	SightSurroundFlankTargetingAnchor.Add(FVector(SightStartingAnchor.X, SightStartingAnchor.Y - DistanceSurroundFlankTargetingAnchor, SightStartingAnchor.Z));
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

#if WITH_EDITOR
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
#endif

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

			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X + DistanceSurroundFlankTargetingAnchor, SightStartingAnchor.Y, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X, SightStartingAnchor.Y + DistanceSurroundFlankTargetingAnchor, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X - DistanceSurroundFlankTargetingAnchor, SightStartingAnchor.Y, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			DrawDebugSphere(GetWorld(), ActorLocation + SightStartingAnchor + FVector(SightStartingAnchor.X, SightStartingAnchor.Y - DistanceSurroundFlankTargetingAnchor, 0.0f), 5.0f, 12, FColor::Orange, true, 0.0f, 0, 0.0f);
			
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
			if (!OwnedAbilities.Contains(CurrentAbility->AbilityId))
			{
				OwnedAbilities.Add(CurrentAbility->AbilityId, CurrentAbility);
				OwnedAbilitiesHandle.Add(CurrentAbility->AbilityId, Handle);
				UnitAbilityInfos.Add(CurrentAbility->AbilityId, FUnitAbilityInfoStruct());
			}
			else
			{
				DebugScreen(GetName() + " Unit got 2 or more abilities with the same ID :" + CurrentAbility->AbilityId + ". Please give these abilities different ID", FColor::Red);
			}
			
			CurrentAbility->OnAbilityAssigned(TBTacticalGameMode, this);
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

float AUnit::GetTargetCoverDefenceBonus(AUnit* TargetUnit, UNodePath* UnitNode, UNodePath* TargetNode)
{
	if (CHECK_NULL_POINTER(TargetUnit)) return 0.0f;
	if (CHECK_NULL_POINTER(UnitNode)) return 0.0f;
	if (CHECK_NULL_POINTER(TargetNode)) return 0.0f;
	
	const TArray<FCoverInfo> AllCoverInfo = TargetNode->AllCoverInfos;
	
	bool bCoverIsEffective = false;
	FCoverInfo CoverInfoToUse;
	
	for (int i=0; i<AllCoverInfo.Num(); i++)
	{
		FVector2D CoverPosition = FVector2D(AllCoverInfo[i].IconPosition.X, AllCoverInfo[i].IconPosition.Y);
		FVector2D TargetPosition = FVector2D(TargetNode->GetComponentLocation().X, TargetNode->GetComponentLocation().Y);
		
		FVector2D DeltaTargetToCoverNormalized = CoverPosition - TargetPosition;
		DeltaTargetToCoverNormalized.Normalize();
		
		// if one check fail, then cover not effective
		bool bCurrentCoverTest = true;
		for (int j=0; j<SightSurroundFlankTargetingAnchor.Num(); j++)
		{
			FVector2D DeltaUnitToTargetNormalized = TargetPosition - (FVector2D(UnitNode->GetComponentLocation().X, UnitNode->GetComponentLocation().Y) + FVector2D(SightSurroundFlankTargetingAnchor[j].X, SightSurroundFlankTargetingAnchor[j].Y));
			DeltaUnitToTargetNormalized.Normalize();
			
			const float DotProduct = FVector2D::DotProduct(DeltaTargetToCoverNormalized, DeltaUnitToTargetNormalized);
			//if (DotProduct >= -0.25f)
			if (DotProduct >= -0.1f)
			{
				bCurrentCoverTest = false;
			}
		}
		
		if (bCurrentCoverTest)
		{
			bCoverIsEffective = true;
			CoverInfoToUse = AllCoverInfo[i];
			break;
		}
	}

	if (bCoverIsEffective)
	{
		return CoverInfoToUse.FullCover ? TargetUnit->FullCoverDefenceBonus: TargetUnit->LowCoverDefenceBonus; 
	}
	else
	{
		return 0.0f;
	}
}

float AUnit::GetHeightAdvantageBonus(AUnit* Target)
{
	if (CHECK_NULL_POINTER(Target)) return 0.0f;
	
	float TargetZPosition = Target->GetActorLocation().Z;
	float UnitZPosition = GetActorLocation().Z;

	float HeightAdvantageScore = 0.0f;
	if (UnitZPosition - TargetZPosition >= 100.0f)
	{
		HeightAdvantageScore = Target->LowGroundDisadvantage;
	} else if (UnitZPosition - TargetZPosition <= -100.0f)
	{
		HeightAdvantageScore = -Target->LowGroundDisadvantage;
	}
	
	return HeightAdvantageScore;
}

void AUnit::SetIsDead(bool Val)
{
	bIsDead = Val;
	if (bIsDead)
	{
		OnUnitIsDeadEvent.Broadcast(this);
	}
}

bool AUnit::GetIsDead()
{
	return bIsDead;
}

void AUnit::MoveToNodePath(UNodePath* TargetNodePath)
{
	if (CHECK_NULL_POINTER(TargetNodePath)) return;
	
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

void AUnit::DestroyCapsule()
{
	CapsuleComponent->DestroyComponent(false);
}

void AUnit::MovementActionCost(const UNodePath* Destination)
{
	if (CHECK_NULL_POINTER(Destination)) return;
	
	const int BaseDistance = UnitAttributeSet->GetMaxMoveDistancePerAction();
	const int ActionCost = Destination->NbSteps > BaseDistance ? 2 : 1;
	UnitAttributeSet->SetActions(UnitAttributeSet->GetActions()-ActionCost);
}

bool AUnit::TryActivateAbilityByID(FString AbilityID, bool ForceActivation)
{
	if (ForceActivation)
	{
		return GetAbilitySystemComponent()->TryActivateAbility(OwnedAbilitiesHandle[AbilityID]);
	}
	
	if (CheckAbilityById(AbilityID))
	{
		return GetAbilitySystemComponent()->TryActivateAbility(OwnedAbilitiesHandle[AbilityID]);
	}
	else
	{
		return false;
	}
}

bool AUnit::CheckAbilityById(FString AbilityID)
{
	if (!OwnedAbilities.Contains(AbilityID))
	{
		return false;
	}
	
	UUnitAbility* AbilityToActivate = OwnedAbilities[AbilityID];
	if (!AbilityToActivate)
	{
		return false;
	}

	if (AbilityToActivate->GetIsDisabled(this) || AbilityToActivate->GetIsHidden(this))
	{
		return false;
	}
	
	return true;
}
