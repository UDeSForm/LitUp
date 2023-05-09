// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LitUpLightRay.generated.h"

UCLASS()
class LITUP_API ALitUpLightRay : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Origin;

	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* LightRay;

	UPROPERTY(VisibleDefaultsOnly)
		ALitUpLightRay* nextLightRay = nullptr; //Pointeur du prochain rayon

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor>LitUpTargetBPClass;

public:
	// Sets default values for this actor's properties
	ALitUpLightRay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float length = 10000.f; //Longeur du raycast
	int maxRays = 1000; //Nombre maximal de rayons
	float currentRefractionIndex = 1.f; //Indice de réfraction du milieu dans lequel le rayon est
	float wavelength = 780.f; //Longeur d'onde de la lumière du rayon

private:
	bool next = false; //Variable qui définit si un prochain rayon existe ou pas
	UMaterialInstanceDynamic* dynamicLaserMaterialInstanceDynamic; //Matériau à modifier pour changer la couleur

	inline void goNext(bool goNext);
	inline void Reflection(const FVector& Direction, const FVector& SurfaceNormal, const FVector& Location);
	inline void Refraction(const FVector& Direction, const FVector& SurfaceNormal, const FVector& Location, const float& CurrentRefractionIndex, const float& ObjectRefractionIndex);
	inline FVector calculateColorFromWaveLength();
};