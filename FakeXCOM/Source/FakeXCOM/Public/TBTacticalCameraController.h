// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TBTacticalCameraController.generated.h"
class USpringArmComponent;
class UCameraComponent;
class UMouseSceneSelectionComponent;
class UWorld;
class APlayerController;

UCLASS()
class FAKEXCOM_API ATBTacticalCameraController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATBTacticalCameraController();

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
	float Speed = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveUp = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveDown = 0.95f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveRight = 0.95f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Properties")
	float PercentMoveLeft = 0.05f;

private:
	UPROPERTY()
	UWorld* WorldPtr;
	
	UPROPERTY()
	APlayerController* PlayerControllerPtr;

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
	
	void PressUp();
	void ReleaseUp();
	
	void PressDown();
	void ReleaseDown();

	void PressRight();
	void ReleaseRight();
	
	void PressLeft();
	void ReleaseLeft();
	
	void MouseScroll();
	void MoveCamera(float DeltaTime);
};
