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
	WorldPtr = GetWorld();
	if (WorldPtr)
	{
		PlayerControllerPtr = WorldPtr->GetFirstPlayerController();
		if (PlayerControllerPtr)
		{
			// Set the new view target with a blend time
			PlayerControllerPtr->SetViewTargetWithBlend(this, 1.0f, EViewTargetBlendFunction::VTBlend_Linear, 0.0f, false);
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
	}
	
}

// Called every frame
void ATBTacticalCameraController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	
	PlayerControllerPtr->GetMousePosition(mouseXValue, mouseYValue);
	PlayerControllerPtr->GetViewportSize(vSizeX, vSizeY);
	
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
	if(bMoveUp)
	{
		RootComponent->AddRelativeLocation(FVector(0.0f, Speed, 0.0f) * DeltaTime);
	}
	
	if(bMoveDown)
	{
		RootComponent->AddRelativeLocation(FVector(0.0f, -Speed, 0.0f) * DeltaTime);
	}
	
	if(bMoveRight)
	{
		RootComponent->AddRelativeLocation(FVector(-Speed, 0.0f, 0.0f) * DeltaTime);
	}
	
	if(bMoveLeft)
	{
		RootComponent->AddRelativeLocation(FVector(Speed, 0.0f, 0.0f) * DeltaTime);
	}
}
