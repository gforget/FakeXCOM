// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBrainComponent.h"

#include "AbilitySystemComponent.h"
#include "AIAbility.h"
#include "Consideration.h"
#include "DebugHeader.h"
#include "NodePath.h"
#include "TBTacticalGameMode.h"
#include "UnitAbility.h"
#include "UtilityMatrixDataTable.h"

// Sets default values for this component's properties
UAIBrainComponent::UAIBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UAIBrainComponent::BeginPlay()
{
	Super::BeginPlay();
	TBTacticalGameMode = GetOwner()->GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	if (TBTacticalGameMode)
	{
		TBTacticalGameMode->UnitManager->OnUnitSelectedEvent.AddDynamic(this, &UAIBrainComponent::OnUnitSelected);
		OwningUnit = Cast<AUnit>(GetOwner());

		//Assign all AIAbilities in the UM
		TArray<FUtilityMatrixDT*> AllUMRows;
		UtilityMatrix->GetAllRows(TEXT("UAIBrainComponent::BeginPlay"), AllUMRows);
		for (int i=0; i<AllUMRows.Num(); i++)
		{
			OwningUnit->AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AllUMRows[i]->ActionAbility, 0, -1));
		}
	}
}

void UAIBrainComponent::OnUnitSelected(AUnit* Unit)
{
	if (OwningUnit->IdUnit == Unit->IdUnit)
	{
		if (TBTacticalGameMode->FactionManagerComponent->FactionsController[Unit->Faction] == AIController)
		{
			//Delay taking the decision so it avoid certain problem with
			//the synchronization of event of the turn system.
			
			GetWorld()->GetTimerManager().SetTimer(
			DecideActionTimerHandle,
			this,
			&UAIBrainComponent::DecideActionTimerFunction,
			DelayBetweenDecision
			);
		}
	}
}

void UAIBrainComponent::DecideActionTimerFunction()
{
	OwningUnit->AbilitySystemComponent->TryActivateAbilityByClass(DecideBestAction());
}

TSubclassOf<UAIAbility> UAIBrainComponent::DecideBestAction()
{
	// Get all rows if you need them all
	TArray<FUtilityMatrixDT*> AllUMRows;
	UtilityMatrix->GetAllRows(TEXT("UAIBrainComponent::DecideBestAction"), AllUMRows);
	TArray<FName> AllRowsName = UtilityMatrix->GetRowNames();
	
	//Get the best action
	float score = 0.0f;
	int nextBestActionIndex = 0;
	bool bDebugScore = false;
	
	for (int i=0; i<AllUMRows.Num(); i++)
	{
		if (!AllUMRows[i]->IsEnabled) continue;
		
		//Check if you need a chosen node path first
		if (AllUMRows[i]->TargetType == EAIAbilityTargetType::NodePath)
		{
			TargetNodePath = PickNodePath(AllUMRows[i]);
		}
		
		//Check if you need a target actor first
		if (AllUMRows[i]->TargetType == EAIAbilityTargetType::Actor)
		{
			TargetActorIndex = PickTargetActor(AllUMRows[i]);
		}

		//score the action next
		float NewScore = ScoreAction(AllUMRows[i]->ActionConsiderations);
		if (NewScore > score)
		{
			nextBestActionIndex = i;
			score = NewScore;
		}

		if (bDebugScore)
		{
			FString ScoreString = FString::Printf(TEXT("%.3f"), NewScore);
			FString CompleteDebugString =  AllRowsName[i].ToString() + " : " + ScoreString;
			DebugScreen(CompleteDebugString, FColor::Cyan);
		}
	}
	
	if (bDebugScore)
	{
		DebugScreen("------------", FColor::Cyan);
	}
	
	return AllUMRows[nextBestActionIndex]->ActionAbility; 
}

