// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpDiffractor.h"

// Sets default values
ALitUpDiffractor::ALitUpDiffractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Origin = CreateDefaultSubobject<USceneComponent>(TEXT("Origin"));
	SetRootComponent(Origin);

	Diffractor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Diffractor"));
	Diffractor->SetupAttachment(Origin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Game/CustomActors/Intermediates/diffractor.diffractor'"));
	Diffractor->SetStaticMesh(CubeMeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial>dMaterial(TEXT("Material'/Game/Materials/M_Diffraction.M_Diffraction'"));
	diffractionMaterial = dMaterial.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial>diffractorMaterial(TEXT("Material'/Game/CustomActors/Intermediates/Material.Material'"));
	Diffractor->SetMaterial(0, diffractorMaterial.Object);
}

// Called when the game starts or when spawned
void ALitUpDiffractor::BeginPlay()
{
	Super::BeginPlay();
	Diffractor->SetStaticMesh(DiffractorMeshAsset);
	
	FActorSpawnParameters sp;
	sp.ObjectFlags = RF_Transient;
	decal = GetWorld()->SpawnActor<ADecalActor>(Origin->GetComponentLocation(), FRotator(0.f,0.f,0.f), sp);
	if (decal)
	{
		decal->GetDecal()->DecalSize = FVector(50.0f, 50.0f, 50.0f);
		decal->SetActorHiddenInGame(true);
		decal->SetDecalMaterial(diffractionMaterial);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No decal spawned"));
	}

	CalculerPatronDiffraction();

	if (decal)
	{
		UMaterialInstanceDynamic* dynamicDiffractionMaterialInstanceDynamic = decal->CreateDynamicMaterialInstance();
		dynamicDiffractionMaterialInstanceDynamic->SetTextureParameterValue("Patron", patronDiffraction);
		decal->SetDecalMaterial(dynamicDiffractionMaterialInstanceDynamic);
	}
}

// Called every frame
void ALitUpDiffractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (decal) decal->SetActorHiddenInGame(!showDecal);
	showDecal = false;
}

// This ultimately is what controls whether or not it can even tick at all in the editor view port. 
//But, it is EVERY view port so it still needs to be blocked from preview windows and junk.
bool ALitUpDiffractor::ShouldTickIfViewportsOnly() const
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

inline void ALitUpDiffractor::CalculerPatronDiffraction()
{
	FVector Start = Origin->GetComponentLocation();
	FVector ForwardVector = Origin->GetForwardVector();
	FHitResult OutHit;
	FVector End = ((ForwardVector * 10000.f) + Start);

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		//DrawDebugLine(GetWorld(), Start, OutHit.Location, FColor::Green, true, 0.04, 0, 10);

		Start += ForwardVector * 100.f;

		decal->SetActorTransform(FTransform(FRotator(0, 0, 90) + ForwardVector.Rotation(), (OutHit.Location - Start) / 2.f + Start + ForwardVector * 0.01, FVector(OutHit.Distance / 100.f, OutHit.Distance / 100.f, OutHit.Distance / 100.f)));

		// Credits: Petr Klapetek http://gsvit.net/tutorial/a_grating.php
		
		double r2 = OutHit.Distance / (double)50;
		double p = largeurFente;
		double q = hauteurFente;
		int ccx = size / 2;
		int ccy = size / 2;
		double k = ((double)2 * PI) / (WaveLength / (double)1000000000);

		double Apq = (0.01 / r2) * p * q;
		p /= (double)1000000000;
		q /= (double)1000000000;
		double kpoverr2 = (k * p) / r2;
		double kqoverr2 = (k * q) / r2;

		TArray<double> xwave;
		xwave.SetNumZeroed(size);
		for (int x = -ccx; x < ccx; x++)
		{
			double kpxoverr2 = kpoverr2 * x;
			xwave[x + ccx] = sin((float)kpxoverr2) / kpxoverr2;
		}

		TArray<double> ywave;
		ywave.SetNumZeroed(size);
		for (int y = -ccy; y < ccy; y++)
		{
			double kqyoverr2 = kqoverr2 * y;
			ywave[y + ccy] = sin((float)kqyoverr2) / kqyoverr2;
		}

		FVector colorPatron = calculateColorFromWaveLength();
		pixelsPatron.SetNumZeroed(size * size);
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				double sqrtresult = Apq * xwave[x] * ywave[y];
				double result = sqrtresult * sqrtresult;
				pixelsPatron[x + y * size] = FColor(colorPatron.X, colorPatron.Y, colorPatron.Z, result * 128);
			}
		}

		patronDiffraction = UTexture2D::CreateTransient(size, size, PF_B8G8R8A8);
		void* TextureData = patronDiffraction->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, pixelsPatron.GetData(), 4 * size * size);
		patronDiffraction->PlatformData->Mips[0].BulkData.Unlock();
		patronDiffraction->UpdateResource();
	}
}

void ALitUpDiffractor::exec(float rayWaveLength)
{
	if (rayWaveLength == WaveLength)
	{
		showDecal = true;
	}
}

inline FVector ALitUpDiffractor::calculateColorFromWaveLength()
{
	// Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
	double red = 0.0;
	double green = 0.0;
	double blue = 0.0;

	if ((380.0 <= WaveLength) && (WaveLength <= 439.0))
	{
		red = -(WaveLength - 440.0) / (440.0 - 380.0);
		green = 0.0;
		blue = 1.0;
	}
	else if ((440.0 <= WaveLength) && (WaveLength <= 489.0))
	{
		red = 0.0;
		green = (WaveLength - 440.0) / (490.0 - 440.0);
		blue = 1.0;
	}
	else if ((490.0 <= WaveLength) && (WaveLength <= 509.0))
	{
		red = 0.0;
		green = 1.0;
		blue = -(WaveLength - 510.0) / (510.0 - 490.0);
	}
	else if ((510.0 <= WaveLength) && (WaveLength <= 579.0))
	{
		red = (WaveLength - 510.0) / (580.0 - 510.0);
		green = 1.0;
		blue = 0.0;
	}
	else if ((580.0 <= WaveLength) && (WaveLength <= 644.0))
	{
		red = 1.0;
		green = -(WaveLength - 645.0) / (645.0 - 580.0);
		blue = 0.0;
	}
	else if ((645.0 <= WaveLength) && (WaveLength <= 780.0))
	{
		red = 1.0;
		green = 0.0;
		blue = 0.0;
	}

	double factor = 0.0;

	if ((380.0 <= WaveLength) && (WaveLength <= 419.0))
		factor = 0.3 + 0.7 * (WaveLength - 380.0) / (420.0 - 380.0);
	else if ((420.0 <= WaveLength) && (WaveLength <= 700.0))
		factor = 1.0;
	else if ((701.0 <= WaveLength) && (WaveLength <= 780.0))
		factor = 0.3 + 0.7 * (780.0 - WaveLength) / (780.0 - 700.0);
	else
		factor = 0.0;

	const double gamma = 0.8;
	const double intensity_max = 255;

	FVector result = { intensity_max * pow(red * factor, gamma), intensity_max * pow(green * factor, gamma), intensity_max * pow(blue * factor, gamma) };

	return result;
}