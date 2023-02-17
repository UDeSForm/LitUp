// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelInfo.h"


LevelInfo::LevelInfo(std::string lvlName, UImage lvlImage, std::string lvlDescription)
	: _lvlName(lvlName), _lvlImage(NewObject<UImage>(lvlImage), _lvlDescription(lvlDescription)
{


}

LevelInfo::~LevelInfo()
{
}
