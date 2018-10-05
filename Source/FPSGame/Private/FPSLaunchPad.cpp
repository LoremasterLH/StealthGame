// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLaunchPad.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	//SetBoxExtent
	LaunchPad = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaunchPad"));
	LaunchPad->SetRelativeScale3D(FVector(1.f, 1.f, 0.1f));
	LaunchPad->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaunchPad->SetupAttachment(MeshComp);

	Plane = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetRelativeScale3D(FVector(2.5f, 2.5f, 1.f));
	Plane->SetupAttachment(LaunchPad);

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	//HitBox->SetWorldScale3D(FVector(4.f, 4.f, 4.f));
	HitBox->SetRelativeScale3D(FVector(4.f, 4.f, 4.f));
	HitBox->SetRelativeLocation(FVector(0.f, 0.f, 1.5f));
	HitBox->SetupAttachment(MeshComp);

	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OverlapLaunchPad);
}

void AFPSLaunchPad::OverlapLaunchPad(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Overlapped with launch pad!"));

		//this->GetActorRotation()
		FVector forwardVector = OverlappedComp->GetForwardVector();
		forwardVector.Z = 1.f;	// To keep upwards momentum, we only want the direction.
		AFPSCharacter* actor = Cast<AFPSCharacter>(OtherActor);
		if (actor)
		{
			actor->LaunchCharacter(FVector(2000.f, 2000.f, 1500.f) * forwardVector, true, true);
		}
		else if(OtherComp && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulse(FVector(2000.f, 2000.f, 1500.f) * forwardVector, NAME_None, true);
		}

		UGameplayStatics::SpawnEmitterAtLocation(this, SteppedOnEffect, GetActorLocation());
		UGameplayStatics::PlaySound2D(this, SteppedOnSound);
	}
}
