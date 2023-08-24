﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/FunKTimeLimit.h"

bool FFunKTimeLimit::IsTimeout(float time) const
{
	return IsLimitless() ? false : Time <= time;
}

bool FFunKTimeLimit::IsLimitless() const
{
	return Time == 0;
}