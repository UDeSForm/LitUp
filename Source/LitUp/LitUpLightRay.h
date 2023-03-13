// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LitUpLightRay.generated.h"

UCLASS(Transient)
class LITUP_API ALitUpLightRay : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Origin;

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* LightRay;

	UPROPERTY(VisibleDefaultsOnly)
		UMaterial* LaserMaterial;

	UPROPERTY(VisibleDefaultsOnly)
		ALitUpLightRay* nextLightRay = nullptr;

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;

public:
	// Sets default values for this actor's properties
	ALitUpLightRay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float length = 10000.f;

private:
	bool next = false;
	int maxRays = 4;

	inline void goNext(bool goNext);
};
