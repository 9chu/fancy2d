////////////////////////////////////////////////////////////////////////////////
/// @file  FMMExportDesc.h
/// @brief FMM模型导出插件描述
/// @note  实现ClassDesc2
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "FMMExporter.h"

// DLL全局量
extern HINSTANCE g_hDllInstance;
extern std::string GetResString(UINT ResID);

////////////////////////////////////////////////////////////////////////////////
/// @brief fancyModelMesh 导出插件描述
/// @note  工厂类
////////////////////////////////////////////////////////////////////////////////
class CFMMExporterDesc : 
	public ClassDesc2
{
public:
	/// @brief 返回实例
	static CFMMExporterDesc* GetInstance()
	{
		static CFMMExporterDesc s_Instance;

		return &s_Instance;
	}
public: // 接口实现
	/// @brief 是否公开
	int IsPublic()
	{
		return TRUE;
	}
	/// @brief 返回子类类名
	LPCSTR ClassName()
	{
		static std::string s_ClassName = GetResString(IDS_CLASS_EXPORT_NAME);

		return s_ClassName.c_str();
	}
	/// @brief 返回父类类型
	SClass_ID SuperClassID()
	{
		return SCENE_EXPORT_CLASS_ID;
	}
	/// @brief 插件分类
	const char* Category()
	{
		static std::string s_Category = GetResString(IDS_CLASS_EXPORT_CATEGORY);

		return s_Category.c_str();
	}
	/// @brief 返回内部名称
	/// @note  脚本可见
	const char* InternalName() { return _T("IFMMExporter"); }
	/// @brief 返回DLL实例
	HINSTANCE HInstance()
	{
		return g_hDllInstance;
	}

	/// @brief 工厂方法
	void* Create(BOOL loading = FALSE)
	{
		return new CFMMExporter();
	}
	/// @brief 返回类ID
	Class_ID ClassID()
	{
		return CFMMExporter::GetClassID();
	}
public:
	CFMMExporterDesc() {}
	~CFMMExporterDesc() {}
};
