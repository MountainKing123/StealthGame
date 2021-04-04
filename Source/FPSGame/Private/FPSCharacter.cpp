// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"

#include "FPSGameMode.h"
#include "FPSProjectile.h"
#include "UnrealNetwork.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->SetRelativeLocation(FVector(0, 0, BaseEyeHeight)); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->SetRelativeRotation(FRotator(2.0f, -15.0f, 5.0f));
	Mesh1PComponent->SetRelativeLocation(FVector(0, 0, -160.0f));

	// Create a gun mesh component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");

	NoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT ("NoiseEmitter"));

	CurrentHealth = MaxHealth;
	
	SetReplicates(true);
    SetReplicateMovement(true);
}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AFPSCharacter::OnHealthUpdate()
{
	if(IsLocallyControlled())
	{
		FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, healthMessage);

		if(CurrentHealth <= 0)
		{
			FString deathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
			Die();
			// SetCurrentHealth(100.0f);
		}	
		
		//Server-specific functionality
		if (GetLocalRole() == ROLE_Authority)
		{
			FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
		}
	}
}

void AFPSCharacter::Die()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		AFPSGameMode* gM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		gM->Respawn(this->GetController());
		
		MultiDie();

		GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AFPSCharacter::BeginDestroy, 3.0f, false);
	}
}

void AFPSCharacter::MultiDie_Implementation()
{
	// Activate ragdoll on death. ToDo: Replace 1P Mesh with 3P Mesh.
	GetCapsuleComponent()->DestroyComponent();
	this->GetCharacterMovement()->DisableMovement();
	this->GetMesh1P()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->GetMesh1P()->SetAllBodiesSimulatePhysics(true);
}

bool AFPSCharacter::MultiDie_Validate()
{
	return true;
}

void AFPSCharacter::SetCurrentHealth(float healthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float AFPSCharacter::TakeDamage(float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator,
								AActor* DamageCauser)
{
	float damageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(damageApplied);
	
	return damageApplied;
}

void AFPSCharacter::OnRep_CurrentHealth()
{
	OnHealthUpdate();
	OnHealthChanged(CurrentHealth);
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!this->IsLocallyControlled())
	{
		FRotator NewRot = CameraComponent->GetRelativeRotation();

		// Using 254.6f prevents over rotation when looking up and down! Ty,b internet!
		NewRot.Pitch = RemoteViewPitch * 360.0f / 254.6f;

		CameraComponent->SetRelativeRotation(NewRot);		
	}
}


void AFPSCharacter::Fire()
{
	this->ServerFire();
	
	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
		}
	}
}

void AFPSCharacter::ServerFire_Implementation()
{
	// try and fire a projectile
	if (ProjectileClass)
	{
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = GunMeshComponent->GetSocketRotation("Muzzle");

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = this;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	}
}

bool AFPSCharacter::ServerFire_Validate()
{
	return true;
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, bIsCarryingObjective);
	DOREPLIFETIME(AFPSCharacter, CurrentHealth);
}