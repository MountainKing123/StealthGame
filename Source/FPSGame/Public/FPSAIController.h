// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FPSAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class FPSGAME_API AFPSAIController : public AAIController
{
	GENERATED_BODY()

protected:

public:
	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName LocationToGoKey;

	TArray<AActor*> BotTargetPoints;

protected:
	virtual void OnPossess(APawn* Pawn) override;

public:
	AFPSAIController();

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const
	{
		return BlackboardComp;
	}

	FORCEINLINE TArray<AActor*> GetAvailableTargetBotPoints()
	{
		return BotTargetPoints;
	}
	
};
