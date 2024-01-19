// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUI.h"

#include "DebugHeader.h"
#include "Gun.h"
#include "TargetManager.h"
#include "TBTacticalGameMode.h"
#include "TurnManager.h"
#include "UnitAbilityManager.h"
#include "UnitManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "GameFramework/GameUserSettings.h"

bool ULevelUI::Initialize()
{
	Super::Initialize();
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	PlayerController = GetWorld()->GetFirstPlayerController();
	
	return true;
}

void ULevelUI::Initialization()
{
	if (TBTacticalGameMode)
	{
		TBTacticalGameMode->UnitManager->OnUnitSpawnedEvent.AddDynamic(this,  &ULevelUI::OnUnitSpawned);
		TBTacticalGameMode->UnitManager->OnUnitSelectedEvent.AddDynamic(this, &ULevelUI::OnUnitSelected);
		TBTacticalGameMode->TargetManager->OnTargetSelectedEvent.AddDynamic(this, &ULevelUI::OnTargetSelected);
		TBTacticalGameMode->TurnManagerComponent->OnTurnStartedEvent.AddDynamic(this, &ULevelUI::OnTurnStartedEvent);
		TBTacticalGameMode->UnitAbilityManager->OnAbilitySelectionModeChangeEvent.AddDynamic(this, &ULevelUI::OnAbilitySelectionModeChangeEvent);
	}
}

void ULevelUI::SubscribeToUnitEvent(AUnit* Unit)
{
	Unit->OnUnitHealthChangeEvent.AddDynamic(this, &ULevelUI::OnUnitHealthChange);
	Unit->Gun->OnGunAmmoChangeEvent.AddDynamic(this, &ULevelUI::OnGunAmmoChange);
	Unit->OnUnitIsDeadEvent.AddDynamic(this, &ULevelUI::OnUnitIsDead);
}

void ULevelUI::BPCallStatusEvent(AUnit* TargetUnit, EStatusType StatusType, float Number)
{
	CallStatusEvent(TargetUnit, StatusType, Number);
}

void ULevelUI::CallStatusEvent_Implementation(AUnit* TargetUnit, EStatusType StatusType, float Number)
{
}

void ULevelUI::BPCallCustomAlertStatusEvent(AUnit* TargetUnit, FColor Color, const FString& Text, float Number)
{
	CallCustomAlertStatusEvent(TargetUnit, Color, Text, Number);
}

void ULevelUI::CallCustomAlertStatusEvent_Implementation(AUnit* TargetUnit, FColor Color, const FString& Text, float Number)
{
}

void ULevelUI::SetControlInterfaceVisibility_Implementation(bool Val)
{
	
}

void ULevelUI::ShowTargetImage_Implementation(bool Val)
{
}

void ULevelUI::OnTargetSelected(int TargetIndex, AUnit* SelectingUnit)
{
	if (TBTacticalGameMode->FactionManagerComponent->FactionsController[SelectingUnit->Faction] == EAssignedController::PlayerController)
	{
		OnBPTargetSelectedEvent(TargetIndex);
	}
}
void ULevelUI::OnBPTargetSelectedEvent_Implementation(int TargetIndex)
{
}

void ULevelUI::OnUnitSelected(AUnit* Unit)
{
	OnBPUnitSelectedEvent(Unit);
}

void ULevelUI::OnBPUnitSelectedEvent_Implementation(AUnit* Unit)
{
}

void ULevelUI::OnUnitSpawned(AUnit* Unit)
{
	OnBPUnitSpawnedEvent(Unit);
}

void ULevelUI::OnBPUnitSpawnedEvent_Implementation(AUnit* Unit)
{
}

void ULevelUI::OnUnitIsDead(AUnit* DeadUnit)
{
	OnUnitIsDeadEvent(DeadUnit);
}

void ULevelUI::OnUnitHealthChange(AUnit* Unit)
{
	OnBPUnitHealthChangeEvent(Unit);
}

void ULevelUI::OnBPUnitHealthChangeEvent_Implementation(AUnit* Unit)
{
}

void ULevelUI::OnGunAmmoChange(AGun* Gun)
{
	OnBPGunAmmoChangeEvent(Gun);
}

void ULevelUI::OnBPGunAmmoChangeEvent_Implementation(AGun* Gun)
{
}

void ULevelUI::OnTurnStartedEvent(EFaction CurrentFaction)
{
	OnBPTurnStartedEvent(CurrentFaction);
}


void ULevelUI::OnBPTurnStartedEvent_Implementation(EFaction Faction)
{
}

void ULevelUI::OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue)
{
	OnBPAbilitySelectionModeChangeEvent(AbilitySelectionModeValue);
}

void ULevelUI::OnBPAbilitySelectionModeChangeEvent_Implementation(bool AbilitySelectionModeValue)
{
}



void ULevelUI::OnUnitIsDeadEvent_Implementation(AUnit* DeadUnit)
{
}

void ULevelUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const float ViewPortScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	
	for (const TPair<int, UOverlay*>& HealthBarAssoc : HealthBarAssociationMap)
	{
		FVector2d ScreenLocation;
		const AUnit* CurrentUnit = TBTacticalGameMode->UnitManager->AllUnitReference[HealthBarAssoc.Key];

		if (CurrentUnit->SkeletalMeshComponent->WasRecentlyRendered())
		{
			const FVector UnitLocation = CurrentUnit->GetActorLocation() + CurrentUnit->HealthBarAnchor;
	
			PlayerController->ProjectWorldLocationToScreen(UnitLocation, ScreenLocation, true);

			//Prevent offsetting caused by the UMG system
			ScreenLocation = ScreenLocation/ViewPortScale;

			Cast<UCanvasPanelSlot>(HealthBarAssoc.Value->Slot)->SetPosition(ScreenLocation);

			HealthBarAssoc.Value->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			HealthBarAssoc.Value->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ULevelUI::AddHealthBar(UOverlay* HealthBar, UCanvasPanelSlot* ReferencePanelSlot, UCanvasPanel* MainCanvas)
{
	UCanvasPanelSlot* HealthBarPanelSlot = Cast<UCanvasPanelSlot>(MainCanvas->AddChild(HealthBar));

	HealthBarPanelSlot->SetAnchors(ReferencePanelSlot->GetAnchors());
	HealthBarPanelSlot->SetSize(ReferencePanelSlot->GetSize());
	HealthBarPanelSlot->SetAlignment(ReferencePanelSlot->GetAlignment());
	HealthBarPanelSlot->SetAutoSize(ReferencePanelSlot->GetAutoSize());
	HealthBarPanelSlot->SetZOrder(ReferencePanelSlot->GetZOrder());
}

void ULevelUI::FocusHealthBar(int IdUnit)
{
	for (const TPair<int, UOverlay*>& HealthBarAssoc : HealthBarAssociationMap)
	{
		if (HealthBarAssoc.Key == IdUnit)
		{
			HealthBarAssoc.Value->SetRenderOpacity(1.0f);
		}
		else
		{
			HealthBarAssoc.Value->SetRenderOpacity(UnfocusAlpha);
		}
	}
}



