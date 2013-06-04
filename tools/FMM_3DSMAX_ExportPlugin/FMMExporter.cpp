#include "FMMExporter.h"

#include <fcyParser\fcyXml.h>
#include <fcyMisc\fcyStringHelper.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

/// @brief 错误处理函数
class CErrorProc : 
	public IGameErrorCallBack
{
public:
	void ErrorProc(IGameError ErrCode)
	{
		DebugPrint("[ CFMMExporter ] ErrorCode = %d; ErrorText = %s\n", ErrCode, GetLastIGameErrorText());
	}
};

////////////////////////////////////////////////////////////////////////////////

CFMMExporter::CFMMExporter(void)
{
	// 注册顶点信息
	m_MeshData.ElementList().resize(4);
	m_MeshData.ElementList()[0].Type = FFM_ELEMENTTYPE_FLOAT3;
	m_MeshData.ElementList()[0].Usage = FFM_LABEL8("POSITION");
	m_MeshData.ElementList()[0].UsageIndex = 0;
	m_MeshData.ElementList()[1].Type = FFM_ELEMENTTYPE_FLOAT3;
	m_MeshData.ElementList()[1].Usage = FFM_LABEL8("NORMAL\0\0");
	m_MeshData.ElementList()[1].UsageIndex = 0;
	m_MeshData.ElementList()[2].Type = FFM_ELEMENTTYPE_COLOR;
	m_MeshData.ElementList()[2].Usage = FFM_LABEL8("COLOR\0\0\0");
	m_MeshData.ElementList()[2].UsageIndex = 0;
	m_MeshData.ElementList()[3].Type = FFM_ELEMENTTYPE_FLOAT2;
	m_MeshData.ElementList()[3].Usage = FFM_LABEL8("TEXCOORD");
	m_MeshData.ElementList()[3].UsageIndex = 0;
}

CFMMExporter::~CFMMExporter(void)
{
}

int CFMMExporter::DoExport(const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	// 清空模型数据
	m_MeshData.VertList().clear();
	m_MeshData.IndexList().clear();
	m_MeshData.SubsetList().clear();
	m_MeshData.MaterialList().clear();
	m_MatCache.clear();
	m_IndexCache.clear();

	// 打开文件
	fcyFileStream* tFile;
	try
	{
		tFile = new fcyFileStream(fcyStringHelper::MultiByteToWideChar(name).c_str(), true);
	}
	catch(...)
	{
		return FALSE;
	}
	tFile->SetLength(0);
	tFile->SetPosition(FCYSEEKORIGIN_BEG, 0);

	bool bExportSelectedObj = (options & SCENE_EXPORT_SELECTED) ? true : false;

	// 错误处理函数
	CErrorProc tErrorProc;
	SetErrorCallBack(&tErrorProc);

	// 核心接口
	Interface * pCore = GetCOREInterface();

	// 版本信息
	DebugPrint("[ CFMMExporter ] IGame Supported 3DS Version = %.2f; IGame Version = %.2f\n" , GetSupported3DSVersion(), GetIGameVersion());
	
	// 显示导出选项
	if(suppressPrompts)
	{
		// ...
	}

	// 获得IGame接口并配置
	IGameScene * pIGame = GetIGameInterface();
	IGameConversionManager * pIGameConvManager = GetConversionManager();
	pIGame->InitialiseIGame(bExportSelectedObj);  // 初始化
	pIGameConvManager->SetCoordSystem(IGameConversionManager::IGAME_D3D);  // 设置坐标系统

	// 开始导出过程
	pCore->ProgressStart("Exporting fancy Model Mesh...", TRUE, ProgressFunc, NULL);

	// 遍历所有材质
	int tMatCount = pIGame->GetRootMaterialCount();
	for(int i = 0; i<tMatCount; i++)
	{
		IGameMaterial* pMat = pIGame->GetRootMaterial(i);

		ExportMaterial(pMat);
	}

	// 遍历所有节点
	int cTopNodeCount = pIGame->GetTopLevelNodeCount();
	for(int i = 0; i<cTopNodeCount; i++)
	{
		IGameNode * pGameNode = pIGame->GetTopLevelNode(i);

		if(pGameNode->IsTarget())
			continue;

		// 导出节点
		ExportSubNode(pGameNode);
	}

	// 保存数据
	m_MeshData.Save(tFile);
	FCYSAFEKILL(tFile);

	pCore->ProgressEnd();
	
	// 释放IGame
	pIGame->ReleaseIGame();

	// 清空模型数据
	m_MeshData.VertList().clear();
	m_MeshData.IndexList().clear();
	m_MeshData.SubsetList().clear();
	m_MeshData.MaterialList().clear();
	m_MatCache.clear();
	m_IndexCache.clear();

	return TRUE;
}

