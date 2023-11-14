// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TBTacticalGameMode.h"
#include "GameFramework/Actor.h"
#include "TBTacticalMainController.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UMouseSceneSelectionComponent;
class UWorld;
class APlayerController;

UCLASS()
class FAKEXCOM_API ATBTacticalMainController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATBTacticalMainController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMouseSceneSelectionComponent* MouseSceneSelectionComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float ScrollingSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveUp = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveDown = 0.95f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveRight = 0.95f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveLeft = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float RotateCameraSpeed = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float CameraHeightSpeed = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float FollowCameraSpeed = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	TArray<float> HeightCameraValue;
	
	void SubscribeOnUnitMovingEvents(UTileMovementComponent* UnitMovementComponent);
	void FollowActor(AActor* TargetActor);
	void GoToActor(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Level UI Functions")
	void PressTurnCameraRight();
	
	UFUNCTION(BlueprintCallable, Category = "Level UI Functions")
	void PressTurnCameraLeft();

	UFUNCTION(BlueprintCallable, Category = "Level UI Functions")
	void PressSelectPreviousSoldier();

	UFUNCTION(BlueprintCallable, Category = "Level UI Functions")
	void PressSelectNextSoldier();
	
private:
	UPROPERTY()
	float TimerClock = 0.016666f; //60 fps
	
	UPROPERTY()
	UWorld* World;
	
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	ATBTacticalGameMode* TBTacticalGameMode;
	
	UPROPERTY()
	bool bMoveUp = false;

	UPROPERTY()
	bool bMoveDown = false;

	UPROPERTY()
	bool bMoveRight = false;

	UPROPERTY()
	bool bMoveLeft = false;

	UPROPERTY()
	bool bPressUp = false;

	UPROPERTY()
	bool bPressDown = false;

	UPROPERTY()
	bool bPressRight = false;

	UPROPERTY()
	bool bPressLeft = false;
	
	//Camera Rotation Variables
	UPROPERTY()
	FTimerHandle RotateTimerHandle;
	
	void RotateCameraTimerFunction();
	
	UPROPERTY()
	FRotator CameraRotation;
	
	//Follow Actor Variables
	UPROPERTY()
	AActor* CameraTargetActor = nullptr;
	
	bool IsCameraControlLock();
	void UnFollowActor();
	
	void PerformFollowActor(float DeltaTime);

	UFUNCTION()
	void OnUnitStartMovingEvent(AActor* MovingActor);

	UFUNCTION()
	void OnUnitStopMovingEvent(AActor* MovingActor);

	UPROPERTY()
	FTimerHandle GoToActorTimerHandle;

	void GoToActorTimerFunction();

	//Camera Height Variables
	UPROPERTY()
	FTimerHandle CameraHeightTimerHandle;
	
	void CameraHeightTimerFunction();
	
	int CurrentHeightCameraValueIndex = 0;
	
	//Input function
	void PressUp();
	void ReleaseUp();
	
	void PressDown();
	void ReleaseDown();

	void PressRight();
	void ReleaseRight();
	
	void PressLeft();
	void ReleaseLeft();

	void PressCameraUp();
	void PressCameraDown();
	
	//Player Move Camera Function
	void MouseScroll();
	void MoveCamera(float DeltaTime);
	
};

