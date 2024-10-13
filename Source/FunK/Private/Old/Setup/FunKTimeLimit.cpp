// Fill out your copyright notice in the Description page of Project Settings.


#include "Old/Setup/FunKTimeLimit.h"

bool FFunKTimeLimit::IsTimeout(float MsPassed) const
{
	return IsLimitless() ? false : Time <= MsPassed;
}

bool FFunKTimeLimit::IsLimitless() const
{
	return Time == 0;
}
