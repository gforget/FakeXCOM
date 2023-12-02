// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUI.h"

#include "DebugHeader.h"
#include "TargetManager.h"
#include "TBTacticalGameMode.h"
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
		TBTacticalGameMode->UnitAbilityManager->OnAbilitySelectionModeChangeEvent.AddDynamic(this, &ULevelUI::OnAbilitySelectionModeChangeEvent);
	}
}

void ULevelUI::OnTargetSelected(int TargetIndex)
{
	OnBPTargetSelectedEvent.Broadcast(TargetIndex);
}

void ULevelUI::OnUnitSelected(AUnit* Unit)
{
	OnBPUnitSelectedEvent.Broadcast(Unit);
}

void ULevelUI::OnUnitSpawned(AUnit* Unit)
{
	OnBPUnitSpawnEvent.Broadcast(Unit);
}

void ULevelUI::OnAbilitySelectionModeChangeEvent(bool AbilitySelectionModeValue)
{
	OnBPAbilitySelectionModeChangeEvent.Broadcast(AbilitySelectionModeValue);
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
			ScreenLocation *= GameUserSettings->GetResolutionScaleNormalized();

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



