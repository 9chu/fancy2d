#include "fuiProperty.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

// void*

void fuiPropertyAccessorHelper<void*>::DefaultGetter(std::wstring& PropData, void* const* Value)
{
	fCharW tBuffer[32];
	if(sizeof(void*) == 4)
		swprintf_s(tBuffer, L"@%u", (fuInt)*Value);
	else if(sizeof(void*) == 8)
		swprintf_s(tBuffer, L"@%I64u", (fuLong)*Value);
	else
		throw fcyException("fuiPropertyAccessorHelper<void*>::DefaultGetter", "sizeof(void*) unexpected.");

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<void*>::DefaultSetter(const std::wstring& PropData, void** Value)
{
	void* tValue;

	if(sizeof(void*) == 4)
	{
		if(1 != swscanf_s(PropData.c_str(), L"@%d", (fuInt*)&tValue))
			throw fcyException("fuiPropertyAccessorHelper<void*>::DefaultSetter", "Property string is not correct.");
	}
	else if(sizeof(void*) == 8)
	{
		if(1 != swscanf_s(PropData.c_str(), L"@%I64u", (fuLong*)&tValue))
			throw fcyException("fuiPropertyAccessorHelper<void*>::DefaultSetter", "Property string is not correct.");
	}
	else
		throw fcyException("fuiPropertyAccessorHelper<void*>::DefaultSetter", "sizeof(void*) unexpected.");

	*Value = tValue;
}

// int

void fuiPropertyAccessorHelper<int>::DefaultGetter(std::wstring& PropData, const int* Value)
{
	fCharW tBuffer[32];
	swprintf_s(tBuffer, L"%d", *Value);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<int>::DefaultSetter(const std::wstring& PropData, int* Value)
{
	int tValue;

	if(1 != swscanf_s(PropData.c_str(), L"%d", &tValue))
		throw fcyException("fuiPropertyAccessorHelper<int>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// bool

void fuiPropertyAccessorHelper<bool>::DefaultGetter(std::wstring& PropData, const bool* Value)
{
	if(*Value == true)
		PropData = L"true";
	else
		PropData = L"false";
}

void fuiPropertyAccessorHelper<bool>::DefaultSetter(const std::wstring& PropData, bool* Value)
{
	bool tValue;

	if(_wcsicmp(PropData.c_str(), L"true") == 0)
		tValue = true;
	else if(_wcsicmp(PropData.c_str(), L"false") == 0)
		tValue = false;
	else if(_wcsicmp(PropData.c_str(), L"1") == 0)
		tValue = true;
	else if(_wcsicmp(PropData.c_str(), L"0") == 0)
		tValue = false;
	else
		throw fcyException("fuiPropertyAccessorHelper<bool>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// float

void fuiPropertyAccessorHelper<float>::DefaultGetter(std::wstring& PropData, const float* Value)
{
	fCharW tBuffer[64];
	swprintf_s(tBuffer, L"%f", *Value);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<float>::DefaultSetter(const std::wstring& PropData, float* Value)
{
	float tValue;

	if(1 != swscanf_s(PropData.c_str(), L"%f", &tValue))
		throw fcyException("fuiPropertyAccessorHelper<float>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyVec2

void fuiPropertyAccessorHelper<fcyVec2>::DefaultGetter(std::wstring& PropData, const fcyVec2* Value)
{
	fCharW tBuffer[64*2];
	swprintf_s(tBuffer, L"%f,%f", Value->x, Value->y);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyVec2>::DefaultSetter(const std::wstring& PropData, fcyVec2* Value)
{
	fcyVec2 tValue;

	if(2 != swscanf_s(PropData.c_str(), L"%f,%f", &tValue.x, &tValue.y))
		throw fcyException("fuiPropertyAccessorHelper<fcyVec2>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyVec3

void fuiPropertyAccessorHelper<fcyVec3>::DefaultGetter(std::wstring& PropData, const fcyVec3* Value)
{
	fCharW tBuffer[64*3];
	swprintf_s(tBuffer, L"%f,%f,%f", Value->x, Value->y, Value->z);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyVec3>::DefaultSetter(const std::wstring& PropData, fcyVec3* Value)
{
	fcyVec3 tValue;

	if(3 != swscanf_s(PropData.c_str(), L"%f,%f,%f", &tValue.x, &tValue.y, &tValue.z))
		throw fcyException("fuiPropertyAccessorHelper<fcyVec3>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyVec4

void fuiPropertyAccessorHelper<fcyVec4>::DefaultGetter(std::wstring& PropData, const fcyVec4* Value)
{
	fCharW tBuffer[64*4];
	swprintf_s(tBuffer, L"%f,%f,%f,%f", Value->x, Value->y, Value->z, Value->w);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyVec4>::DefaultSetter(const std::wstring& PropData, fcyVec4* Value)
{
	fcyVec4 tValue;

	if(4 != swscanf_s(PropData.c_str(), L"%f,%f,%f,%f", &tValue.x, &tValue.y, &tValue.z, &tValue.w))
		throw fcyException("fuiPropertyAccessorHelper<fcyVec4>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyRect

void fuiPropertyAccessorHelper<fcyRect>::DefaultGetter(std::wstring& PropData, const fcyRect* Value)
{
	fCharW tBuffer[64*4];
	swprintf_s(tBuffer, L"%f,%f,%f,%f", Value->a.x, Value->a.y, Value->b.x, Value->b.y);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyRect>::DefaultSetter(const std::wstring& PropData, fcyRect* Value)
{
	fcyRect tValue;

	if(4 != swscanf_s(PropData.c_str(), L"%f,%f,%f,%f", &tValue.a.x, &tValue.a.y, &tValue.b.x, &tValue.b.y))
		throw fcyException("fuiPropertyAccessorHelper<fcyRect>::DefaultSetter", "Property string is not correct.");

	*Value = tValue;
}

// fcyColor

void fuiPropertyAccessorHelper<fcyColor>::DefaultGetter(std::wstring& PropData, const fcyColor* Value)
{
	fCharW tBuffer[64];
	swprintf_s(tBuffer, L"%hhu,%hhu,%hhu,%hhu", Value->a, Value->r, Value->g, Value->b);

	PropData = tBuffer;
}

void fuiPropertyAccessorHelper<fcyColor>::DefaultSetter(const std::wstring& PropData, fcyColor* Value)
{
	float t[4];

	if(4 != swscanf_s(PropData.c_str(), L"%f,%f,%f,%f", &t[0], &t[1], &t[2], &t[3]))
	{
		if(3 != swscanf_s(PropData.c_str(), L"%f,%f,%f", &t[1], &t[2], &t[3]))
			throw fcyException("fuiPropertyAccessorHelper<fcyColor>::DefaultSetter", "Property string is not correct.");
		else
			t[0] = 255.f;
	}

	*Value = fcyColor((int)t[0], (int)t[1], (int)t[2], (int)t[3]);
}

// std::wstring

void fuiPropertyAccessorHelper<std::wstring>::DefaultGetter(std::wstring& PropData, const std::wstring* Value)
{
	PropData = *Value;
}

void fuiPropertyAccessorHelper<std::wstring>::DefaultSetter(const std::wstring& PropData, std::wstring* Value)
{
	*Value = PropData;
}

////////////////////////////////////////////////////////////////////////////////

void fuiPropertySet::RegisterProperty(const std::wstring& Str, fuiProperty* pProp)
{
	m_PropList[Str] = pProp;
}

fuiProperty* fuiPropertySet::QueryPropertyInterface(const std::wstring& Prop)
{
	unordered_map<std::wstring, fuiProperty*>::const_iterator i = m_PropList.find(Prop);
	if(i == m_PropList.end())
		throw fcyException("fuiControl::RawGetProperty", "Property is not exsit.");
	else
		return (i->second);
}

const std::wstring& fuiPropertySet::RawGetProperty(const std::wstring& PropName)const
{
	unordered_map<std::wstring, fuiProperty*>::const_iterator i = m_PropList.find(PropName);
	if(i == m_PropList.end())
		throw fcyException("fuiControl::RawGetProperty", "Property is not exsit.");
	else
		return (i->second)->Get();
}

void fuiPropertySet::RawSetProperty(const std::wstring& PropName, const std::wstring& Value)
{
	unordered_map<std::wstring, fuiProperty*>::iterator i = m_PropList.find(PropName);
	if(i == m_PropList.end())
		throw fcyException("fuiControl::RawGetProperty", "Property is not exsit.");
	else
		(i->second)->Set(Value);
}
