// Fill out your copyright notice in the Description page of Project Settings.

#include "TBTacticalCameraController.h"
#include "DebugHeader.h"
#include "MouseSceneSelectionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ATBTacticalCameraController::ATBTacticalCameraController()
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
void ATBTacticalCameraController::BeginPlay()
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
		InputComponent->BindAction("Up", IE_Pressed, this, &ATBTacticalCameraController::PressUp);
		InputComponent->BindAction("Up", IE_Released, this, &ATBTacticalCameraController::ReleaseUp);

		InputComponent->BindAction("Down", IE_Pressed, this, &ATBTacticalCameraController::PressDown);
		InputComponent->BindAction("Down", IE_Released, this, &ATBTacticalCameraController::ReleaseDown);
		
		InputComponent->BindAction("Right", IE_Pressed, this, &ATBTacticalCameraController::PressRight);
		InputComponent->BindAction("Right", IE_Released, this, &ATBTacticalCameraController::ReleaseRight);
		
		InputComponent->BindAction("Left", IE_Pressed, this, &ATBTacticalCameraController::PressLeft);
		InputComponent->BindAction("Left", IE_Released, this, &ATBTacticalCameraController::ReleaseLeft);

		InputComponent->BindAction("TurnCameraRight", IE_Pressed, this, &ATBTacticalCameraController::PressTurnCameraRight);
		InputComponent->BindAction("TurnCameraLeft", IE_Pressed, this, &ATBTacticalCameraController::PressTurnCameraLeft);
	}
	
	TBTacticalGameMode = GetWorld()->GetAuthGameMode<ATBTacticalGameMode>();
	CameraRotation = GetActorRotation();
}

// Called every frame
void ATBTacticalCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCameraControlLock)
	{
		return;
	}
	
	MouseScroll();
	MoveCamera(DeltaTime);
}

void ATBTacticalCameraController::PressUp()
{
	bMoveUp = true;
	bPressUp = true;
}

void ATBTacticalCameraController::ReleaseUp()
{
	bMoveUp = false;
	bPressUp = false;
}

void ATBTacticalCameraController::PressDown()
{
	bMoveDown = true;
	bPressDown = true;
}

void ATBTacticalCameraController::ReleaseDown()
{
	bMoveDown = false;
	bPressDown = false;
}

void ATBTacticalCameraController::PressRight()
{
	bMoveRight = true;
	bPressRight = true;
}

void ATBTacticalCameraController::ReleaseRight()
{
	bMoveRight = false;
	bPressRight = false;
}

void ATBTacticalCameraController::PressLeft()
{
	bMoveLeft = true;
	bPressLeft = true;
}

void ATBTacticalCameraController::ReleaseLeft()
{
	bMoveLeft = false;
	bPressLeft = false;
}

void ATBTacticalCameraController::PressTurnCameraRight()
{
	CameraRotation -= FRotator(0.0f, 90.0f, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalCameraController::RotateCameraTimerFunction,
	  RotateTimerClock, 
	  true);
}

void ATBTacticalCameraController::PressTurnCameraLeft()
{
	CameraRotation += FRotator(0.0f, 90.0f, 0.0f);
	GetWorld()->GetTimerManager().SetTimer(
	  RotateTimerHandle,
	  this,
	  &ATBTacticalCameraController::RotateCameraTimerFunction,
	  RotateTimerClock, 
	  true);
}

void ATBTacticalCameraController::RotateCameraTimerFunction()
{
	const FRotator CurrentRotation = FMath::Lerp(GetActorRotation(), CameraRotation, RotateCameraSpeed * RotateTimerClock);
	SetActorRotation(CurrentRotation);
	if (CurrentRotation.Equals(CameraRotation, 0.1f))
	{
		GetWorld()->GetTimerManager().ClearTimer(RotateTimerHandle);
	}
}

void ATBTacticalCameraController::MouseScroll()
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

void ATBTacticalCameraController::MoveCamera(float DeltaTime)
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

	TranslationVector = TranslationVector*Speed*DeltaTime;
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

