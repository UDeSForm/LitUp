// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "LitUpDiffractor.generated.h"

UCLASS()
class LITUP_API ALitUpDiffractor : public AActor
{
	GENERATED_BODY()
		UPROPERTY(VisibleDefaultsOnly)
		USceneComponent* Origin;

		UPROPERTY(VisibleDefaultsOnly)
		UStaticMeshComponent* Diffractor;

	UPROPERTY(EditAnywhere)
		UStaticMesh* DiffractorMeshAsset;

	UPROPERTY(EditAnywhere)
		UTexture2D* Fente;

	UPROPERTY(EditAnywhere)
		float WaveLength = 780.f;

	UPROPERTY(EditAnywhere)
		double pixelFente = 25;

	UPROPERTY(EditAnywhere)
		int size = 8192;

	/** Allows Tick To happen in the editor viewport*/
	virtual bool ShouldTickIfViewportsOnly() const override;
public:
	// Sets default values for this actor's properties
	ALitUpDiffractor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CalculerPatronDiffraction();
	void exec(float rayWaveLength);
private:
	bool showDecal = false;
	TArray<FColor> pixelsPatron;
	UTexture2D* patronDiffraction;
	inline FVector calculateColorFromWaveLength();
	ADecalActor* decal;
	UMaterial* diffractionMaterial;
};