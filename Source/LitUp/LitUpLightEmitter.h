// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LitUpLightRay.h"
#include "LitUpLightEmitter.generated.h"

UCLASS()
class LITUP_API ALitUpLightEmitter : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* Emitter;

	UPROPERTY(EditAnywhere)
		UStaticMesh* EmitterMeshAsset; //Mesh de l'émetteur

	UPROPERTY(VisibleDefaultsOnly)
		ALitUpLightRay* LightRay = nullptr; //Pointeur du rayon généré par l'émetteur

	UPROPERTY(EditAnywhere)
		int MaxRays = 1000; //Nombre maximal de rayons générés par cet émetteur

	UPROPERTY(EditAnywhere)
		float WaveLength = 780.f; //Longeur d'onde émise par l'émetteur

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;

public:	
	// Sets default values for this actor's properties
	ALitUpLightEmitter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};