// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBrainComponent.h"

#include "AbilitySystemComponent.h"
#include "AIAbility.h"
#include "Consideration.h"
#include "TBTacticalGameMode.h"
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
	AtbTacticalGameMode = GetOwner()->GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	if (AtbTacticalGameMode)
	{
		AtbTacticalGameMode->UnitManager->OnUnitSelectedEvent.AddDynamic(this, &UAIBrainComponent::OnUnitSelected);
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
		if (AtbTacticalGameMode->FactionManagerComponent->FactionsController[Unit->Faction] == AIController)
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
	
	//Get the best action
	float score = 0.0f;
	int nextBestActionIndex = 0;
	
	for (int i=0; i<AllUMRows.Num(); i++)
	{
		//Check if you need a chosen node path first
		if (AllUMRows[i]->TargetType == EAIAbilityTargetType::NodePath)
		{
			TargetNodePath = PickNodePath(AllUMRows[i]);
		}
		
		//Check if you need a target actor first
		if (AllUMRows[i]->TargetType == EAIAbilityTargetType::Actor)
		{
			//TODO create function to select an actor target
		}

		//score the action next
		if (const float NewScore = ScoreAction(AllUMRows[i]->ActionConsiderations) > score)
		{
			nextBestActionIndex = i;
			score = NewScore;
		}
	}
	
	return AllUMRows[nextBestActionIndex]->ActionAbility; 
}

float UAIBrainComponent::ScoreAction(TArray<UConsideration*> Considerations)
{
	float score = 1.0f;
	
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		const float considerationScore = Considerations[i]->ScoreConsideration(OwningUnit, OwningUnit->TBTacticalGameMode);
		score *= considerationScore;
		
		// if (score == 0) 
		// {
		// 	action.score = 0;
		// 	return action.score; //no point computing further
		// }
	}
	
	// averaging scheme of overall score
	// in other word : create a more usefull value due to fact that score is aggregated by multiplying decimal, which make the value lower and lower

	// float originalScore = score;
	// float modFactor = 1 - (1 / action.considerations.Length);
	// float makeupValue = (1 - originalScore) * modFactor;
	// action.score = originalScore + (makeupValue * originalScore);
	//
	// return action.score;
	
	return score;
}

UNodePath* UAIBrainComponent::PickNodePath(FUtilityMatrixDT* UMRow)
{
	float score = 0.0f;
	int  chosenNodeIndex = 0;
	
	TArray<UNodePath*> AllValidNode;
	AllValidNode.Append(AllBaseDistanceNode);
	AllValidNode.Append(AllLongDistanceNode);
	
	for (int i=0; i<AllValidNode.Num(); i++)
	{
		if (const float NewScore = ScoreNodePath(UMRow->TargetConsiderations) > score)
		{
			chosenNodeIndex = i;
			score = NewScore;
		}
	}

	return AllValidNode[chosenNodeIndex];
}

float UAIBrainComponent::ScoreNodePath(TArray<UConsideration*> Considerations)
{
	const float score = 1.0f;
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		//TODO: make  ScoreConsideration for node path
		
		// float considerationScore = action.considerations[i].ScoreConsideration(npc);
		// score *= considerationScore;
		// if (score == 0) 
		// {
		// 	action.score = 0;
		// 	return action.score; //no point computing further
		// }
	}
	
	// averaging scheme of overall score
	// in other word : create a more usefull value due to fact that score is aggregated by multiplying decimal, which make the value lower and lower

	// float originalScore = score;
	// float modFactor = 1 - (1 / action.considerations.Length);
	// float makeupValue = (1 - originalScore) * modFactor;
	// action.score = originalScore + (makeupValue * originalScore);
	//
	// return action.score;
	
	return score;
}






