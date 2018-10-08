// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
	
	GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
		return;

	AFPSGameMode *GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	//DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12, FColor::Red, false, 10.f);

	SetGuardState(EAIState::Alerted);
}

void AFPSAIGuard::OnNoiseHeard(APawn* HeardPawn, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	//DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, false, 10.f);

	// Get new direction
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);	// So it doesn't reset to the previous state, but original.
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.f);

	SetGuardState(EAIState::Suspicious);
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;
	OnRep_GuardState();
}

void AFPSAIGuard::MoveToNextPoint()
{
	if (CurrentPatrolPoint == nullptr)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}

	FVector MoveDirection;
	FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();	// Vector between Actor and Patrol point.
	if (Delta.Size() < 80)
	{
		// If the guard should change direction, get the new one and set guard to face it.
		if (CurrentPatrolPoint == SecondPatrolPoint)
		{
			CurrentPatrolPoint = FirstPatrolPoint;
		}
		else
		{
			CurrentPatrolPoint = SecondPatrolPoint;
		}

		MoveDirection = CurrentPatrolPoint->GetActorLocation() - GetActorLocation();
		FRotator FaceDirection = FRotationMatrix::MakeFromX(MoveDirection).Rotator();
		FaceDirection.Pitch = 0.f;
		FaceDirection.Roll = 0.f;

		SetActorRotation(FaceDirection);
		OriginalRotation = GetActorRotation();
	}
	else
	{
		// If the guard isn't changing direction just get it's current direction.
		MoveDirection = CurrentPatrolPoint->GetActorLocation() - GetActorLocation();
	}

	MoveDirection.Normalize();

	AddMovementInput(MoveDirection);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (CurrentPatrolPoint)
	if(bPatrol && GuardState == EAIState::Idle)
	{
		MoveToNextPoint();
	}
}

// Replicates guardstate to all clients
void AFPSAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState);
}
