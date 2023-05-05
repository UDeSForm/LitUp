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

	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Diffractor->SetStaticMesh(CubeMeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UMaterial>dMaterial(TEXT("Material'/Game/Materials/M_Diffraction.M_Diffraction'"));
	diffractionMaterial = dMaterial.Object;
}

// Called when the game starts or when spawned
void ALitUpDiffractor::BeginPlay()
{
	Super::BeginPlay();
	Diffractor->SetStaticMesh(DiffractorMeshAsset);

	decal = GetWorld()->SpawnActor<ADecalActor>(Origin->GetComponentLocation(), FRotator(0.f,0.f,0.f));
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

		double r = OutHit.Distance / (double)100;
		double taillePixelM = r / (double)size;
		int fsX = Fente->GetSizeX();
		int fsY = Fente->GetSizeY();
		int nX = fsX;
		int nY = fsY;
		int fX = 0;
		int fY = 0;

		FTexture2DMipMap MyMipMap = Fente->GetPlatformData()->Mips[0];
		FByteBulkData RawImageData = MyMipMap.BulkData;
		FColor* FormatedImageData = static_cast<FColor*>(RawImageData.Lock(LOCK_READ_ONLY));
		for (int i = 0; i < fsX; i++)
		{
			for (int j = 0; j < fsY; j++)
			{
				FColor PixelColor = FormatedImageData[j * fsX + i];
				if (PixelColor.R > 0)
				{
					if (i < nX) nX = i;
					if (j < nY) nY = j;

					if (i > fX) fX = i;
					if (j > fY) fY = j;	
				}
			}
		}
		RawImageData.Unlock();

		// Credits: Petr Klapetek http://gsvit.net/tutorial/a_grating.php

		double p = (fX - nX + 1);
		double q = (fY - nY + 1);
		int ccx = ((p / 2 + nX) * (size/fsX));
		int ccy = ((q / 2 + nY) * (size/fsY));
		double k = ((double)2 * PI) / (WaveLength / (double)1000000000);

		double Apq = p * q;
		p *= pixelFente / (double)1000000000;
		q *= pixelFente / (double)1000000000;
		double r2 = 2 * r;
		double kpoverr2 = (k * p) / r2;
		double kqoverr2 = (k * q) / r2;

		TArray<double> xwave;
		xwave.SetNumZeroed(size);
		for (int x = -ccx; x < size - ccx; x++)
		{
			double kpxoverr2 = kpoverr2 * x;
			xwave[x + ccx] = sin((float)kpxoverr2) / kpxoverr2;
		}

		TArray<double> ywave;
		ywave.SetNumZeroed(size);
		for (int y = -ccy; y < size - ccy; y++)
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

		FCreateTexture2DParameters params;
		params.bUseAlpha = true;
		patronDiffraction = FImageUtils::CreateTexture2D(size, size, pixelsPatron, Diffractor, "Patron", EObjectFlags::RF_Transient, params);
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