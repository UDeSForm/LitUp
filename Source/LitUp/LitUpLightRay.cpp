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

	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
	SetRootComponent(Origin);

	LightRay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightRay"));
	LightRay->SetupAttachment(Origin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>CylinderMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder'"));
	LightRay->SetStaticMesh(CylinderMeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial>laserMaterial(TEXT("Material '/Game/Materials/M_Laser.M_Laser'"));
	dynamicLaserMaterialInstanceDynamic = UMaterialInstanceDynamic::Create(laserMaterial.Object, LightRay);
	LightRay->SetMaterial(0, dynamicLaserMaterialInstanceDynamic);

	LightRay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightRay->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	LightRay->CastShadow = false;
	LightRay->bEmissiveLightSource = true;
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

	FVector Start = Origin->GetComponentLocation();
	FVector ForwardVector = Origin->GetForwardVector();
	FHitResult OutHit;
	FVector End = ((ForwardVector * length) + Start);

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.04, 0, 10);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		LightRay->SetWorldTransform(FTransform(FRotator(90, 0, 0) + ForwardVector.Rotation(), (OutHit.Location - Start) / 2.f + Start, FVector(0.05, 0.05, OutHit.Distance / 100.f)));

		if (OutHit.GetActor()->IsA(ALitUpLightTarget::StaticClass()))
		{
			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::`Green, TEXT("Target is getting hit!"));
			//UE_LOG(LogLightRay, Display, TEXT("Target is getting hit!"));

			goNext(false);

			Cast<ALitUpLightTarget>(OutHit.GetActor())->exec(wavelength);
		}
		else if (OutHit.GetActor()->IsA(ALitUpMirror::StaticClass()))
		{
			goNext(true);

			if (nextLightRay) Reflection(ForwardVector, OutHit.Normal, OutHit.Location);
		}
		else if (OutHit.GetActor()->IsA(ALitUpPrism::StaticClass())) // REFRACTION
		{
			goNext(true);

			if (nextLightRay) Refraction(ForwardVector, OutHit.Normal, OutHit.Location, currentRefractionIndex, Cast<ALitUpPrism>(OutHit.GetActor())->getRefractionIndex());
		}
		else if (OutHit.GetActor()->IsA(ALitUpDiffractor::StaticClass()))
		{
			goNext(false);

			Cast<ALitUpDiffractor>(OutHit.GetActor())->exec(wavelength);
		}
		else
		{
			goNext(false);

			//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("No hit!"));
			//UE_LOG(LogLightRay, Display, TEXT("No hit!"));
		}
	}
	else
	{
		LightRay->SetRelativeTransform(FTransform(FRotator(90, 0, 0), FVector(length / 2.f, 0, 0), FVector(0.05, 0.05, length / 100.f)));
	}

	if (!maxRays)
	{
		goNext(false);
	}

	if (nextLightRay)
	{
		nextLightRay->wavelength = wavelength;
		nextLightRay->maxRays = maxRays - 1;
	}

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
	if (next == true && goNext == true)
	{
		return;
	}
	else if (next == false && goNext == true && maxRays > 0)
	{
		next = true;

		FActorSpawnParameters sp;
		sp.ObjectFlags = RF_Transient;
		nextLightRay = GetWorld()->SpawnActor<ALitUpLightRay>(ALitUpLightRay::StaticClass(), sp);
		nextLightRay->maxRays = maxRays - 1;
		nextLightRay->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

		//if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("New ray created!"));
		//UE_LOG(LogLightRay, Display, TEXT("New ray created!"));
	}
	else
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
	float tolerance = 0.01f;

	float SquareSum = SurfaceNormal.X * SurfaceNormal.X + SurfaceNormal.Y * SurfaceNormal.Y + SurfaceNormal.Z * SurfaceNormal.Z;

	FVector NormalizedNormal;

	if (SquareSum > 1.f + tolerance || SquareSum < 1.f - tolerance)
	{
		NormalizedNormal = SurfaceNormal * FMath::InvSqrt(SquareSum);
	}
	else
	{
		NormalizedNormal = SurfaceNormal;
	}

	FVector reflection = Direction - 2 * (Direction | NormalizedNormal) * NormalizedNormal;

	nextLightRay->SetActorTransform(FTransform(reflection.Rotation(), Location + (reflection * 0.0001)));
}

inline void ALitUpLightRay::Refraction(const FVector& Direction, const FVector& SurfaceNormal, const FVector& Location, const float& CurrentRefractionIndex, const float& ObjectRefractionIndex)
{
	double n1;
	double n2;

	if (CurrentRefractionIndex < ObjectRefractionIndex)
	{
		n1 = CurrentRefractionIndex;
		n2 = ObjectRefractionIndex;
	}
	else
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
		//GEngine->AddOnScreenDebugMessage(-14, 1.f, FColor::Yellow, FString::Printf(TEXT("RTI")));
		Reflection(Direction, SurfaceNormal, Location);
	}
	else
	{
		double cosRefracted = FMath::Sqrt(1.0 - sinRefractedSquared);
		//GEngine->AddOnScreenDebugMessage(-14, 1.f, FColor::Yellow, FString::Printf(TEXT("Angle refracte = %f deg"), FMath::RadiansToDegrees(FMath::Acos(cosRefracted)))); // Bon angle
		FVector refraction = n * Direction + (n * cosIncident - cosRefracted) * SurfaceNormal;

		nextLightRay->SetActorTransform(FTransform(refraction.Rotation(), Location + (refraction * 0.0001)));
		nextLightRay->currentRefractionIndex = n2;
	}
}

inline FVector ALitUpLightRay::calculateColorFromWaveLength()
{
	// Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;

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

	double factor = 0.0;

	if ((380.0 <= wavelength) && (wavelength <= 419.0))
		factor = 0.3 + 0.7 * (wavelength - 380.0) / (420.0 - 380.0);
	else if ((420.0 <= wavelength) && (wavelength <= 700.0))
		factor = 1.0;
	else if ((701.0 <= wavelength) && (wavelength <= 780.0))
		factor = 0.3 + 0.7 * (780.0 - wavelength) / (780.0 - 700.0);
	else
		factor = 0.0;

	const double gamma = 0.8;
	const double intensity_max = 100.0;

	FVector result = { intensity_max * pow(red * factor, gamma), intensity_max * pow(green * factor, gamma), intensity_max * pow(blue * factor, gamma) };

	return result;
}
