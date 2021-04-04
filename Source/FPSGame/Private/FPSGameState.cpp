// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	for(FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; it++)
	{
		AFPSPlayerController* pC = Cast<AFPSPlayerController>(it->Get());
		if(pC && pC->IsLocalController())
		{
			pC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			APawn* myPawn = pC->GetPawn();
			if(myPawn)
			{
				myPawn->DisableInput(pC);
			}
		}
	}
}
