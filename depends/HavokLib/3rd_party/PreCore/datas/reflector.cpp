/*	a source for Reflector class
	more info in README for PreCore Project

	Copyright 2018-2019 Lukas Cone

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include "reflector.hpp"
#include "reflectorRegistry.hpp"
#include <algorithm>

const reflType *Reflector::GetReflectedType(const JenHash hash) const
{
	const reflectorStatic *inst = _rfRetreive().rfStatic;
	const int _ntypes = GetNumReflectedValues();
	for (int t = 0; t < _ntypes; t++)
		if (inst->types[t].valueNameHash == hash)
			return inst->types + t;

	return nullptr;
}

static unsigned char subTypeRemap[] = { 2, 4, 6 };

static ES_INLINE void SetReflectedPrimitive(char *objAddr, JenHash type, const char *value)
{
	switch (type)
	{

	case 1:
		*reinterpret_cast<bool*>(objAddr) = !strcmp(value, "true");
		break;

	case 2:
		*(objAddr) = static_cast<char>(std::atoi(value));
		break;
	case 3:
		*(reinterpret_cast<unsigned char*>(objAddr)) = static_cast<unsigned char>(std::atoi(value));
		break;

	case 4:
		*reinterpret_cast<short*>(objAddr) = static_cast<short>(std::atoi(value));
		break;
	case 5:
		*reinterpret_cast<unsigned short*>(objAddr) = static_cast<unsigned short>(std::atoi(value));
		break;

	case 6:
		*reinterpret_cast<int*>(objAddr) = std::atoi(value);
		break;
	case 7:
		*reinterpret_cast<unsigned int*>(objAddr) = static_cast<unsigned int>(std::atoll(value));
		break;

	case 8:
		*reinterpret_cast<int64*>(objAddr) = std::atoll(value);
		break;
	case 9:
		*reinterpret_cast<uint64*>(objAddr) = std::strtoull(value, nullptr, 10);
		break;

	case 10:
		*reinterpret_cast<float*>(objAddr) = static_cast<float>(std::atof(value));
		break;
	case 11:
		*reinterpret_cast<double*>(objAddr) = std::atof(value);
		break;

	case 19:
		*reinterpret_cast<std::string *>(objAddr) = value;
		break;

	default:
		break;
	}
}

static void SetReflectedMember(const reflType *reflValue, const char *value, char *thisAddr)
{
	if (!reflValue)
		return;

	switch (reflValue->typeHash)
	{
	case 12:
	{
		int ival = 0;

		if (REFEnumStorage.count(reflValue->subtypeHash))
		{
			ival = REFEnumStorage[reflValue->subtypeHash].MultiConstructor(value);
		}

		memcpy(thisAddr + reflValue->offset, reinterpret_cast<const char *>(&ival), reflValue->subSize);

		break;
	}

	case 13:
	{
		int ival = 0;

		if (REFEnumStorage.count(reflValue->subtypeHash))
		{
			ival = REFEnumStorage[reflValue->subtypeHash].Constructor(value);
		}

		*reinterpret_cast<int *>(thisAddr + reflValue->offset) = ival;

		break;
	}

	case 14:
	{
		int ival = 0;

		if (REFEnumStorage.count(reflValue->subtypeHash))
		{
			ival = REFEnumStorage[reflValue->subtypeHash].Constructor(value);
		}

		memcpy(thisAddr + reflValue->offset, reinterpret_cast<const char *>(&ival), reflValue->subSize);

		break;
	}

	case 15: //vector2
	case 16: //vector
	case 17: //vector4
	{
		std::string valueCopy = value;
		std::replace(valueCopy.begin(), valueCopy.end(), '[', ' ');
		std::replace(valueCopy.begin(), valueCopy.end(), ']', ' ');

		size_t currentItem = 0;
		size_t lastItem = 0;

		for (size_t i = 0; i < valueCopy.size() + 1; i++)
		{
			if (valueCopy[i] == ',' || valueCopy[i] == '\0')
			{
				//valueCopy[i] = '\0';

				SetReflectedPrimitive(thisAddr + reflValue->offset + (reflValue->subSize * currentItem), reflValue->subtypeHash, (valueCopy.begin() + lastItem).operator->());
				lastItem = i + 1;
				currentItem++;
			}
		}

		break;
	}
	case 20:
	{
		std::string valueCopy = value;
		std::replace(valueCopy.begin(), valueCopy.end(), '{', ' ');
		std::replace(valueCopy.begin(), valueCopy.end(), '}', ' ');

		size_t currentItem = 0;
		size_t lastItem = 0;

		for (size_t i = 0; i < valueCopy.size() + 1; i++)
		{
			if (valueCopy[i] == ',' || valueCopy[i] == '\0')
			{
				SetReflectedPrimitive(thisAddr + reflValue->offset + (reflValue->subSize * currentItem), reflValue->subtypeHash, (valueCopy.begin() + lastItem).operator->());
				lastItem = i + 1;
				currentItem++;
			}
		}

		break;
	}
	default:
		SetReflectedPrimitive(thisAddr + reflValue->offset, reflValue->typeHash, value);
		break;

	}

}

void Reflector::SetReflectedValue(int ID, const char *value)
{
	const reflType *reflValue = GetReflectedType(ID);
	const reflectorInstance inst = _rfRetreive();
	char *thisAddr = static_cast<char *>(inst.rfInstance);

	SetReflectedMember(reflValue, value, thisAddr);
}

void Reflector::SetReflectedValue(const JenHash hash, const char *value)
{
	const reflType *reflValue = GetReflectedType(hash);
	const reflectorInstance inst = _rfRetreive();
	char *thisAddr = static_cast<char *>(inst.rfInstance);
	
	SetReflectedMember(reflValue, value, thisAddr);
}

static ES_INLINE std::string GetReflectedPrimitive(const char *objAddr, JenHash type)
{
	switch (type)
	{

	case 1:
		return *reinterpret_cast<const bool *>(objAddr) ? "true" : "false";

	case 2:
		return std::to_string(static_cast<int>(*reinterpret_cast<const char*>(objAddr)));
	case 3:
		return std::to_string(static_cast<int>(*reinterpret_cast<const unsigned char*>(objAddr)));

	case 4:
		return std::to_string(*reinterpret_cast<const short*>(objAddr));
	case 5:
		return std::to_string(*reinterpret_cast<const unsigned short*>(objAddr));

	case 6:
		return std::to_string(*reinterpret_cast<const int*>(objAddr));
	case 7:
		return std::to_string(*reinterpret_cast<const unsigned int*>(objAddr));

	case 8:
		return std::to_string(*reinterpret_cast<const int64*>(objAddr));
	case 9:
		return std::to_string(*reinterpret_cast<const uint64*>(objAddr));

	case 10:
		return std::to_string(*reinterpret_cast<const float*>(objAddr));
	case 11:
		return std::to_string(*reinterpret_cast<const double*>(objAddr));

	case 18:
		return objAddr;

	case 19:
		return *reinterpret_cast<const std::string *>(objAddr);

	default:
		return "";
	}

}

std::string Reflector::GetReflectedValue(int id) const
{
	if (id >= GetNumReflectedValues())
		return "";

	const reflectorInstanceConst inst = _rfRetreive();
	const char *thisAddr = static_cast<const char *>(inst.rfInstance);
	const reflType &reflValue = inst.rfStatic->types[id];

	const int valueOffset = reflValue.offset;

	switch (reflValue.typeHash)
	{
	case 12:
	{
		if (REFEnumStorage.count(reflValue.subtypeHash))
		{
			std::string oval = {};
			int ival = 0;
			memcpy(reinterpret_cast<char*>(&ival), thisAddr + valueOffset, reflValue.subSize);
			REFEnumStorage[reflValue.subtypeHash].MultiDestructor(oval, ival);
			return oval;
		}
		else
			return "";
	}

	case 13:
	{
		if (REFEnumStorage.count(reflValue.subtypeHash))
		{
			std::string oval = {};
			REFEnumStorage[reflValue.subtypeHash].Destructor(oval, *reinterpret_cast<const int*>(thisAddr + valueOffset));
			return oval;
		}
		else
			return "";
		
	}

	case 14:
	{
		if (REFEnumStorage.count(reflValue.subtypeHash))
		{
			std::string oval = {};
			int ival = 0;

			memcpy(reinterpret_cast<char*>(&ival), thisAddr + valueOffset, reflValue.subSize);

			REFEnumStorage[reflValue.subtypeHash].Destructor(oval, ival);
			return oval;
		}
		else
			return "";
	}

	case 15: //vector2
	case 16: //vector
	case 17: //vector4
	{
		const int numItems = reflValue.numItems;

		std::string outVal = "[";

		for (int i = 0; i < numItems; i++)
		{
			outVal += GetReflectedPrimitive(thisAddr + valueOffset + (static_cast<intptr_t>(reflValue.subSize) * i), reflValue.subtypeHash) + ((i + 1 < numItems) ? ", " : "");
		}

		return outVal + "]";
	}

	case 20:
	{
		const int numItems = reflValue.numItems;

		std::string outVal = "{";

		for (int i = 0; i < numItems; i++)
		{
			outVal += GetReflectedPrimitive(thisAddr + valueOffset + (static_cast<intptr_t>(reflValue.subSize) * i), reflValue.subtypeHash) + ((i + 1 < numItems) ? ", " : "");
		}

		return outVal + "}";
	}
	case 21:
		return "SUBCLASS_TYPE";
	default:
		return GetReflectedPrimitive(thisAddr + valueOffset, reflValue.typeHash);
	}

}

const Reflector::SubClass Reflector::GetReflectedSubClass(int id) const
{
	if (id >= GetNumReflectedValues())
		return {};

	const reflectorInstanceConst inst = _rfRetreive();
	const char *thisAddr = static_cast<const char *>(inst.rfInstance);
	const reflType &reflValue = inst.rfStatic->types[id];

	const int valueOffset = reflValue.offset;

	if (reflValue.typeHash != 21 || !REFSubClassStorage.count(reflValue.subtypeHash))
		return {};

	return { {},{REFSubClassStorage.at(reflValue.subtypeHash), thisAddr + valueOffset} };
}

const Reflector::SubClass Reflector::GetReflectedSubClass(int id)
{
	if (id >= GetNumReflectedValues())
		return {};

	const reflectorInstance inst = _rfRetreive();
	char *thisAddr = static_cast<char *>(inst.rfInstance);
	const reflType & reflValue = inst.rfStatic->types[id];

	const int valueOffset = reflValue.offset;

	if (reflValue.typeHash != 21 || !REFSubClassStorage.count(reflValue.subtypeHash))
		return {};

	return { {REFSubClassStorage.at(reflValue.subtypeHash), thisAddr + valueOffset},{REFSubClassStorage.at(reflValue.subtypeHash), thisAddr + valueOffset} };
}

RefEnumMapper REFEnumStorage;
RefSubClassMapper REFSubClassStorage;