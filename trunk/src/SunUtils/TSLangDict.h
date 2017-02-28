#ifndef _TSLANGDICT_H_
#define _TSLANGDICT_H_

#include "TSCommonTypeDef.h"
#include "TSMap.h"
#include "TSString.h"

namespace TSun
{
	// Language dictionary class
	class LangDict
	{
	public:
		// constructor
		LangDict(MemAllocator* allocator = getDefaultMemAllocator());
		// destructor
		~LangDict();
	protected:
		// key string list
		Map<TU64, WString> mStringList;
		// memory allocator
		DEFINE_MEM_ALLOCATOR_MEMBER;
	public:
		// load from file
		// note : this will call clear
		TBOOL loadFromFile(const TCHAR* filename);
		// save to file
		TBOOL saveToFile(const TCHAR* filename);
		// clear strings
		TVOID clearStrings();
		// find string by key
		TBOOL findByKey(TU64 key, WString& value);
	};
}

#endif