void CFMMExporter::FillFaceVertex(IGameMesh* pMesh, FaceEx* pFace, Vertex Out[])
{
	Point3 tPos;
	Point3 tNormal;
	float tAlpha;
	Point3 tColor;
	Point2 tTex;

	pMesh->GetVertex(pFace->vert[0], tPos);
	pMesh->GetNormal(pFace->norm[0], tNormal);
	pMesh->GetTexVertex(pFace->texCoord[0], tTex);
	if(!pMesh->GetAlphaVertex(pFace->alpha[0], tAlpha))
		tAlpha = 1.f;
	if(!pMesh->GetColorVertex(pFace->color[0], tColor))
		tColor.x = tColor.y = tColor.z = 0;

	Out[0].Position.x = tPos.x; Out[0].Position.y = tPos.y; Out[0].Position.z = tPos.z;
	Out[0].Normal.x = tNormal.x; Out[0].Normal.y = tNormal.y; Out[0].Normal.z = tNormal.z;
	Out[0].TexCoord.x = tTex.x; Out[0].TexCoord.y = tTex.y;
	Out[0].Diffuse = fcyColor(tAlpha, tColor.x, tColor.y, tColor.z);

	pMesh->GetVertex(pFace->vert[1], tPos);
	pMesh->GetNormal(pFace->norm[1], tNormal);
	pMesh->GetTexVertex(pFace->texCoord[1], tTex);
	if(!pMesh->GetAlphaVertex(pFace->alpha[1], tAlpha))
		tAlpha = 1.f;
	if(!pMesh->GetColorVertex(pFace->color[1], tColor))
		tColor.x = tColor.y = tColor.z = 0;

	Out[1].Position.x = tPos.x; Out[1].Position.y = tPos.y; Out[1].Position.z = tPos.z;
	Out[1].Normal.x = tNormal.x; Out[1].Normal.y = tNormal.y; Out[1].Normal.z = tNormal.z;
	Out[1].TexCoord.x = tTex.x; Out[1].TexCoord.y = tTex.y;
	Out[1].Diffuse = fcyColor(tAlpha, tColor.x, tColor.y, tColor.z);

	pMesh->GetVertex(pFace->vert[2], tPos);
	pMesh->GetNormal(pFace->norm[2], tNormal);
	pMesh->GetTexVertex(pFace->texCoord[2], tTex);
	if(!pMesh->GetAlphaVertex(pFace->alpha[2], tAlpha))
		tAlpha = 1.f;
	if(!pMesh->GetColorVertex(pFace->color[2], tColor))
		tColor.x = tColor.y = tColor.z = 0;

	Out[2].Position.x = tPos.x; Out[2].Position.y = tPos.y; Out[2].Position.z = tPos.z;
	Out[2].Normal.x = tNormal.x; Out[2].Normal.y = tNormal.y; Out[2].Normal.z = tNormal.z;
	Out[2].TexCoord.x = tTex.x; Out[2].TexCoord.y = tTex.y;
	Out[2].Diffuse = fcyColor(tAlpha, tColor.x, tColor.y, tColor.z);
}

std::wstring CFMMExporter::ExportProperty(IGameProperty* pProp)
{
	wstring tRet;
	wchar_t tTextBuffer[256];

	if(pProp)
	{
		switch(pProp->GetType())
		{
		case IGAME_POINT3_PROP:
			{
				Point3 p; 
				pProp->GetPropertyValue(p);

				swprintf_s(tTextBuffer, L"%f,%f,%f", p.x, p.y, p.z);
				tRet = tTextBuffer;
			}
			break;
		case IGAME_FLOAT_PROP:
			{
				float p; 
				pProp->GetPropertyValue(p);

				swprintf_s(tTextBuffer, L"%f", p);
				tRet = tTextBuffer;
			}
			break;
		case IGAME_STRING_PROP:
			{
				const char* pStr = NULL;
				pProp->GetPropertyValue(pStr);

				tRet = fcyStringHelper::MultiByteToWideChar(pStr);
			}
			break;
		}
	}

	return tRet;
}

void CFMMExporter::ExportMaterial(IGameMaterial* pMat, int ID)
{
	// 获得名称
	wstring tName = fcyStringHelper::MultiByteToWideChar(pMat->GetMaterialName());
	
	// 注册材质
	if(ID != -1)
	{
		m_MatCache[ID] = tName;
	}

	// 材质容器
	if(pMat->IsMultiType())
	{
		int tCount = pMat->GetSubMaterialCount();
		for(int i = 0; i<tCount; i++)
		{
			ExportMaterial(pMat->GetSubMaterial(i), pMat->GetMaterialID(i));
		}
	}
	else
	{
		// 子材质，最终加入材质节点
		fcyModelMaterial tMatData;

		// 创建材质XML数据
		fcyXml tXmlData;

		// 创建根
		fcyXmlNode tRoot;
		tRoot.SetName(L"fancyModelMaterial");

		// 导出标准材质数据
		{
			fcyXmlNode tNode;
			tNode.SetName(L"Diffuse");
			tNode.SetContent(ExportProperty(pMat->GetDiffuseData()).c_str());
			tRoot.AppendNode(tNode);

			tNode.SetName(L"Ambient");
			tNode.SetContent(ExportProperty(pMat->GetAmbientData()).c_str());
			tRoot.AppendNode(tNode);

			tNode.SetName(L"Specular");
			tNode.SetContent(ExportProperty(pMat->GetSpecularData()).c_str());
			tRoot.AppendNode(tNode);

			tNode.SetName(L"Glossiness");
			tNode.SetContent(ExportProperty(pMat->GetGlossinessData()).c_str());
			tRoot.AppendNode(tNode);

			tNode.SetName(L"Opacity");
			tNode.SetContent(ExportProperty(pMat->GetOpacityData()).c_str());
			tRoot.AppendNode(tNode);

			tNode.SetName(L"SpecularLevel");
			tNode.SetContent(ExportProperty(pMat->GetSpecularLevelData()).c_str());
			tRoot.AppendNode(tNode);
		}

		// 设置根
		tXmlData.SetRoot(tRoot);

		// 创建材质信息
		tMatData.MaterialName() = tName;
		tXmlData.WriteToStr(tMatData.MaterialData());

		m_MeshData.MaterialList().push_back(tMatData);
	}
}

