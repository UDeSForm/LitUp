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
		UStaticMesh* EmitterMeshAsset;

	UPROPERTY(VisibleDefaultsOnly)
		UChildActorComponent* LightRay;

	UPROPERTY(EditAnywhere)
		int MaxRays;

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
