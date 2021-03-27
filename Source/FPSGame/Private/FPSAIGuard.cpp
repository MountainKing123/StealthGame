// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"

#include "DrawDebugHelpers.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSAIController.h"
#include "FPSGameMode.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	this->PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	this->PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	this->GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	this->AIController = Cast<AFPSAIController>(this->GetController());
	this->AIController->BlackboardComp->SetValueAsEnum("GuardState", static_cast<uint8>(this->GuardState));
	
	Super::BeginPlay();

	this->OriginalRotation = this->GetActorRotation();
}

void AFPSAIGuard::OnPawnSeen(APawn* seenPawn)
{
	if(seenPawn == nullptr)
		return;
	
	DrawDebugSphere(GetWorld(), seenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* gm = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if(gm)
	{
		gm->CompleteMission(seenPawn, false);
	}

	SetGuardState(EAIState::Alerted);
	this->AIController->BlackboardComp->SetValueAsEnum("GuardState", static_cast<uint8>(this->GuardState));
	uint8 currentState = this->AIController->BlackboardComp->GetValueAsEnum("GuardState");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("My Location is: %i"), currentState));
}

// This will only be invoked, if Pawn has not been seen already.
void AFPSAIGuard::OnNoiseHeard(APawn* heardPawn, const FVector& Location, float Volume)
{
	if(this->GuardState == EAIState::Alerted)
		return;
	
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	FVector direction = Location - this->GetActorLocation();
	direction.Normalize();
	
	FRotator newLookAt = FRotationMatrix::MakeFromX(direction).Rotator();
	newLookAt.Pitch = 0.0f;
	newLookAt.Roll = 0.0f;
	
	SetActorRotation(newLookAt);

	GetWorldTimerManager().ClearTimer(timerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(timerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	this->SetGuardState(EAIState::Suspicios);
	this->AIController->BlackboardComp->SetValueAsEnum("GuardState", static_cast<uint8>(this->GuardState));
	uint8 currentState = this->AIController->BlackboardComp->GetValueAsEnum("GuardState");
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("My Location is: %i"), currentState));
}

void AFPSAIGuard::SetGuardState(EAIState newState)
{
	if(GuardState == newState)
		return;

	GuardState = newState;
	OnStateChanged(GuardState);
}

void AFPSAIGuard::ResetOrientation()
{
	if(this->GuardState == EAIState::Alerted)
		return;
	
	this->SetActorRotation(this->OriginalRotation);

	this->SetGuardState(EAIState::Idle);
	this->AIController->BlackboardComp->SetValueAsEnum("GuardState", static_cast<uint8>(this->GuardState));
}


// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


