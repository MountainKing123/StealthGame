// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"

#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicios,
	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	FRotator OriginalRotation;

	FTimerHandle timerHandle_ResetOrientation;

	EAIState GuardState;
	
public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnSeen(APawn* seenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* heardPawn, const FVector& Location, float Volume);

	void SetGuardState(EAIState newState);

	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnStateChanged(EAIState newState);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ResetOrientation();
};
