// Fill out your copyright notice in the Description page of Project Settings.


#include "LitUpDiffractor.h"

// Sets default values
ALitUpDiffractor::ALitUpDiffractor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Diffractor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Diffractor"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	Diffractor->SetStaticMesh(CubeMeshAsset.Object);
	SetRootComponent(Diffractor);
}

// Called when the game starts or when spawned
void ALitUpDiffractor::BeginPlay()
{
	Super::BeginPlay();
	Diffractor->SetStaticMesh(DiffractorMeshAsset);
	//CalculerPatronDiffraction();
}

// Called every frame
void ALitUpDiffractor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

//inline void ALitUpDiffractor::CalculerPatronDiffraction()
//{
//	//https://isaratech.com/ue4-reading-the-pixels-from-a-utexture2d/
//	TextureCompressionSettings OldCompressionSettings = Fente->CompressionSettings; TextureMipGenSettings OldMipGenSettings = Fente->MipGenSettings; bool OldSRGB = Fente->SRGB;
//
//	Fente->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
//	Fente->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
//	Fente->SRGB = false;
//	Fente->UpdateResource();
//
//	const FColor* FormatedImageData = static_cast<const FColor*>(Fente->PlatformData->Mips[0].BulkData.LockReadOnly());
//	int sizeX = Fente->GetSizeX();
//	int sizeY = Fente->GetSizeY();
//
//	for (int32 X = 0; X < sizeX; X++)
//	{
//		for (int32 Y = 0; Y < sizeY; Y++)
//		{
//			FColor PixelColor = FormatedImageData[Y * sizeX + X];
//			if (PixelColor.R > 128)
//			{
//				//Calculer 2d array de valeurs d'intensité pour ce pixel de la fente
//				//et accumuler les valeurs d'intensité des 2d arrays
//				//décimale intensité: intensité - floor(intensité)
//				//scale intensité pour minimum et maximum intensité: 2|intensité - 0,5|
//			}
//		}
//	}
//
//
//
//	Fente->PlatformData->Mips[0].BulkData.Unlock();
//
//	Fente->CompressionSettings = OldCompressionSettings;
//	Fente->MipGenSettings = OldMipGenSettings;
//	Fente->SRGB = OldSRGB;
//	Fente->UpdateResource();
//}

void ALitUpDiffractor::exec()
{
	
}