void CFMMExporter::ExportSubNode(IGameNode* pNode)
{
	// 处理当前节点
	IGameObject * pObj = pNode->GetIGameObject();

	if(pObj)
	{
		switch(pObj->GetIGameType())
		{
		case IGameObject::IGAME_MESH:
			ExportMesh(pNode, (IGameMesh*)pObj);
			break;
		}
	}

	pNode->ReleaseIGameObject();

	// 处理子节点
	for(int i = 0; i<pNode->GetChildCount(); i++)
	{
		IGameNode * pGameNode = pNode->GetNodeChild(i);

		if(pGameNode->IsTarget())
			continue;

		ExportSubNode(pGameNode);
	}
}

void CFMMExporter::ExportMesh(IGameNode* pParent, IGameMesh* pObj)
{
	// 子集名称
	string tName = pParent->GetName();

	// 初始化
	pObj->SetCreateOptimizedNormalList();
	pObj->InitializeData();

	// 缓冲
	unordered_map<int, vector<fuInt> > tSubsetCache; // 缓存所有子集
	unordered_map<int, string> tSubsetNameCache;     // 子集名称缓存

	fInt tFaceCount = pObj->GetNumberOfFaces();
	for(int i = 0; i<tFaceCount; i++)
	{
		FaceEx* pFace = pObj->GetFace(i);
		int tMat = pObj->GetFaceMaterialID(i);

		// 创建面的顶点
		Vertex tVert[3];
		FillFaceVertex(pObj, pFace, tVert);

		// 从缓存查询
		unordered_map<Vertex, fuInt>::iterator v0 = m_IndexCache.find(tVert[0]);
		if(v0 == m_IndexCache.end())
		{
			int tIndex = m_IndexCache.size();
			m_IndexCache[tVert[0]] = tIndex;
			v0 = m_IndexCache.find(tVert[0]);

			m_MeshData.VertList().push_back(tVert[0]);
		}
		unordered_map<Vertex, fuInt>::iterator v1 = m_IndexCache.find(tVert[1]);
		if(v1 == m_IndexCache.end())
		{
			int tIndex = m_IndexCache.size();
			m_IndexCache[tVert[1]] = tIndex;
			v1 = m_IndexCache.find(tVert[1]);
			
			m_MeshData.VertList().push_back(tVert[1]);
		}
		unordered_map<Vertex, fuInt>::iterator v2 = m_IndexCache.find(tVert[2]);
		if(v2 == m_IndexCache.end())
		{
			int tIndex = m_IndexCache.size();
			m_IndexCache[tVert[2]] = tIndex;
			v2 = m_IndexCache.find(tVert[2]);
			
			m_MeshData.VertList().push_back(tVert[2]);
		}

		// 加入索引
		if(tSubsetCache.find(tMat) == tSubsetCache.end())
		{
			tSubsetNameCache[tMat] = tName + "@" + fcyStringHelper::ToStr((fInt)tSubsetCache.size());
		}
		tSubsetCache[tMat].push_back(v0->second);
		tSubsetCache[tMat].push_back(v1->second);
		tSubsetCache[tMat].push_back(v2->second);
	}

	// --- 把所有数据写入Mesh ---
	unordered_map<int, vector<fuInt> >::iterator i = tSubsetCache.begin();
	while(i != tSubsetCache.end())
	{
		// 查询子集的材质
		const wstring& tMatName = m_MatCache[i->first];
		
		fcyModelSubset tSubset;
		tSubset.MaterialName() = tMatName;
		tSubset.SubsetName() = fcyStringHelper::MultiByteToWideChar(tSubsetNameCache[i->first]);
		tSubset.StartIndex() = m_MeshData.IndexList().size();
		tSubset.PrimitiveType() = FFM_PRIMTYPE_TRIANGLELIST;
		tSubset.PrimitiveCount() = i->second.size() / 3;

		// 写出数据
		for(int j = 0; j<i->second.size(); j++)
		{
			m_MeshData.IndexList().push_back(i->second[j]);
		}

		m_MeshData.SubsetList().push_back(tSubset);

		i++;
	}
}
