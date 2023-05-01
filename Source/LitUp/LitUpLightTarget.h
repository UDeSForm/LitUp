// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LitUpLightTarget.generated.h"

UCLASS()
class LITUP_API ALitUpLightTarget : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* Target;

	UPROPERTY(EditAnywhere)
		UStaticMesh* TargetMeshAsset;

	UPROPERTY(EditAnywhere)
		bool UseWaveLength = false;

	UPROPERTY(EditAnywhere)
		float WaveLength = 780.f;

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;

public:	
	// Sets default values for this actor's properties
	ALitUpLightTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void exec(float &waveLength);

	UPROPERTY(EditAnywhere, Category = "BlueprintClass")
		AActor* bpActor;

private:
	UMaterialInstanceDynamic* dynamicLaserMaterialInstanceDynamic;

	inline FVector calculateColorFromWaveLength();
};
