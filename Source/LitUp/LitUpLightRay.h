// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LitUpLightRay.generated.h"

UCLASS()
class LITUP_API ALitUpLightRay : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* Cylinder;

	UPROPERTY(VisibleDefaultsOnly)
		UMaterial* LaserMaterial;

public:
	ALitUpLightRay();
	~ALitUpLightRay();
	FHitResult getResult(const float& length, const FVector& Start, const FVector& ForwardVector, const AActor *parent, const UWorld *world);

private:
};
