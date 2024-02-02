// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#include "Controller/TBTacticalMainController.h"

#include "Camera/CameraComponent.h"
#include "Controller/MouseSceneSelectionComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Managers/TargetManager.h"
#include "Managers/UnitAbilityManager.h"
#include "Managers/UnitManager.h"
#include "Pathfinder/TileMovementComponent.h"
#include "Utility/DebugHeader.h"

// Sets default values
ATBTacticalMainController::ATBTacticalMainController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MouseSceneSelectionComponent = CreateDefaultSubobject<UMouseSceneSelectionComponent>(TEXT("Mouse Scene Selection Component"));
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetMobility(EComponentMobility::Movable);
	
	// Create a RootComponent as a SceneComponent
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	RootComponent->SetMobility(EComponentMobility::Movable);
	
	// Create and attach CameraComponent to SpringArmComponent
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
	SpringArmComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CameraComponent->AttachToComponent(SpringArmComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Set the properties of the SpringArmComponent as needed
	SpringArmComponent->bEnableCameraLag = true; // Enable camera lag for a spring-like effect
	SpringArmComponent->CameraLagSpeed = 10.0f; // Adjust camera lag speed as needed
}

// Called when the game starts or when spawned
void ATBTacticalMainController::BeginPlay()
{
	Super::BeginPlay();
	SpringArmComponent->TargetArmLength = HeightCameraValue[CurrentHeightCameraValueIndex];
	
	World = GetWorld();
	CameraRotation = GetActorRotation();
	
	if (World)
	{
		TBTacticalGameMode = World->GetAuthGameMode<ATBTacticalGameMode>();
		TBTacticalGameMode->MainController = this;
		PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			// Set the new view target with a blend time
			PlayerController->SetViewTargetWithBlend(this, 1.0f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
		}
	}

	if (InputComponent)
	{
		InputComponent->BindAction("Up", IE_Pressed, this, &ATBTacticalMainController::PressUp);
		InputComponent->BindAction("Up", IE_Released, this, &ATBTacticalMainController::ReleaseUp);

		InputComponent->BindAction("Down", IE_Pressed, this, &ATBTacticalMainController::PressDown);
		InputComponent->BindAction("Down", IE_Released, this, &ATBTacticalMainController::ReleaseDown);
		
		InputComponent->BindAction("Right", IE_Pressed, this, &ATBTacticalMainController::PressRight);
		InputComponent->BindAction("Right", IE_Released, this, &ATBTacticalMainController::ReleaseRight);
		
		InputComponent->BindAction("Left", IE_Pressed, this, &ATBTacticalMainController::PressLeft);
		InputComponent->BindAction("Left", IE_Released, this, &ATBTacticalMainController::ReleaseLeft);

		InputComponent->BindAction("TurnCameraRight", IE_Pressed, this, &ATBTacticalMainController::PressTurnCameraRight);
		InputComponent->BindAction("TurnCameraLeft", IE_Pressed, this, &ATBTacticalMainController::PressTurnCameraLeft);

		InputComponent->BindAction("SelectPreviousSoldier", IE_Pressed, this, &ATBTacticalMainController::PressSelectPreviousSoldier);
		InputComponent->BindAction("SelectNextSoldier", IE_Pressed, this, &ATBTacticalMainController::PressSelectNextSoldier);

		InputComponent->BindAction("CameraUp", IE_Pressed, this, &ATBTacticalMainController::PressCameraUp);
		InputComponent->BindAction("CameraDown", IE_Pressed, this, &ATBTacticalMainController::PressCameraDown);

		InputComponent->BindAction("RightClick", IE_Pressed, this, &ATBTacticalMainController::PressRightClick);

		InputComponent->BindAction("Escape", IE_Pressed, this, &ATBTacticalMainController::PressEscape);
	}
	
	TBTacticalGameMode->TurnManagerComponent->OnTurnStartedEvent.AddDynamic(this, &ATBTacticalMainController::OnTurnStartedEvent);
}

// Called every frame
void ATBTacticalMainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PerformFollowActor(DeltaTime);
	MouseScroll();
	MoveCamera(DeltaTime);
}

void ATBTacticalMainController::FollowActor(AActor* TargetActor)
{
	CameraTargetActor = TargetActor;
}

void ATBTacticalMainController::UnFollowActor()
{
	CameraTargetActor = nullptr;
}

bool ATBTacticalMainController::IsCameraControlLock()
{
	return CameraTargetActor != nullptr || bForceCameraLock || bAICameraLock;
}

void ATBTacticalMainController::PerformFollowActor(float DeltaTime)
{
	if (CameraTargetActor)
	{
		const FVector CurrentLocation =
			FMath::Lerp(GetActorLocation(),
				CameraTargetActor->GetActorLocation(),
				FollowCameraSpeed * DeltaTime);
		
		SetActorLocation(CurrentLocation);
	}
}

