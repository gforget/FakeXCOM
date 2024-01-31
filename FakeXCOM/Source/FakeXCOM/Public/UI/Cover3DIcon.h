// Copyright(c) 2023 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cover3DIcon.generated.h"

UCLASS()
class FAKEXCOM_API ACover3DIcon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACover3DIcon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Icon Properties")
	UStaticMeshComponent* HalfShieldIcon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Icon Properties")
	UStaticMeshComponent* FullShieldIcon;
};
