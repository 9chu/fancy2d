////////////////////////////////////////////////////////////////////////////////
/// @file  FMMExport.h
/// @brief FFM模型导出插件实现
/// @note  实现SceneExport
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <unordered_map>
#include <string>

#include <Max.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>

#include <IGame\IGame.h>
#include <IGame\IGameFX.h>
#include <IGame\IGameError.h>

#include <fcyParser\fcyXml.h>
#include <fcyMisc\fcyHash.h>
#include <fcyFile\fcyModelMeshFile.h>
#include <fcyMath.h>

#include "resource.h"

struct Vertex
{
	fcyVec3 Position;
	fcyVec3 Normal;
	fcyColor Diffuse;
	fcyVec2 TexCoord;
};

namespace std
{
	template<> 
	struct hash<Vertex>
	{
		inline size_t operator()(const Vertex& v) const
		{
			return fcyHash::SuperFastHash(v);
		}
	};

	template<>
	struct equal_to<Vertex>
		: public binary_function<Vertex, Vertex, bool>
	{	
		// functor for operator==
		bool operator()(const Vertex& _Left, const Vertex& _Right) const
		{	
			if(memcmp(&_Left, &_Right, sizeof(Vertex))!=0)
				return false;
			return true;
		}
	};
};

class CFMMExporter : 
	public SceneExport
{
public:
	/// @brief 返回类ID
	/// @note  使用help/gencid.exe产生
	static Class_ID GetClassID()
	{
		return Class_ID(0x58cc7c23, 0xcd63d85);
	}

	/// @brief 导出过程的进度条函数
	static DWORD WINAPI ProgressFunc(LPVOID arg)
	{
		return 0;
	}
protected:
	// 导出用上下文
	struct OutputContext
	{
		fcyModelMeshFile MeshData;                          ///< @brief 模型数据
		fcyRefPointer<fcyModelVertexLabel> VertexLabel;     ///< @brief 顶点标签
		fcyRefPointer<fcyModelIndexLabel> IndexLabel;       ///< @brief 索引标签

		std::unordered_map<int, std::wstring> MatCache;     ///< @brief 缓存 <材质ID, 材质名>
		std::unordered_map<Vertex, fuInt> IndexCache;       ///< @brief 缓存所有顶点的索引
	};
protected:
	void FillFaceVertex(IGameMesh* pMesh, FaceEx* pFace, Vertex Out[]);

	std::wstring ExportProperty(IGameProperty* pProp);
	void ExportFXProperty(fcyXmlNode& Node, IGameFXProperty* pProp);
	void ExportMaterial(OutputContext* pContext, IGameMaterial* pMat, int ID = -1);
	void ExportSubNode(OutputContext* pContext, IGameNode* pNode);
	void ExportMesh(OutputContext* pContext, IGameNode* pParent, IGameMesh* pObj);
public: // 接口实现
	/// @brief 返回支持的扩展名数量
	int	ExtCount() { return 1; }
	/// @brief 返回扩展名（不带'.'）
	const char* Ext(int n) { return "FFM"; }
	/// @brief 返回扩展名说明
	const char* LongDesc() { return "fancy Model Mesh File"; }
	/// @brief 返回短说明
	const char* ShortDesc() { return "fancy Model"; }
	/// @brief 返回作者
	const char* AuthorName() { return "Chu"; }
	/// @brief 返回版权信息
	const char* CopyrightMessage() { return "Copyright Chu"; }
	/// @brief 返回其他信息1
	const char* OtherMessage1() { return ""; }
	/// @brief 返回其他信息2
	const char* OtherMessage2() { return ""; }
	/// @brief 返回版本
	/// @note  v3.01 = 301
	unsigned int Version() { return 100; }
	/// @brief 显示关于对话框
	/// @note  可选
	void ShowAbout(HWND hWnd) {  }
	/// @brief 后缀支持度检测
	BOOL SupportsOptions(int ext, DWORD options) { return TRUE; }

	/// @brief 开始导出
	int DoExport(const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
public:
	CFMMExporter(void);
	~CFMMExporter(void);
};
