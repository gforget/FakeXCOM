// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionManager.h"


// Sets default values for this component's properties
UFactionManager::UFactionManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	FactionsController.Add(EFaction::XCOM, EAssignedController::PlayerController);
	FactionsController.Add(EFaction::Alien, EAssignedController::AIController);
	FactionsController.Add(EFaction::Resistance, EAssignedController::AIController);
	FactionsController.Add(EFaction::Civilian, EAssignedController::AIController);
}

EFactionRelation UFactionManager::GetFactionRelation(EFaction FactionA, EFaction FactionB)
{
	switch(FactionA)
	{
		case XCOM:
			switch(FactionB)
			{
				case XCOM:
					return OwnFaction;
				case Alien:
					return Enemy;
				case Resistance:
					return Ally;
				case Civilian:
					return Neutral;
			}
			break;
		case Alien:
			switch(FactionB)
			{
			case XCOM:
				return Enemy;
			case Alien:
				return OwnFaction;
			case Resistance:
				return Enemy;
			case Civilian:
				return Neutral;
				}
			break;
		case Resistance:
			switch(FactionB)
			{
			case XCOM:
				return Ally;
			case Alien:
				return Enemy;
			case Resistance:
				return OwnFaction;
			case Civilian:
				return Neutral;
				}
			break;
		case Civilian:
			switch(FactionB)
			{
			case XCOM:
				return Neutral;
			case Alien:
				return Neutral;
			case Resistance:
				return Neutral;
			case Civilian:
				return OwnFaction;
			}
			break;
	}
	
	return Neutral;
}

void UFactionManager::BeginPlay()
{
	Super::BeginPlay();
}

