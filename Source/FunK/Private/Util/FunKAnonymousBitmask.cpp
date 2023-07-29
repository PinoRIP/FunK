#include "Util/FunKAnonymousBitmask.h"

#include "MathUtil.h"

inline void FFunKAnonymousBitmask::Clear(int32 Index)
{
	if(Index >= Length || Index <= INDEX_NONE) return;
	Bits &= ~(1 << Index);
}

inline void FFunKAnonymousBitmask::Set(int32 Index)
{
	if(Index >= Length || Index <= INDEX_NONE) return;
	Bits |= (1 << Index);
}

void FFunKAnonymousBitmask::Set(const FFunKAnonymousBitmask& Bitmask)
{
	Bits |= Bitmask.Bits;
}

void FFunKAnonymousBitmask::ClearAll()
{
	const int32 Bitmask = (1 << Length) - 1;
	Bits &= ~Bitmask;
}

void FFunKAnonymousBitmask::SetAll()
{
	const int32 Bitmask = (1 << Length) - 1;
	Bits |= Bitmask;
}

inline bool FFunKAnonymousBitmask::IsClear(int32 Index) const
{
	if(Index >= Length || Index < INDEX_NONE) return false;
	if(Index == INDEX_NONE) return Bits == 0;
	else return !IsBitSet(Index);
}

inline bool FFunKAnonymousBitmask::IsSet(int32 Index) const
{
	if(Index >= Length || Index < INDEX_NONE) return false;
	if(Index == INDEX_NONE) return Bits == (0 | ((1 << Length) - 1));
	else return IsBitSet(Index);
}

bool FFunKAnonymousBitmask::IsBitSet(int32 Index) const
{
	const int32 Bitmask = 1 << Index;
	return (Bits & Bitmask) != 0;
}
