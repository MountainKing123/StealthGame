// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSBTTargetPointSelection.h"
#include "FPSBotTargetPoint.h"
#include "FPSAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UFPSBTTargetPointSelection::ExecuteTask(UBehaviorTreeComponent &OwnerComp, uint8* NodeMemory)
{
	AFPSAIController* AIController = Cast<AFPSAIController>(OwnerComp.GetAIOwner());

	if(AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComp();
		AFPSBotTargetPoint* CurrentPoint = Cast<AFPSBotTargetPoint>(BlackboardComp->GetValueAsObject("LocationToGo"));
		AFPSBotTargetPoint* NextTargetPoint = nullptr;
		TArray<AActor*> AvailableTargetPoints = AIController->GetAvailableTargetBotPoints();

		int32 RandomIndex;

		do
		{
			RandomIndex = FMath::RandRange(0, AvailableTargetPoints.Num() - 1);
			NextTargetPoint = Cast<AFPSBotTargetPoint>(AvailableTargetPoints[RandomIndex]);
		}
		while(CurrentPoint == NextTargetPoint);

		BlackboardComp->SetValueAsObject("LocationToGo", NextTargetPoint);
		
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}


