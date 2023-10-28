#pragma once

#include "CoreMinimal.h"
#include "FunKBitset.generated.h"

// Bitmask with variable length
USTRUCT()
struct FUNK_API FFunKBitset
{
	GENERATED_BODY()
	
public:
	FFunKBitset()
		: Length(1) { }
	
	FFunKBitset(const int32 InLength)
		: Length(InLength)
	{
		check(Length > 0);
		check(Length <= 32);
	}
	
	void Clear(int32 Index);
	void Set(int32 Index);
	void Set(const FFunKBitset& Bitmask);
	
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
