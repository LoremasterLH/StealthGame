// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();
	
protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* LaunchPad;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* HitBox;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Plane;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* SteppedOnEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundBase* SteppedOnSound;

	UFUNCTION()
	void OverlapLaunchPad(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
