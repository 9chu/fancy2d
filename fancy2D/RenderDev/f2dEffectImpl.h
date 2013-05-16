////////////////////////////////////////////////////////////////////////////////
/// @file  f2dEffectImpl.h
/// @brief fancy2D D3DXEffect包装
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "f2dRenderDeviceAPI.h"

#include <unordered_map>

template<class T>
class f2dEffectParamImpl :
	public T
{
protected:
	f2dEffect* m_pParent;

	D3DXHANDLE m_pHandle;
	D3DXPARAMETER_DESC m_Desc;

	// 子对象缓存
	std::unordered_map<D3DXHANDLE, f2dEffectParam*> m_ObjCache;
protected:
	f2dEffectParam* GetCacheObj(D3DXHANDLE pHandle)
	{
		if(m_ObjCache.find(pHandle) == m_ObjCache.end())
		{
			m_ObjCache[pHandle] = f2dEffectImpl::CreateEffectParam(m_pParent, pHandle);
		}

		return m_ObjCache[pHandle];
	}
public: // 接口实现
	fcStr GetName()
	{
		return m_Desc.Name;
	}
	F2DEPTYPE GetType()
	{
		switch(m_Desc.Class)
		{
		case D3DXPC_SCALAR:
			return F2DEPTYPE_VALUE;
		case D3DXPC_VECTOR:
			if(m_Desc.Type == D3DXPT_FLOAT)
				return F2DEPTYPE_OBJECT;
			else
				return F2DEPTYPE_UNKNOWN;
		case D3DXPC_MATRIX_ROWS:
		case D3DXPC_MATRIX_COLUMNS:
			return F2DEPTYPE_ARRAY;
		case D3DXPC_OBJECT:
			return F2DEPTYPE_OBJECT;
		case D3DXPC_STRUCT:
			return F2DEPTYPE_STRUCT;
		}

		return F2DEPTYPE_UNKNOWN;
	}
	fcStr GetSemantic()
	{
		return m_Desc.Semantic;
	}
	f2dEffectParam* GetAnnotation(fcStr Name)
	{
		D3DXHANDLE tHandle = ((ID3DXEffect*)m_pParent->GetHandle())->GetAnnotationByName(m_pHandle, Name);

		if(tHandle == NULL)
			return NULL;

		return GetCacheObj(tHandle);
	}
	f2dEffectParam* GetAnnotation(fuInt Index)
	{
		D3DXHANDLE tHandle = ((ID3DXEffect*)m_pParent->GetHandle())->GetAnnotation(m_pHandle, Index);

		if(tHandle == NULL)
			return NULL;

		return GetCacheObj(tHandle);
	}
	fuInt GetAnnotationCount()
	{
		return m_Desc.Annotations;
	}
protected:
	f2dEffectParamImpl(f2dEffect* pParent, D3DXHANDLE pThisHandle, const D3DXPARAMETER_DESC& Desc)
		: m_pParent(pParent), m_pHandle(pThisHandle), m_Desc(Desc) { }
};

class f2dEffectParamValueImpl :
	public f2dEffectParamImpl<f2dEffectParamValue>
{
	friend class f2dEffectImpl;
public: // 接口实现
	F2DEPVTYPE GetValueType()
	{
		switch(m_Desc.Type)
		{
		case D3DXPT_VOID:
			return F2DEPVTYPE_VOID;
		case D3DXPT_BOOL:
			return F2DEPVTYPE_BOOL;
		case D3DXPT_INT:
			return F2DEPVTYPE_INT;
		case D3DXPT_FLOAT:
			return F2DEPVTYPE_FLOAT;
		}

		return F2DEPVTYPE_UNKNOWN;
	}
	fBool GetBool()
	{
		BOOL tRet;
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->GetBool(m_pHandle, &tRet)))
			return false;

		return tRet == TRUE ? true : false;
	}
	fFloat GetFloat()
	{
		float tRet;
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->GetFloat(m_pHandle, &tRet)))
			return 0.f;

		return tRet;
	}
	fInt GetInt()
	{
		int tRet;
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->GetInt(m_pHandle, &tRet)))
			return 0;

		return tRet;
	}
	fResult SetBool(fBool Value)
	{
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->SetBool(m_pHandle, Value)))
			return FCYERR_INTERNALERR;
		else
			return FCYERR_OK;
	}
	fResult SetFloat(fFloat Value)
	{
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->SetFloat(m_pHandle, Value)))
			return FCYERR_INTERNALERR;
		else
			return FCYERR_OK;
	}
	fResult SetInt(fInt Value)
	{
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->SetInt(m_pHandle, Value)))
			return FCYERR_INTERNALERR;
		else
			return FCYERR_OK;
	}
