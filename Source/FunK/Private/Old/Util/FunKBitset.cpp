#include "Old/Util/FunKBitset.h"

inline void FFunKBitset::Clear(int32 Index)
{
	if (Index >= Length || Index <= INDEX_NONE)
		return;
	
	Bits &= ~(1 << Index);
}

inline void FFunKBitset::Set(int32 Index)
{
	if (Index >= Length || Index <= INDEX_NONE)
		return;
	
	Bits |= (1 << Index);
}

void FFunKBitset::Set(const FFunKBitset& Bitmask)
{
	Bits |= Bitmask.Bits;
}

void FFunKBitset::ClearAll()
{
	const int32 Bitmask = (1 << Length) - 1;
	Bits &= ~Bitmask;
}

void FFunKBitset::SetAll()
{
	const int32 Bitmask = (1 << Length) - 1;
	Bits |= Bitmask;
}

inline bool FFunKBitset::IsClear(int32 Index) const
{
	if (Index >= Length || Index < INDEX_NONE)
		return false;
	
	if (Index == INDEX_NONE)
		return Bits == 0;
	
	return !IsBitSet(Index);
}

inline bool FFunKBitset::IsSet(int32 Index) const
{
	if (Index >= Length || Index < INDEX_NONE)
		return false;
	
	if (Index == INDEX_NONE)
		return Bits == (0 | ((1 << Length) - 1));
	
	return IsBitSet(Index);
}

bool FFunKBitset::IsBitSet(int32 Index) const
{
	const int32 Bitmask = 1 << Index;
	return (Bits & Bitmask) != 0;
}
