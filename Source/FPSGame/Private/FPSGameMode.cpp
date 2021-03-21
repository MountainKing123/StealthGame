// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if(InstigatorPawn)
	{
		InstigatorPawn->DisableInput(nullptr);

		if(SpectatingViewPointClass)
		{
			TArray<AActor*> spectatingActors;
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewPointClass, spectatingActors);

			// Change vietarget if any valid actor found.
			if(spectatingActors.Num() > 0)
			{
				AActor* newViewTarget = spectatingActors[0];
					
				APlayerController* pc = Cast<APlayerController>(InstigatorPawn->GetController());
				if(pc)
				{
					pc->SetViewTargetWithBlend(newViewTarget, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic);
				}
			}			
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("SpectatingViewPointClass is nullptr. Update GameMode class with valid subclass."));
		}
	}

	OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
