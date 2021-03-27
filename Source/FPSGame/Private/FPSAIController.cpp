// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIController.h"
#include "FPSAIGuard.h"
#include "FPSBotTargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Kismet/GameplayStatics.h"

AFPSAIController::AFPSAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	LocationToGoKey = "LocationToGo";
}

void AFPSAIController::OnPossess(APawn* Pawn)
{
	Super::OnPossess(Pawn);

	AFPSAIGuard* AIGuard = Cast<AFPSAIGuard>(Pawn);

	if(AIGuard)
	{
		if(AIGuard->BehaviorTree->BlackboardAsset)
			BlackboardComp->InitializeBlackboard(*(AIGuard->BehaviorTree->BlackboardAsset));

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFPSBotTargetPoint::StaticClass(), BotTargetPoints);

		BehaviorComp->StartTree(*AIGuard->BehaviorTree);
	}
}
