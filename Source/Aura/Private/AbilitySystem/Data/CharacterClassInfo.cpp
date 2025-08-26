// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	FCharacterClassDefaultInfo Info = CharacterClassInformation.FindChecked(CharacterClass);
	return Info;
}
