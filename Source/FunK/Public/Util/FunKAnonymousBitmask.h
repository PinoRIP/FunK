#pragma once

struct FUNK_API FFunKAnonymousBitmask
{
public:
	FFunKAnonymousBitmask()
		: Length(1) { }
	
	FFunKAnonymousBitmask(int32 length)
		: Length(length)
	{
		check(Length > 0);
	}
	
	void Clear(int32 Index);
	void Set(int32 Index);
	
	void ClearAll();
	void SetAll();

	bool IsClear(int32 Index = INDEX_NONE) const;
	bool IsSet(int32 Index = INDEX_NONE) const;
private:
	int32 Length;
	int32 Bits = 0;
	
	bool IsBitSet(int32 Index) const;
};
