// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKAssertions.h"

#include "FunKTestBase.h"
#include "EventBus/FunKEventBusSubsystem.h"
#include "Events/FunKEvent.h"
#include "Misc/AutomationTest.h"

bool UFunKAssertions::AssertTrue(bool Condition, const FString& Message, UObject* Context)
{
	return Assert([Condition]() { return Condition; }, Message, Context);
}

bool UFunKAssertions::AssertFalse(bool Condition, const FString& Message, UObject* Context)
{
	return Assert([Condition]() { return !Condition; }, Message, Context);
}

bool UFunKAssertions::AssertIsValid(UObject* Object, const FString& Message, UObject* Context)
{
	return Assert([Object]() { return IsValid(Object); }, Message, Context);
}

bool UFunKAssertions::AssertInt(int32 Actual, EFunKComparisonMethod ShouldBe, int32 Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, FString::FromInt(Expected), FString::FromInt(Actual), ShouldBe), Context);
}

bool UFunKAssertions::AssertFloat(float Actual, EFunKComparisonMethod ShouldBe, float Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), ShouldBe), Context);
}

bool UFunKAssertions::AssertDouble(double Actual, EFunKComparisonMethod ShouldBe, double Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), ShouldBe), Context);
}

bool UFunKAssertions::AssertDateTime(FDateTime Actual, EFunKComparisonMethod ShouldBe, FDateTime Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), ShouldBe), Context);
}

bool UFunKAssertions::AssertTransformEqual(const FTransform& Actual, const FTransform& Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertTransformNotEqual(const FTransform& Actual, const FTransform& NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertFloatEqual(float Actual, float Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return FMath::IsNearlyEqual(Actual, Expected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertFloatNotEqual(float Actual, float NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !FMath::IsNearlyEqual(Actual, NotExpected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(NotExpected), FString::SanitizeFloat(Actual), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertDoubleEqual(double Actual, double Expected, const FString& What, UObject* Context, double Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return FMath::IsNearlyEqual(Actual, Expected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertDoubleNotEqual(double Actual, double NotExpected, const FString& What, UObject* Context, double Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !FMath::IsNearlyEqual(Actual, NotExpected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(NotExpected), FString::SanitizeFloat(Actual), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertNameEqual(FName Actual, FName Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, Expected]() { return Actual == Expected; }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertNameNotEqual(FName Actual, FName NotExpected, const FString& What, UObject* Context)
{
	return Assert([Actual, NotExpected]() { return Actual != NotExpected; }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertStringEqual(FString Actual, FString Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, Expected]() { return Actual == Expected; }, BuildMessage(What, Expected, Actual, EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertStringNotEqual(FString Actual, FString NotExpected, const FString& What, UObject* Context)
{
	return Assert([Actual, NotExpected]() { return Actual != NotExpected; }, BuildMessage(What, NotExpected, Actual, EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertObjectEqual(UObject* Actual, UObject* Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, Expected]() { return Actual == Expected; }, BuildMessage(What, GetObjectName(Expected), GetObjectName(Actual), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertObjectNotEqual(UObject* Actual, UObject* NotExpected, const FString& What, UObject* Context)
{
	return Assert([Actual, NotExpected]() { return Actual != NotExpected; }, BuildMessage(What, GetObjectName(NotExpected), GetObjectName(Actual), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertRotatorEqual(FRotator Actual, FRotator Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertRotatorNotEqual(FRotator Actual, FRotator NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertVectorEqual(FVector Actual, FVector Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertVectorNotEqual(FVector Actual, FVector NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertVector2DEqual(FVector2D Actual, FVector2D Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertVector2DNotEqual(FVector2D Actual, FVector2D NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AssertVector4Equal(FVector4 Actual, FVector4 Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertVector4NotEqual(FVector4 Actual, FVector4 NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AsserQuatEqual(FQuat Actual, FQuat Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertQuatNotEqual(FQuat Actual, FQuat NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::AsserMatrixEqual(FMatrix Actual, FMatrix Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKComparisonMethod::EqualTo), Context);
}

bool UFunKAssertions::AssertMatrixNotEqual(FMatrix Actual, FMatrix NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertions::Assert(TFunctionRef<bool()> Assertion, const FString& Message, UObject* Context, int32 stackOffset)
{
	const bool result = Assertion();
	
	auto event = FFunKEvent(result ? EFunKEventType::Info : EFunKEventType::Error, Message);
	if(Context)
		event.AddToContext(Context->GetName());
	
#if WITH_AUTOMATION_TESTS
	SAFE_GETSTACK(Stack, stackOffset, 2);
	event.AddToContext(Stack[0].FunctionName);
	if(Stack.Num() > 1 && !FString(Stack[1].Filename).Contains(".gen.cpp"))
		event.AddToContext(FString::Printf(TEXT("%s (%s)"), *FString(Stack[1].FunctionName), *FString::FromInt(Stack[1].LineNumber)));
#endif

	if(const AFunKTestBase* TestBase = Cast<AFunKTestBase>(Context))
	{
		TestBase->RaiseEvent(event);
	}
	else
	{
		UFunKEventBusSubsystem* eventBus = Context->GetWorld()->GetSubsystem<UFunKEventBusSubsystem>();
		eventBus->Raise(event);
	}
	
	return result;
}

FString UFunKAssertions::GetComparisonName(EFunKComparisonMethod comparison)
{
	const UEnum* Enum = StaticEnum<EFunKComparisonMethod>();
	return Enum->GetNameStringByValue(static_cast<uint8>(comparison));
}

FString UFunKAssertions::BuildMessage(const FString& What, const FString& Expected, const FString& Actual, EFunKComparisonMethod ComparisonMethod, const FString Ext)
{
	return FString::Printf(TEXT("%s: expected %s to be %s%s%s"), *What, *Actual, (ComparisonMethod == EFunKComparisonMethod::None ? TEXT("") : *GetComparisonName(ComparisonMethod)), *Expected, *Ext);
}

FString UFunKAssertions::GetObjectName(UObject* object)
{
	return object ? object->GetName() : "NULL";
}