// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"

/**
 * 
 */
class LITUP_API LevelInfo
{
public:
	LevelInfo(std::string lvlName, UImage lvlImage, std::string lvlDescription);
	~LevelInfo();

private:
	std::string _lvlName;
	UImage* _lvlImagePtr;
	std::string _lvlDescription;
};