void ATBTacticalMainController::SubscribeOnUnitMovingEvents(UTileMovementComponent* UnitMovementComponent)
{
	UnitMovementComponent->OnUnitStartMovingEvent.AddDynamic(this, &ATBTacticalMainController::OnUnitStartMovingEvent);
	UnitMovementComponent->OnUnitStopMovingEvent.AddDynamic(this, &ATBTacticalMainController::OnUnitStopMovingEvent);
}

void ATBTacticalMainController::OnUnitStartMovingEvent(AActor* MovingActor)
{
	FollowActor(MovingActor);
}

void ATBTacticalMainController::OnUnitStopMovingEvent(AActor* MovingActor)
{
	UnFollowActor();
}

void ATBTacticalMainController::OnTurnStartedEvent(EFaction CurrentSelectedFaction)
{
	if (TBTacticalGameMode->FactionManagerComponent->FactionsController[CurrentSelectedFaction] == EAssignedController::AIController)
	{
		bAICameraLock = true;
	}
	else
	{
		bAICameraLock = false;
	}
}

void ATBTacticalMainController::GoToActor(AActor* TargetActor)
{
	FollowActor(TargetActor);
	
	GetWorld()->GetTimerManager().SetTimer(
  GoToActorTimerHandle,
  this,
  &ATBTacticalMainController::GoToActorTimerFunction,
  TimerClock, 
  true);
}

void ATBTacticalMainController::GoToActorTimerFunction()
{
	if (!CameraTargetActor)
	{
		DebugScreen("Camera Target Actor not set", FColor::Red);
	}
	
	if (CameraTargetActor && GetActorLocation().Equals(CameraTargetActor->GetActorLocation(), 5.0f))
	{
		UnFollowActor();
		GetWorld()->GetTimerManager().ClearTimer(GoToActorTimerHandle);
	}
}

void ATBTacticalMainController::PressUp()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveUp = true;
	bPressUp = true;
}

void ATBTacticalMainController::ReleaseUp()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveUp = false;
	bPressUp = false;
}

void ATBTacticalMainController::PressDown()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveDown = true;
	bPressDown = true;
}

void ATBTacticalMainController::ReleaseDown()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveDown = false;
	bPressDown = false;
}

void ATBTacticalMainController::PressRight()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveRight = true;
	bPressRight = true;
}

void ATBTacticalMainController::ReleaseRight()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveRight = false;
	bPressRight = false;
}

void ATBTacticalMainController::PressLeft()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveLeft = true;
	bPressLeft = true;
}

void ATBTacticalMainController::ReleaseLeft()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	bMoveLeft = false;
	bPressLeft = false;
}

void ATBTacticalMainController::PressTurnCameraRight()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	CameraRotation -= FRotator(0.0f, 90.0f, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalMainController::RotateCameraTimerFunction,
	  TimerClock, 
	  true);
}

void ATBTacticalMainController::PressTurnCameraLeft()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	CameraRotation += FRotator(0.0f, 90.0f, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalMainController::RotateCameraTimerFunction,
	  TimerClock, 
	  true);
}

void ATBTacticalMainController::RotateCameraToSpecificYaw(float yaw)
{
	CameraRotation = FRotator(CameraRotation.Pitch, yaw, CameraRotation.Roll);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalMainController::RotateCameraTimerFunction,
	  TimerClock, 
	  true);
}

void ATBTacticalMainController::RotateCameraTimerFunction()
{
	const FRotator CurrentRotation = FMath::Lerp(GetActorRotation(), CameraRotation, RotateCameraSpeed * TimerClock);
	SetActorRotation(CurrentRotation);
	
	if (CurrentRotation.Equals(CameraRotation, 0.1f))
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimerHandle);
	}
}

void ATBTacticalMainController::PressSelectPreviousSoldier()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	if (TBTacticalGameMode->UnitAbilityManager->GetAbilitySelectionMode())
	{
		TBTacticalGameMode->TargetManager->SelectPreviousTarget();
	}
	else
	{
		TBTacticalGameMode->UnitManager->SelectPreviousUnit();
	}
}

void ATBTacticalMainController::PressSelectNextSoldier()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	if (TBTacticalGameMode->UnitAbilityManager->GetAbilitySelectionMode())
	{
		TBTacticalGameMode->TargetManager->SelectNextTarget();
	}
	else
	{
		TBTacticalGameMode->UnitManager->SelectNextUnit();
	}
}

void ATBTacticalMainController::ForceCameraLock(bool value)
{
	bForceCameraLock = value;
}

void ATBTacticalMainController::PressCameraUp()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	if (CurrentHeightCameraValueIndex - 1 >= 0)
	{
		CurrentHeightCameraValueIndex--;
		MoveCameraHeight(CurrentHeightCameraValueIndex);
	}
}

