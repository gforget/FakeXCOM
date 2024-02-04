// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "AI/AIBrainComponent.h"

#include "AbilitySystemComponent.h"
#include "TBTacticalGameMode.h"

#include "AI/Consideration.h"
#include "AI/UtilityMatrixDataTable.h"
#include "GameplayAbilities//AIAbility.h"
#include "GameplayAbilities/UnitAbility.h"
#include "Pathfinder/NodePath.h"
#include "Utility/DebugHeader.h"

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
			if (AllUMRows[i]->TargetType == EAIAbilityTargetType::NodePath)
			{
				TargetNodePath.Add(i, nullptr);
			}

			if (AllUMRows[i]->TargetType == EAIAbilityTargetType::Actor)
			{
				TargetActorIndex.Add(i, -1);
			}
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
	TSubclassOf<UAIAbility> ChosenAbility = DecideBestAction();
	if (ChosenAbility)
	{
		OwningUnit->AbilitySystemComponent->TryActivateAbilityByClass(ChosenAbility);
	}
}

TSubclassOf<UAIAbility> UAIBrainComponent::DecideBestAction()
{
	// Get all rows if you need them all
	TArray<FUtilityMatrixDT*> AllUMRows;
	UtilityMatrix->GetAllRows(TEXT("UAIBrainComponent::DecideBestAction"), AllUMRows);
	TArray<FName> AllRowsName = UtilityMatrix->GetRowNames();
	
	//Get the best action
	float score = 0.0f;
	int nextBestActionIndex = -1;
	bool bDebugScore = bLogActionScore;
	
	for (int i=0; i<AllUMRows.Num(); i++)
	{
		if (!AllUMRows[i]->IsEnabled)
		{
			continue;
		} 
		
		//Check if you need a chosen node path first
		if (AllUMRows[i]->TargetType == EAIAbilityTargetType::NodePath)
		{
			TargetNodePath[i] = PickNodePath(i, AllUMRows[i]);
		}
		
		//Check if you need a target actor first
		if (AllUMRows[i]->TargetType == EAIAbilityTargetType::Actor)
		{
			TargetActorIndex[i] = PickTargetActor(i, AllUMRows[i]);
		}

		//score the action next
		TArray<FString> DebugConsiderationString;
		float NewScore = ScoreAction(i, AllUMRows[i]->ActionConsiderations, DebugConsiderationString);
		if (NewScore > score)
		{
			nextBestActionIndex = i;
			score = NewScore;
		}

		if (bDebugScore)
		{
			FString AllDebugString =  AllRowsName[i].ToString() + " : " + FString::Printf(TEXT("%.3f"), NewScore);
			for (int j=0; j<DebugConsiderationString.Num(); j++)
			{
				AllDebugString += "\n     " + DebugConsiderationString[j];
			}
			
			DebugScreen(AllDebugString, FColor::Cyan);
		}
	}

	if (nextBestActionIndex == -1)
	{
		for (int i=0; i<AllUMRows.Num(); i++)
		{
			if (AllUMRows[i]->IsDefaultAction && AllUMRows[i]->IsEnabled)
			{
				nextBestActionIndex = i;
				break;
			}
		}
	}
	
	if (bDebugScore)
	{
		DebugScreen("Chosen Action : " + AllRowsName[nextBestActionIndex].ToString(), FColor::Cyan);
		DebugScreen("------------", FColor::Cyan);
	}

	ChosenActionIndex = nextBestActionIndex;
	if (nextBestActionIndex != -1)
	{
		return AllUMRows[nextBestActionIndex]->ActionAbility;
	}
	else
	{
		DebugScreen("AI Can't pick action, none of them are enabled and set to default", FColor::Red);
		return nullptr;
	}
}

float UAIBrainComponent::ScoreAction(int ActionIndex, TArray<UConsideration*> Considerations, TArray<FString>& DebugStrings)
{
	float score = 1.0f;
	
	if (Considerations.Num() == 0 ) return 0.0f;
	
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		float considerationScore = Considerations[i]->ScoreConsideration(OwningUnit, ActionIndex, OwningUnit->TBTacticalGameMode);
		score *= considerationScore;
		
		DebugStrings.Add(Considerations[i]->GetName() + " : " + FString::Printf(TEXT("%f"), considerationScore));

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

UNodePath* UAIBrainComponent::PickNodePath(int ActionIndex, FUtilityMatrixDT* UMRow)
{
	float score = 0.0f;
	int  chosenNodeIndex = 0;
	const bool bDebugNodeScore = bShowNodeScore;
	
	TArray<UNodePath*> AllValidNode;
	AllValidNode.Append(AllBaseDistanceNode);
	AllValidNode.Append(AllLongDistanceNode);
	
	for (int i=0; i<AllValidNode.Num(); i++)
	{
		float NewScore = ScoreNodePath(ActionIndex, UMRow->TargetConsiderations, AllValidNode[i]);
		
		//use r.DebugSafeZone.MaxDebugTextStringsPerActor 0 to extend the number of character you can see
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

float UAIBrainComponent::ScoreNodePath(int ActionIndex, TArray<UConsideration*> Considerations, UNodePath* Node)
{
	float score = 1.0f;
	if (Considerations.Num() == 0 ) return 0.0f;
	
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		const float considerationScore = Considerations[i]->ScoreConsiderationNode(
			OwningUnit,
			ActionIndex,
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

int UAIBrainComponent::PickTargetActor(int ActionIndex, FUtilityMatrixDT* UMRow)
{
	float score = 0.0f;
	int  chosenActorIndex = -1;

	const bool bDebugTargetActorScore = bShowTargetActorScore;
	
	TArray<AActor*> AllTarget = TBTacticalGameMode->TargetManager->AllCurrentAvailableTarget;
	for (int i=0; i<AllTarget.Num(); i++)
	{
		float NewScore = ScoreTargetActor(ActionIndex, UMRow->TargetConsiderations, AllTarget[i]);

		//use r.DebugSafeZone.MaxDebugTextStringsPerActor 0 to extend the number of character you can see
		if (bDebugTargetActorScore)
		{
			FString ScoreString = FString::Printf(TEXT("%.3f"), NewScore);
			DrawDebugString(GetWorld(), AllTarget[i]->GetActorLocation(), ScoreString, 0,FColor::Red, DelayBetweenDecision, false, 1);
		}
		
		if (NewScore > score)
		{
			chosenActorIndex = i;
			score = NewScore;
		}
	}
	
	if (bDebugTargetActorScore && chosenActorIndex != -1)
	{
		FString ScoreString = FString::Printf(TEXT("%.3f"), score);
		DrawDebugString(GetWorld(), AllTarget[chosenActorIndex]->GetActorLocation(), ScoreString, 0,FColor::Purple, DelayBetweenDecision, false, 1);
	}
	
	return chosenActorIndex;
}

float UAIBrainComponent::ScoreTargetActor(int ActionIndex, TArray<UConsideration*> Considerations, AActor* Actor)
{
	float score = 1.0f;
	if (Considerations.Num() == 0 ) return 0.0f;
	
	for (int i = 0; i < Considerations.Num(); i++) 
	{
		const float considerationScore = Considerations[i]->ScoreConsiderationActor(
			OwningUnit,
			ActionIndex,
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






