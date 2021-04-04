// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
}

APlayerStart* AFPSGameMode::GetSpawnPoints()
{
	for(int32 i = 0; i < SpawnPoints.Num(); i++)
	{
		int32 Slot = FMath::RandRange(0, SpawnPoints.Num() - 1);
		if(SpawnPoints[Slot])
			return SpawnPoints[Slot];
	}
	
	return nullptr;
}

void AFPSGameMode::Spawn(AController* Controller)
{
	if(APlayerStart* SpawnPoint = GetSpawnPoints())
	{
		FVector Location = SpawnPoint->GetActorLocation();
		FRotator Rotation = SpawnPoint->GetActorRotation();

		if(APawn* Pawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation))
		{
			Controller->Possess(Pawn);
		}
		else
		{
			// ToDo.
		}
	}
}

void AFPSGameMode::Respawn(AController* Controller)
{
	if(Controller)
	{
		if(Controller->GetLocalRole() == ROLE_Authority)
		{
			FTimerDelegate RespawnTimerDelegate;
			RespawnTimerDelegate.BindUFunction(this, FName("Spawn"), Controller);
			GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnTimerDelegate, 2.0f, false);
		}
	}
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if(InstigatorPawn)
	{
		if(SpectatingViewPointClass)
		{
			TArray<AActor*> spectatingActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewPointClass, spectatingActors);

			// Change vietarget if any valid actor found.
			if(spectatingActors.Num() > 0)
			{
				AActor* newViewTarget = spectatingActors[0];

				for(FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
				{
					APlayerController* pC = it->Get();
					if(pC)
					{
						pC->SetViewTargetWithBlend(newViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
					}
				}
			}			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewPointClass is nullptr. Update GameMode class with valid subclass."));
		}
	}

	AFPSGameState* gS = this->GetGameState<AFPSGameState>();
	if(gS)
	{
		gS->MulticastOnMissionComplete(InstigatorPawn, bMissionSuccess);
	}
	
	OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