void ATBTacticalMainController::PressCameraDown()
{
	if (bForceCameraLock || bAICameraLock || bControlDisable) return;
	if (CurrentHeightCameraValueIndex + 1 < HeightCameraValue.Num())
	{
		CurrentHeightCameraValueIndex++;
		MoveCameraHeight(CurrentHeightCameraValueIndex);
	}
}

void ATBTacticalMainController::CameraHeightTimerFunction()
{
	const float CurrentHeight = FMath::Lerp(SpringArmComponent->TargetArmLength, HeightCameraValue[CurrentHeightCameraValueIndex], CameraHeightSpeed * TimerClock);
	SpringArmComponent->TargetArmLength = CurrentHeight;
	
	if (SpringArmComponent->TargetArmLength >= HeightCameraValue[CurrentHeightCameraValueIndex]-0.1f &&
		SpringArmComponent->TargetArmLength <= HeightCameraValue[CurrentHeightCameraValueIndex]+0.1f)
	{
		GetWorld()->GetTimerManager().ClearTimer(CameraHeightTimerHandle);
	}
}

 void ATBTacticalMainController::MoveCameraHeight(int CameraHeightIndex)
{
	if(HeightCameraValue.Num() == 0)
	{
		DebugScreen("HeightCameraValues not set", FColor::Red);
		return;	
	}
	
	CurrentHeightCameraValueIndex = CameraHeightIndex;
	GetWorld()->GetTimerManager().SetTimer(
	  CameraHeightTimerHandle,
	  this,
	  &ATBTacticalMainController::CameraHeightTimerFunction,
	  TimerClock, 
	  true);
}

void ATBTacticalMainController::DisableController(bool Val)
{
	SetActorTickEnabled(!Val);
	bControlDisable = Val;
}

void ATBTacticalMainController::PressRightClick()
{
	if (bAICameraLock || bControlDisable) return;
	if (TBTacticalGameMode->UnitAbilityManager->GetAbilitySelectionMode())
	{
		TBTacticalGameMode->UnitAbilityManager->DeactivateAbilitySelectionMode();
	}
}

void ATBTacticalMainController::PressEscape()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), 0, EQuitPreference::Quit, false);
}

void ATBTacticalMainController::MouseScroll()
{
	if (IsCameraControlLock() || bControlDisable) return;
	if (bPressUp || bPressDown || bPressRight || bPressLeft)
	{
		return;
	}
	
	float mouseXValue = 0.0f;
	float mouseYValue = 0.0f;
	
	int32 vSizeX = 0;
	int32 vSizeY = 0;
	
	PlayerController->GetMousePosition(mouseXValue, mouseYValue);
	PlayerController->GetViewportSize(vSizeX, vSizeY);
	
	// prevent scrolling when mouse out of focus of games
	if (FMath::IsNearlyZero(mouseXValue) && FMath::IsNearlyZero(mouseYValue))
	{
		bMoveUp = false;
		bMoveDown = false;
		bMoveLeft = false;
		bMoveRight = false;
		return;
	}
	
	if (mouseYValue/vSizeY <= PercentMoveUp)
	{
		bMoveUp = true;
	}
	else
	{
		bMoveUp = false;
	}
	
	if (mouseYValue/vSizeY >= PercentMoveDown)
	{
		bMoveDown = true;
	}
	else
	{
		bMoveDown = false;
	}
	
	if (mouseXValue/vSizeX <= PercentMoveLeft)
	{
		bMoveLeft = true;
	}
	else
	{
		bMoveLeft = false;
	}

	if (mouseXValue/vSizeX >= PercentMoveRight)
	{
		bMoveRight = true;
	}
	else
	{
		bMoveRight = false;
	}
}

void ATBTacticalMainController::MoveCamera(float DeltaTime)
{
	if (IsCameraControlLock() || bControlDisable) return;
	FVector TranslationVector = FVector::Zero();
	if(bMoveUp)
	{
		TranslationVector += GetActorForwardVector();
	}
	
	if(bMoveDown)
	{
		TranslationVector -= GetActorForwardVector();
	}
	
	if(bMoveRight)
	{
		TranslationVector += GetActorRightVector();
	}
	
	if(bMoveLeft)
	{
		TranslationVector -= GetActorRightVector();
	}

	TranslationVector = TranslationVector*ScrollingSpeed*DeltaTime;
	if (
		((RootComponent->GetComponentLocation() + TranslationVector).X < TBTacticalGameMode->TopPosition.X) &&
		((RootComponent->GetComponentLocation() + TranslationVector).X > TBTacticalGameMode->BottomPosition.X) &&
		((RootComponent->GetComponentLocation() + TranslationVector).Y > TBTacticalGameMode->BottomPosition.Y) &&
		((RootComponent->GetComponentLocation() + TranslationVector).Y < TBTacticalGameMode->TopPosition.Y)
		)
	{
		RootComponent->AddRelativeLocation(TranslationVector);
	}
}