protected:
	f2dEffectParamValueImpl(f2dEffect* pParent, D3DXHANDLE pThisHandle, const D3DXPARAMETER_DESC& Desc)
		: f2dEffectParamImpl(pParent, pThisHandle, Desc) { }
};

// 尚未实现
class f2dEffectParamObjImpl :
	public f2dEffectParamImpl<f2dEffectParamObj>
{
	friend class f2dEffectImpl;
public: // 接口实现
	F2DEPOTYPE GetObjType()
	{
		if(m_Desc.Class == D3DXPC_VECTOR)
			return F2DEPOTYPE_VECTOR;
		else
		{
			switch (m_Desc.Type)
			{
			case D3DXPT_STRING:
				return F2DEPOTYPE_STRING;
			case D3DXPT_TEXTURE:
			case D3DXPT_TEXTURE1D:
			case D3DXPT_TEXTURE2D:
			case D3DXPT_TEXTURE3D:
				return F2DEPOTYPE_TEXTURE;
			}
		}

		return F2DEPOTYPE_UNKNOWN;
	}
	fcStr GetString()
	{
		LPCSTR tRet;
		if(FAILED(((ID3DXEffect*)m_pParent->GetHandle())->GetString(m_pHandle, &tRet)))
			return NULL;
		
		return tRet;
	}
	fResult GetTexture(f2dTexture** pOut)
	{
		return FCYERR_NOTIMPL;
	}
	fcyVec4 GetVector()
	{
		return fcyVec4();
	}
	fResult SetString(fcStr Str)
	{
		return FCYERR_NOTIMPL;
	}
	fResult SetTexture(f2dTexture* pTex)
	{
		return FCYERR_NOTIMPL;
	}
	fResult SetVector(const fcyVec4& Vec)
	{
		return FCYERR_NOTIMPL;
	}
protected:
	f2dEffectParamObjImpl(f2dEffect* pParent, D3DXHANDLE pThisHandle, const D3DXPARAMETER_DESC& Desc)
		: f2dEffectParamImpl(pParent, pThisHandle, Desc) { }
};

// 尚未实现
class f2dEffectParamArrImpl :
	public f2dEffectParam
{
public: // 接口实现
	virtual fBool IsRowFirst();
	virtual fuInt GetRowColumnCount();

	f2dEffectParam* GetElement(fuInt Index);
};

// 尚未实现
class f2dEffectParamStructImpl :
	public f2dEffectParam
{
public: // 接口实现
	fuInt GetElementCount();
	f2dEffectParam* GetElement(fcStr Name);
	f2dEffectParam* GetElement(fuInt Index);
};

// 尚未实现
class f2dEffectFunctionImpl
{
public: // 接口实现
	fcStr GetName();
	f2dEffectParam* GetAnnotation();
	fuInt GetAnnotationCount();
};

// 尚未实现
class f2dEffectPassImpl
{
public: // 接口实现
	fcStr GetName();
	f2dEffectParam* GetAnnotation();
	fuInt GetAnnotationCount();
};

// 尚未实现
class f2dEffectTechniqueImpl
{
public: // 接口实现
	fcStr GetName();
	f2dEffectParam* GetAnnotation();
	fuInt GetAnnotationCount();

	fBool IsValidate();
	fuInt GetPassCount();
	f2dEffectPass* GetPass(fuInt Index);
	f2dEffectPass* GetPass(fcStr Name);
};

// 尚未实现
class f2dEffectImpl :
	public fcyRefObjImpl<f2dEffect>
{
	friend class f2dRenderDeviceImpl;
public:
	static f2dEffectParam* CreateEffectParam(f2dEffect* pParent, D3DXHANDLE pThisHandle);
private:
	ID3DXEffect* m_pEffect;

	// 第一层变量列表

public: // 接口实现
	fuInt GetParamCount();
	f2dEffectParam* GetParam(fuInt Index);
	f2dEffectParam* GetParam(fcStr Name);

	fuInt GetTechniqueCount();
	f2dEffectTechnique* GetTechnique(fuInt Index);
	f2dEffectTechnique* GetTechnique(fcStr Name);

	fuInt GetFunctionCount();
	f2dEffectFunction* GetFunction(fuInt Index);
	f2dEffectFunction* GetFunction(fcStr Name);
	
	f2dEffectTechnique* GetCurrentTechnique();
	fResult SetCurrentTechnique(f2dEffectTechnique* pTech);
protected:
	f2dEffectImpl(f2dRenderDevice* pDev, f2dStream* pStream, f2dEffect** pOut, f2dStream** pErrOut);
	~f2dEffectImpl();
};
