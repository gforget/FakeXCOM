﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUI.h"

#include "DebugHeader.h"
#include "TBTacticalGameMode.h"
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

	if (TBTacticalGameMode)
	{
		TBTacticalGameMode->OnUnitSelectedEvent.AddDynamic(this, &ULevelUI::OnUnitSelected);
	}
	
	return true;
}

void ULevelUI::OnUnitSelected(AUnit* Unit)
{
	OnBPUnitSelectedEvent.Broadcast(Unit);
}

void ULevelUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const float ViewPortScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	
	for (const TPair<int, UOverlay*>& HealthBarAssoc : HealthBarAssociationMap)
	{
		FVector2d ScreenLocation;
		const AUnit* CurrentUnit = TBTacticalGameMode->AllUnitReference[HealthBarAssoc.Key];
		const FVector UnitLocation =CurrentUnit->GetActorLocation() + CurrentUnit->HealthBarAnchor;
		
		PlayerController->ProjectWorldLocationToScreen(UnitLocation, ScreenLocation, true);

		//Prevent offsetting caused by the UMG system
		ScreenLocation = ScreenLocation/ViewPortScale;
		ScreenLocation *= GameUserSettings->GetResolutionScaleNormalized();
		
		Cast<UCanvasPanelSlot>(HealthBarAssoc.Value->Slot)->SetPosition(ScreenLocation);
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



