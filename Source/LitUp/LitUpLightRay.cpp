// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpLightRay.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include <Materials/MaterialInstanceConstant.h>

//Ajouter les include des Actors ici
#include "LitUpLightTarget.h"
#include "LitUpMirror.h"
#include "LitUpPrism.h"
#include "LitUpDiffractor.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLightRay, Log, All);
DEFINE_LOG_CATEGORY(LogLightRay);

// Sets default values
ALitUpLightRay::ALitUpLightRay()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Créer un composant situé au début du rayon
	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
	SetRootComponent(Origin);

	//Créer et attacher le mesh du rayon à l'origine
	LightRay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightRay"));
	LightRay->SetupAttachment(Origin);

	//Définir le mesh utilisé par le rayon
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CylinderMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	LightRay->SetStaticMesh(CylinderMeshAsset.Object);

	//Définir le matériau utilisé par le rayon
	static ConstructorHelpers::FObjectFinder<UMaterial>laserMaterial(TEXT("Material '/Game/Materials/M_Laser.M_Laser'"));
	dynamicLaserMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(laserMaterial.Object, LightRay);
	LightRay->SetMaterial(0, dynamicLaserMaterialInstanceDynamic);

	//Définir des paramètres du rayon
	LightRay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightRay->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	LightRay->CastShadow = false;
	LightRay->bEmissiveLightSource = true;
	LightRay->bReceivesDecals = false;
}

