// Fill out your copyright notice in the Description page of Project Settings.

#include "TBTacticalMainController.h"
#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


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
	SpringArmComponent->TargetArmLength = 500.0f; // Adjust this as per your needs
	SpringArmComponent->bEnableCameraLag = true; // Enable camera lag for a spring-like effect
	SpringArmComponent->CameraLagSpeed = 10.0f; // Adjust camera lag speed as needed
	
}

// Called when the game starts or when spawned
void ATBTacticalMainController::BeginPlay()
{
	Super::BeginPlay();
	World = GetWorld();
	if (World)
	{
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
	}
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	CameraRotation = GetActorRotation();
}

// Called every frame
void ATBTacticalMainController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCameraControlLock)
	{
		return;
	}
	
	MouseScroll();
	MoveCamera(DeltaTime);
}

void ATBTacticalMainController::PressUp()
{
	bMoveUp = true;
	bPressUp = true;
}

void ATBTacticalMainController::ReleaseUp()
{
	bMoveUp = false;
	bPressUp = false;
}

void ATBTacticalMainController::PressDown()
{
	bMoveDown = true;
	bPressDown = true;
}

void ATBTacticalMainController::ReleaseDown()
{
	bMoveDown = false;
	bPressDown = false;
}

void ATBTacticalMainController::PressRight()
{
	bMoveRight = true;
	bPressRight = true;
}

void ATBTacticalMainController::ReleaseRight()
{
	bMoveRight = false;
	bPressRight = false;
}

void ATBTacticalMainController::PressLeft()
{
	bMoveLeft = true;
	bPressLeft = true;
}

void ATBTacticalMainController::ReleaseLeft()
{
	bMoveLeft = false;
	bPressLeft = false;
}

void ATBTacticalMainController::PressTurnCameraRight()
{
	CameraRotation -= FRotator(0.0f, 90.0f, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalMainController::RotateCameraTimerFunction,
	  RotateTimerClock, 
	  true);
}

void ATBTacticalMainController::PressTurnCameraLeft()
{
	CameraRotation += FRotator(0.0f, 90.0f, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalMainController::RotateCameraTimerFunction,
	  RotateTimerClock, 
	  true);
}

void ATBTacticalMainController::RotateCameraTimerFunction()
{
	const FRotator CurrentRotation = FMath::Lerp(GetActorRotation(), CameraRotation, RotateCameraSpeed * RotateTimerClock);
	SetActorRotation(CurrentRotation);
	if (CurrentRotation.Equals(CameraRotation, 0.1f))
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimerHandle);
	}
}

void ATBTacticalMainController::MouseScroll()
{
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

