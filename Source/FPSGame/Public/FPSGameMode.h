// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

class AFPSPlayerController;
class APlayerStart;

UCLASS()
class AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(EditDefaultsOnly, Category="Spectating")
	TSubclassOf<AActor> SpectatingViewPointClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawn")
	TArray<APlayerStart*> SpawnPoints;

	FTimerHandle RespawnTimerHandle;

public:

	AFPSGameMode();

protected:
	APlayerStart* GetSpawnPoints();

public:
	UFUNCTION()
	void Spawn(AController* Controller);
	
	void Respawn(AController* Controller);

	void CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess);

	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);
};



