// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBlackHole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	BlackHole = CreateDefaultSubobject<USphereComponent>(TEXT("BlackHole"));
	BlackHole->SetSphereRadius(100);
	BlackHole->SetupAttachment(MeshComp);

	BlackHole->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::OverlapBlackHole);

	BlackHoleRadius = CreateDefaultSubobject<USphereComponent>(TEXT("BlackHoleRadius"));
	BlackHoleRadius->SetSphereRadius(5000);
	BlackHoleRadius->SetupAttachment(MeshComp);
}

void AFPSBlackHole::OverlapBlackHole(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
		OtherActor->Destroy();
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlappingComps;
	BlackHoleRadius->GetOverlappingComponents(OverlappingComps);

	for (int32 i = 0; i < OverlappingComps.Num(); i++)
	{
		UPrimitiveComponent* comp = OverlappingComps[i];
		if (comp && comp->IsSimulatingPhysics())	// If component exists and is simulating physics.
		{
			const float SphereRadius = BlackHoleRadius->GetScaledSphereRadius();
			const float ForceStrength = -10000;

			comp->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Linear, true);
		}
	}
}

