// Fill out your copyright notice in the Description page of Project Settings.


#include "Util/FunKAssertionBlueprintFunctionLibrary.h"
#include "FunKTestBase.h"
#include "Events/FunKEvent.h"
#include "Misc/AutomationTest.h"

bool UFunKAssertionBlueprintFunctionLibrary::AssertTrue(bool Condition, const FString& Message, UObject* Context)
{
	return Assert([Condition]() { return Condition; }, Message, Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertFalse(bool Condition, const FString& Message, UObject* Context)
{
	return Assert([Condition]() { return !Condition; }, Message, Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertIsValid(UObject* Object, const FString& Message, UObject* Context)
{
	return Assert([Object]() { return IsValid(Object); }, Message, Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertInt(int32 Actual, EFunKAssertionComparisonMethod ShouldBe, int32 Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, FString::FromInt(Expected), FString::FromInt(Actual), ShouldBe), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertFloat(float Actual, EFunKAssertionComparisonMethod ShouldBe, float Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), ShouldBe), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertDouble(double Actual, EFunKAssertionComparisonMethod ShouldBe, double Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), ShouldBe), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertDateTime(FDateTime Actual, EFunKAssertionComparisonMethod ShouldBe, FDateTime Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, ShouldBe, Expected]() { return PerformComparison(Actual, Expected, ShouldBe); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), ShouldBe), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertTransformEqual(const FTransform& Actual, const FTransform& Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertTransformNotEqual(const FTransform& Actual, const FTransform& NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertFloatEqual(float Actual, float Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return FMath::IsNearlyEqual(Actual, Expected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertFloatNotEqual(float Actual, float NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !FMath::IsNearlyEqual(Actual, NotExpected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(NotExpected), FString::SanitizeFloat(Actual), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertDoubleEqual(double Actual, double Expected, const FString& What, UObject* Context, double Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return FMath::IsNearlyEqual(Actual, Expected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(Expected), FString::SanitizeFloat(Actual), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertDoubleNotEqual(double Actual, double NotExpected, const FString& What, UObject* Context, double Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !FMath::IsNearlyEqual(Actual, NotExpected, Tolerance); }, BuildMessage(What, FString::SanitizeFloat(NotExpected), FString::SanitizeFloat(Actual), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertNameEqual(FName Actual, FName Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, Expected]() { return Actual == Expected; }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertNameNotEqual(FName Actual, FName NotExpected, const FString& What, UObject* Context)
{
	return Assert([Actual, NotExpected]() { return Actual != NotExpected; }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertStringEqual(FString Actual, FString Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, Expected]() { return Actual == Expected; }, BuildMessage(What, Expected, Actual, EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertStringNotEqual(FString Actual, FString NotExpected, const FString& What, UObject* Context)
{
	return Assert([Actual, NotExpected]() { return Actual != NotExpected; }, BuildMessage(What, NotExpected, Actual, EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertObjectEqual(UObject* Actual, UObject* Expected, const FString& What, UObject* Context)
{
	return Assert([Actual, Expected]() { return Actual == Expected; }, BuildMessage(What, GetObjectName(Expected), GetObjectName(Actual), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertObjectNotEqual(UObject* Actual, UObject* NotExpected, const FString& What, UObject* Context)
{
	return Assert([Actual, NotExpected]() { return Actual != NotExpected; }, BuildMessage(What, GetObjectName(NotExpected), GetObjectName(Actual), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertRotatorEqual(FRotator Actual, FRotator Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertRotatorNotEqual(FRotator Actual, FRotator NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertVectorEqual(FVector Actual, FVector Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertVectorNotEqual(FVector Actual, FVector NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertVector2DEqual(FVector2D Actual, FVector2D Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertVector2DNotEqual(FVector2D Actual, FVector2D NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertVector4Equal(FVector4 Actual, FVector4 Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertVector4NotEqual(FVector4 Actual, FVector4 NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertQuatEqual(FQuat Actual, FQuat Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertQuatNotEqual(FQuat Actual, FQuat NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AsserMatrixEqual(FMatrix Actual, FMatrix Expected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, Expected, Tolerance]() { return Actual.Equals(Expected, Tolerance); }, BuildMessage(What, Expected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::EqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::AssertMatrixNotEqual(FMatrix Actual, FMatrix NotExpected, const FString& What, UObject* Context, float Tolerance)
{
	return Assert([Actual, NotExpected, Tolerance]() { return !Actual.Equals(NotExpected, Tolerance); }, BuildMessage(What, NotExpected.ToString(), Actual.ToString(), EFunKAssertionComparisonMethod::NotEqualTo), Context);
}

bool UFunKAssertionBlueprintFunctionLibrary::Assert(const TFunctionRef<bool()>& Assertion, const FString& Message, UObject* Context, const int32 StackOffset)
{
	const bool bResult = Assertion();

	FFunKEvent Event = FFunKEvent(bResult ? EFunKEventType::Info : EFunKEventType::Error, Message);
	if (Context)
		Event.AddToContext(Context->GetName());
	
#if WITH_AUTOMATION_TESTS
	SAFE_GETSTACK(Stack, StackOffset, 2);
	Event.AddToContext(Stack[0].FunctionName);
	if (Stack.Num() > 1 && !FString(Stack[1].Filename).Contains(".gen.cpp"))
		Event.AddToContext(FString::Printf(TEXT("%s (%s)"), *FString(Stack[1].FunctionName), *FString::FromInt(Stack[1].LineNumber)));
#endif

	if (const AFunKTestBase* TestBase = Cast<AFunKTestBase>(Context))
	{
		TestBase->RaiseEvent(Event);
	}
	else
	{
		UFunKEventBusSubsystem* EventBus = Context->GetWorld()->GetSubsystem<UFunKEventBusSubsystem>();
		EventBus->Raise(Event);
	}
	
	return bResult;
}

FString UFunKAssertionBlueprintFunctionLibrary::GetComparisonName(EFunKAssertionComparisonMethod Comparison)
{
	const UEnum* Enum = StaticEnum<EFunKAssertionComparisonMethod>();
	return Enum->GetNameStringByValue(static_cast<uint8>(Comparison));
}

FString UFunKAssertionBlueprintFunctionLibrary::BuildMessage(const FString& What, const FString& Expected, const FString& Actual, const EFunKAssertionComparisonMethod ComparisonMethod, const FString& Ext)
{
	return FString::Printf(TEXT("%s: expected %s to be %s%s%s"), *What, *Actual, (ComparisonMethod == EFunKAssertionComparisonMethod::None ? TEXT("") : *GetComparisonName(ComparisonMethod)), *Expected, *Ext);
}

FString UFunKAssertionBlueprintFunctionLibrary::GetObjectName(const UObject* Object)
{
	return Object ? Object->GetName() : "NULL";
}