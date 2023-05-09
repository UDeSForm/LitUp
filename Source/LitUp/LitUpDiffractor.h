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
		//Longueur d'onde en nm
		double WaveLength = 780;

	UPROPERTY(EditAnywhere)
		//Largeur en nm
		double largeurFente = 100;

	UPROPERTY(EditAnywhere)
		//Hauteur en nm
		double hauteurFente = 100;

	UPROPERTY(EditAnywhere)
		//taille en pixels du patron de diffraction
		int size = 8192;

	UPROPERTY(VisibleDefaultsOnly)
		UTexture2D* patronDiffraction;

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
	//Fonction appelée une seule fois qui crée le patron
	void CalculerPatronDiffraction();
	//Fonction qui est appelée lorsque le diffracteur est atteint par un laser
	void exec(float rayWaveLength);

private:
	bool showDecal = false;
	TArray<FColor> pixelsPatron;
	inline FVector calculateColorFromWaveLength();
	ADecalActor* decal;
	UMaterial* diffractionMaterial;
};