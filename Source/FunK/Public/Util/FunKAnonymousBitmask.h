#pragma once

#include "CoreMinimal.h"
#include "FunKAnonymousBitmask.generated.h"

// Bitmask with variable length
USTRUCT()
struct FUNK_API FFunKAnonymousBitmask
{
	GENERATED_BODY()
	
public:
	FFunKAnonymousBitmask()
		: Length(1) { }
	
	FFunKAnonymousBitmask(const int32 InLength)
		: Length(InLength)
	{
		check(Length > 0);
		check(Length <= 32);
	}
	
	void Clear(int32 Index);
	void Set(int32 Index);
	void Set(const FFunKAnonymousBitmask& Bitmask);
	
	void ClearAll();
	void SetAll();

	bool IsClear(int32 Index = INDEX_NONE) const;
	bool IsSet(int32 Index = INDEX_NONE) const;

	int32 Num() const { return Length; }
	
private:
	UPROPERTY()
	int32 Length;

	UPROPERTY()
	int32 Bits = 0;
	
	bool IsBitSet(int32 Index) const;
};
