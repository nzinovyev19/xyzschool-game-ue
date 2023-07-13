// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"

void UAmmoWidget::UpdateAmmoCount(int32 NewAmmo, int32 NewTotalAmmo, int32 NewGrenadeAmount)
{
	Ammo = NewAmmo;
	TotalAmmo = NewTotalAmmo;
}

void UAmmoWidget::UpdateGrenadeCount(int32 NewGrenadeAmount)
{
	GrenadeAmount = NewGrenadeAmount;
}