float UAIBrainComponent::ScoreAction(TArray<UConsideration*> Considerations)
{
	float score = 1.0f;
	
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		float considerationScore = Considerations[i]->ScoreConsideration(OwningUnit, OwningUnit->TBTacticalGameMode);
		score *= considerationScore;
		
		if (score == 0) 
		{
			return score; //no point computing further
		}
	}
	
	// averaging scheme of overall score
	// in other word : create a more usefull value due to fact that score is aggregated by multiplying decimal, which make the value lower and lower

	float originalScore = score;
	float modFactor = 1 - (1 / Considerations.Num());
	float makeupValue = (1 - originalScore) * modFactor;
	score = originalScore + (makeupValue * originalScore);
	
	return score;
}

UNodePath* UAIBrainComponent::PickNodePath(FUtilityMatrixDT* UMRow)
{
	float score = 0.0f;
	int  chosenNodeIndex = 0;
	bool bDebugNodeScore = false;
	
	TArray<UNodePath*> AllValidNode;
	AllValidNode.Append(AllBaseDistanceNode);
	AllValidNode.Append(AllLongDistanceNode);
	
	for (int i=0; i<AllValidNode.Num(); i++)
	{
		float NewScore = ScoreNodePath(UMRow->TargetConsiderations, AllValidNode[i]);
		
		//use r.DebugSafeZone.MaxDebugTextStringsPerActor 30000 to extend the number of character you can see
		if (bDebugNodeScore)
		{
			FString ScoreString = FString::Printf(TEXT("%.3f"), NewScore);
			DrawDebugString(GetWorld(), AllValidNode[i]->GetComponentLocation(), ScoreString, 0,FColor::Red, DelayBetweenDecision, false, 1);
		}
		
		if ( NewScore > score)
		{
			chosenNodeIndex = i;
			score = NewScore;
		}
	}
	
	if (bDebugNodeScore)
	{
		FString ScoreString = FString::Printf(TEXT("%.3f"), score);
		DrawDebugString(GetWorld(), AllValidNode[chosenNodeIndex]->GetComponentLocation(), ScoreString, 0,FColor::Purple, DelayBetweenDecision, false, 1);
	}
	
	return AllValidNode[chosenNodeIndex];
}

float UAIBrainComponent::ScoreNodePath(TArray<UConsideration*> Considerations, UNodePath* Node)
{
	float score = 1.0f;
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		const float considerationScore = Considerations[i]->ScoreConsiderationNode(
			OwningUnit,
			OwningUnit->TBTacticalGameMode,
			Node);
		score *= considerationScore;
		
		if (score == 0) 
		{
			return score; //no point computing further
		}
	}
	
	// averaging scheme of overall score
	// in other word : create a more usefull value due to fact that score is aggregated by multiplying decimal, which make the value lower and lower

	float originalScore = score;
	float modFactor = 1 - (1 / Considerations.Num());
	float makeupValue = (1 - originalScore) * modFactor;
	score = originalScore + (makeupValue * originalScore); 


	return score;
}

int UAIBrainComponent::PickTargetActor(FUtilityMatrixDT* UMRow)
{
	float score = 0.0f;
	int  chosenActorIndex = -1;
	
	TArray<AActor*> AllTarget = TBTacticalGameMode->TargetManager->AllCurrentAvailableTarget;
	for (int i=0; i<AllTarget.Num(); i++)
	{
		float NewScore = ScoreTargetActor(UMRow->TargetConsiderations, AllTarget[i]);
		if (NewScore > score)
		{
			chosenActorIndex = i;
			score = NewScore;
		}
	}

	return chosenActorIndex;
}

float UAIBrainComponent::ScoreTargetActor(TArray<UConsideration*> Considerations, AActor* Actor)
{
	float score = 1.0f;
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		const float considerationScore = Considerations[i]->ScoreConsiderationActor(
			OwningUnit,
			OwningUnit->TBTacticalGameMode,
			Actor);
		score *= considerationScore;
		
		if (score == 0) 
		{
			return score; //no point computing further
		}
	}
	
	// averaging scheme of overall score
	// in other word : create a more usefull value due to fact that score is aggregated by multiplying decimal, which make the value lower and lower

	float originalScore = score;
	float modFactor = 1 - (1 / Considerations.Num());
	float makeupValue = (1 - originalScore) * modFactor;
	score = originalScore + (makeupValue * originalScore);
	
	return score;
}