// Called when the game starts or when spawned
void ALitUpLightRay::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALitUpLightRay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Faire un raycast à partir de l'origine de longeur length
	FVector Start = Origin->GetComponentLocation();
	FVector ForwardVector = Origin->GetForwardVector();
	FHitResult OutHit;
	FVector End = ((ForwardVector * length) + Start);

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true; //On veut faire des raycast qui obtiennent des résultats même s'ils commencent à l'intérieur d'un objet

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.04, 0, 10);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		//Définir la position du cylindre comme étant le millieu du vecteur partant de l'origine jusqu'au point touché et changer sa taille afin qu'il parte de l'origine jusqu'au point touché
		LightRay->SetWorldTransform(FTransform(FRotator(90, 0, 0) + ForwardVector.Rotation(), (OutHit.Location - Start) / 2.f + Start, FVector(0.05, 0.05, OutHit.Distance / 100.f)));

		if (OutHit.GetActor()->IsA(ALitUpLightTarget::StaticClass())) // CIBLE
		{
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::`Green, TEXT("Target is getting hit!"));
			//UE_LOG(LogLightRay, Display, TEXT("Target is getting hit!"));

			goNext(false); //Aucun nouveau rayon généré si on touche cet objet

			Cast<ALitUpLightTarget>(OutHit.GetActor())->exec(wavelength);
		}
		else if (OutHit.GetActor()->IsA(ALitUpMirror::StaticClass())) // RÉFLEXION
		{
			goNext(true); //Généré un nouveau rayon si on touche cet objet

			if (nextLightRay) Reflection(ForwardVector, OutHit.Normal, OutHit.Location);
		}
		else if (OutHit.GetActor()->IsA(ALitUpPrism::StaticClass())) // RÉFRACTION
		{
			goNext(true); //Généré un nouveau rayon si on touche cet objet

			if (nextLightRay) Refraction(ForwardVector, OutHit.Normal, OutHit.Location, currentRefractionIndex, Cast<ALitUpPrism>(OutHit.GetActor())->getRefractionIndex());
		}
		else if (OutHit.GetActor()->IsA(ALitUpDiffractor::StaticClass())) // DIFFRACTION
		{
			goNext(false); //Aucun nouveau rayon généré si on touche cet objet

			Cast<ALitUpDiffractor>(OutHit.GetActor())->exec(wavelength);
		}
		else
		{
			goNext(false); // Ne pas générer des rayons à l'infini

			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("No hit!"));
			//UE_LOG(LogLightRay, Display, TEXT("No hit!"));
		}
	}
	else //Si le raycast échoue, on définit des valeurs par défaut
	{
		LightRay->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	}

	if (!maxRays) //Si l'émetteur a changé son nombre maximal de rayons, enlever ceux en trop
	{
		goNext(false);
	}

	if (nextLightRay) //Définir les paramètres pour que lorsqu'ils sont changés dans l'éditeur, le changement se fait en temps réel
	{
		nextLightRay->wavelength = wavelength;
		nextLightRay->maxRays = maxRays - 1;
	}

	//Définir la couleur du laser dépendant de la longueur d'onde
	dynamicLaserMaterialInstanceDynamic->SetVectorParameterValue(FName("LaserColor"), calculateColorFromWaveLength());
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpLightRay::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline void ALitUpLightRay::goNext(bool goNext)
{
	if (next == true && goNext == true) //Un rayon a déja été créé et on en a besoin
	{
		return;
	}
	else if (next == false && goNext == true && maxRays > 0) //Il n'y a pas de rayon et on en a besoin d'un et on n'est pas à la limite maximale de rayons
	{
		next = true;

		FActorSpawnParameters sp;
		sp.ObjectFlags = RF_Transient; //Le rayon ne se fait pas sauvegarder
		nextLightRay = GetWorld()->SpawnActor<ALitUpLightRay>(ALitUpLightRay::StaticClass(), sp);
		nextLightRay->maxRays = maxRays - 1; //Définir le nombre maximal de rayons générés par le prochain rayon
		nextLightRay->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform); //Attacher le nouveau rayon

		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("New ray created!"));
		//UE_LOG(LogLightRay, Display, TEXT("New ray created!"));
	}
	else //Pour tous les autres cas, enlever le prochain rayon s'il existe
	{
		next = false;

		if (nextLightRay)
		{
			nextLightRay->goNext(false);

			nextLightRay->K2_DestroyActor();
			nextLightRay = nullptr;
			if (GEngine) GEngine->ForceGarbageCollection(true);

			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Ray deleted!"));
			//UE_LOG(LogLightRay, Display, TEXT("Ray deleted!"));
		}
	}

	return;
}

inline void ALitUpLightRay::Reflection(const FVector& Direction, const FVector& SurfaceNormal, const FVector& Location)
{
	
	// Tolérance limite afin d'éviter un calcul trop pointu
	float tolerance = 0.01f;

	// get taille du vecteur
	float SquareSum = SurfaceNormal.X * SurfaceNormal.X + SurfaceNormal.Y * SurfaceNormal.Y + SurfaceNormal.Z * SurfaceNormal.Z;

	FVector NormalizedNormal;

	// Normalisation du vecteur de la normale

	if (SquareSum > 1.f + tolerance || SquareSum < 1.f - tolerance)
	{
		NormalizedNormal = SurfaceNormal * FMath::InvSqrt(SquareSum);
	}
	else
	{
		NormalizedNormal = SurfaceNormal;
	}

	// Calcul du vecteur résultant
	FVector reflection = Direction - 2 * (Direction | NormalizedNormal) * NormalizedNormal;

	// Application du vecteur résultant sur le prochain rayon 
	nextLightRay->SetActorTransform(FTransform(reflection.Rotation(), Location + (reflection * 0.0001)));
}

inline void ALitUpLightRay::Refraction(const FVector& Direction, const FVector& SurfaceNormal, const FVector& Location, const float& CurrentRefractionIndex, const float& ObjectRefractionIndex)
{
	// Param Direction: Vecteur incident
	// Param SurfaceNormal: Vecteur de la normale de la surface frappée
	// Param Location: Point d'impact
	double n1;
	double n2;

	if (CurrentRefractionIndex < ObjectRefractionIndex) // On entre dans un objet
	{
		n1 = CurrentRefractionIndex;
		n2 = ObjectRefractionIndex;
	}
	else // On sort d'un objet
	{
		n1 = ObjectRefractionIndex;
		n2 = 1;
	}

	double n = n1 / n2;
	double cosIncident = FVector::DotProduct(SurfaceNormal, -Direction);
	double sinRefractedSquared = n * n * (1.0 - (cosIncident * cosIncident));
	if (sinRefractedSquared > 1.0)
	{
		// RTI applicable ici
		Reflection(Direction, SurfaceNormal, Location);
	}
	else
	{
		double cosRefracted = FMath::Sqrt(1.0 - sinRefractedSquared);
		FVector refraction = n * Direction + (n * cosIncident - cosRefracted) * SurfaceNormal; // Informations sur le vecteur réfracté

		nextLightRay->SetActorTransform(FTransform(refraction.Rotation(), Location + (refraction * 0.0001))); // Prochain rayon
		nextLightRay->currentRefractionIndex = n2;
	}
}

inline FVector ALitUpLightRay::calculateColorFromWaveLength()
{
	// Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;

	//Fonction par partie approximative des contributions r,g,b pour la couleur selon la longeur d'onde
	if ((380.0 <= wavelength) && (wavelength <= 439.0))
	{
		red = -(wavelength - 440.0) / (440.0 - 380.0);
		green = 0.0;
		blue = 1.0;
	}
	else if ((440.0 <= wavelength) && (wavelength <= 489.0))
	{
		red = 0.0;
		green = (wavelength - 440.0) / (490.0 - 440.0);
		blue = 1.0;
	}
	else if ((490.0 <= wavelength) && (wavelength <= 509.0))
	{
		red = 0.0;
		green = 1.0;
		blue = -(wavelength - 510.0) / (510.0 - 490.0);
	}
	else if ((510.0 <= wavelength) && (wavelength <= 579.0))
	{
		red = (wavelength - 510.0) / (580.0 - 510.0);
		green = 1.0;
		blue = 0.0;
	}
	else if ((580.0 <= wavelength) && (wavelength <= 644.0))
	{
		red = 1.0;
		green = -(wavelength - 645.0) / (645.0 - 580.0);
		blue = 0.0;
	}
	else if ((645.0 <= wavelength) && (wavelength <= 780.0))
	{
		red = 1.0;
		green = 0.0;
		blue = 0.0;
	}

	//Simuler les longeurs d'onde visibles, en diminuant l'intensité aux valeur extrêmes
	double factor = 0.0;

	if ((380.0 <= wavelength) && (wavelength <= 419.0))
		factor = 0.3 + 0.7 * (wavelength - 380.0) / (420.0 - 380.0);
	else if ((420.0 <= wavelength) && (wavelength <= 700.0))
		factor = 1.0;
	else if ((701.0 <= wavelength) && (wavelength <= 780.0))
		factor = 0.3 + 0.7 * (780.0 - wavelength) / (780.0 - 700.0);
	else
		factor = 0.0;

	//Ajuster le gamma
	const double gamma = 0.8;
	const double intensity_max = 100.0;

	FVector result = { intensity_max * pow(red * factor, gamma), intensity_max * pow(green * factor, gamma), intensity_max * pow(blue * factor, gamma) };

	return result;
}