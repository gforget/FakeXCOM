// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBrainComponent.h"
#include "UtilityMatrixDataTable.h"

// Sets default values for this component's properties
UAIBrainComponent::UAIBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UAIBrainComponent::BeginPlay()
{
	Super::BeginPlay();
	SetBrainActivation(false);
}

// Called every frame
void UAIBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAIBrainComponent::SetBrainActivation(bool value)
{
	bAIBrainActivated = value;
	SetComponentTickEnabled(value);
}

bool UAIBrainComponent::GetBrainActivation()
{
	return bAIBrainActivated;
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
		if (const float NewScore = ScoreAction(AllUMRows[i]->ActionConsiderations) > score)
		{
			nextBestActionIndex = i;
			score = NewScore;
		}
	}
	
	bFinishedDeciding = true;
	return AllUMRows[nextBestActionIndex]->ActionAbility; 
}

float UAIBrainComponent::ScoreAction(TArray<UConsideration*> Considerations)
{
	//TODO:: Create the consideration methods for scoring
	const float score = 1.0f;
	for (int i = 0; i < Considerations.Num(); i++) 
	{ 
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



