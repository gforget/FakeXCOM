// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.


#include "Controller/TBTacticalController.h"

void ATBTacticalController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	
}
