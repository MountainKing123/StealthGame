// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

	UPROPERTY(EditAnywhere, Category="Launchpad Physics")
	float LaunchForce = 1000.f;
	
	UPROPERTY(EditAnywhere, Category="Launchpad Physics")
	float MeshForceMultiplier = 300.f;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp;	
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UStaticMeshComponent* OverlayMeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UArrowComponent* ArrowComp;
	
	UPROPERTY(EditDefaultsOnly, Category="Components")
	UBoxComponent* OverlapComp;

	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	USoundBase* ObjectiveMissingSound;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	UParticleSystem* LaunchPadFX;

protected:
// 	// Called when the game starts or when spawned
// 	virtual void BeginPlay() override;
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PlayEffects();
public:	
// 	// Called every frame
// 	virtual void Tick(float DeltaTime) override;

};
