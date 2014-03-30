#include "FMMExporter.h"

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
}

CFMMExporter::~CFMMExporter(void)
{
}

int CFMMExporter::DoExport(const MCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts, DWORD options)
{
	// 打开文件
	fcyFileStream* tFile;
	try
	{
		tFile = new fcyFileStream(fcyStringHelper::MultiByteToWideChar(name).c_str(), true);
	}
	catch(...)
	{
		MessageBoxW(0, L"创建文件失败。", L"FMMExporter - 错误", 0);
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

	// 准备模型数据
	OutputContext tContext;
	{
		tContext.VertexLabel = new fcyModelVertexLabel();
		tContext.VertexLabel->Release();
		tContext.MeshData.GetLabelList().push_back(*tContext.VertexLabel);

		tContext.IndexLabel = new fcyModelIndexLabel();
		tContext.IndexLabel->Release();
		tContext.MeshData.GetLabelList().push_back(*tContext.IndexLabel);

		// 注册顶点信息
		tContext.VertexLabel->ResizeElement(4);
		tContext.VertexLabel->GetVertexElement(0).Type = fcyModelVertexLabel::ELEMENTTYPE_FLOAT3;
		tContext.VertexLabel->GetVertexElement(0).Usage = FMM_MAKE_LABELNAME8("POSITION");
		tContext.VertexLabel->GetVertexElement(0).UsageIndex = 0;
		tContext.VertexLabel->GetVertexElement(1).Type = fcyModelVertexLabel::ELEMENTTYPE_FLOAT3;
		tContext.VertexLabel->GetVertexElement(1).Usage = FMM_MAKE_LABELNAME8("NORMAL\0\0");
		tContext.VertexLabel->GetVertexElement(1).UsageIndex = 0;
		tContext.VertexLabel->GetVertexElement(2).Type = fcyModelVertexLabel::ELEMENTTYPE_COLOR;
		tContext.VertexLabel->GetVertexElement(2).Usage = FMM_MAKE_LABELNAME8("COLOR\0\0\0");
		tContext.VertexLabel->GetVertexElement(2).UsageIndex = 0;
		tContext.VertexLabel->GetVertexElement(3).Type = fcyModelVertexLabel::ELEMENTTYPE_FLOAT2;
		tContext.VertexLabel->GetVertexElement(3).Usage = FMM_MAKE_LABELNAME8("TEXCOORD");
		tContext.VertexLabel->GetVertexElement(3).UsageIndex = 0;
	}

	// 遍历所有材质
	int tMatCount = pIGame->GetRootMaterialCount();
	for(int i = 0; i<tMatCount; i++)
	{
		IGameMaterial* pMat = pIGame->GetRootMaterial(i);

		ExportMaterial(&tContext, pMat);
	}

	// 遍历所有节点
	int cTopNodeCount = pIGame->GetTopLevelNodeCount();
	for(int i = 0; i<cTopNodeCount; i++)
	{
		IGameNode * pGameNode = pIGame->GetTopLevelNode(i);

		if(pGameNode->IsTarget())
			continue;

		// 导出节点
		ExportSubNode(&tContext, pGameNode);
	}

	// 保存数据
	tContext.MeshData.Save(tFile);
	FCYSAFEKILL(tFile);

	pCore->ProgressEnd();
	
	// 释放IGame
	pIGame->ReleaseIGame();

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
	Out[0].TexCoord.x = tTex.x; Out[0].TexCoord.y = 1.f + tTex.y;
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
	Out[1].TexCoord.x = tTex.x; Out[1].TexCoord.y = 1.f + tTex.y;
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
	Out[2].TexCoord.x = tTex.x; Out[2].TexCoord.y = 1.f + tTex.y;
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

void CFMMExporter::ExportFXProperty(fcyXmlElement& Node, IGameFXProperty* pProp)
{
	Node.SetAttribute(L"Name", fcyStringHelper::MultiByteToWideChar(pProp->GetPropertyName()));

	// ... 如何导出参数？
}

void CFMMExporter::ExportMaterial(OutputContext* pContext, IGameMaterial* pMat, int ID)
{
	// 获得名称
	wstring tName = fcyStringHelper::MultiByteToWideChar(pMat->GetMaterialName());
	
	// 注册材质
	if(ID != -1)
		pContext->MatCache[ID] = tName;

	// 材质容器
	if(pMat->IsMultiType())
	{
		int tCount = pMat->GetSubMaterialCount();
		for(int i = 0; i<tCount; i++)
		{
			ExportMaterial(pContext, pMat->GetSubMaterial(i), pMat->GetMaterialID(i));
		}
	}
	else
	{
		// 创建材质XML数据
		fcyXmlDocument tXmlData;

		// 创建根
		fcyXmlElement* tRoot = tXmlData.CreateElement(L"fancyModelMaterial");
		
		// 导出标准材质数据
		{
			fcyXmlElement* tNode = tXmlData.CreateElement(L"Diffuse");
			tNode->SetContent(ExportProperty(pMat->GetDiffuseData()));
			tRoot->AppendNode(tNode);

			tNode = tXmlData.CreateElement(L"Ambient");
			tNode->SetContent(ExportProperty(pMat->GetAmbientData()));
			tRoot->AppendNode(tNode);

			tNode = tXmlData.CreateElement(L"Specular");
			tNode->SetContent(ExportProperty(pMat->GetSpecularData()));
			tRoot->AppendNode(tNode);

			tNode = tXmlData.CreateElement(L"Glossiness");
			tNode->SetContent(ExportProperty(pMat->GetGlossinessData()));
			tRoot->AppendNode(tNode);

			tNode = tXmlData.CreateElement(L"Opacity");
			tNode->SetContent(ExportProperty(pMat->GetOpacityData()));
			tRoot->AppendNode(tNode);

			tNode = tXmlData.CreateElement(L"SpecularLevel");
			tNode->SetContent(ExportProperty(pMat->GetSpecularLevelData()));
			tRoot->AppendNode(tNode);
		}

		// 导出纹理组
		{
			int tCount = pMat->GetNumberOfTextureMaps();
			for(int i = 0; i<tCount; i++)
			{
				IGameTextureMap* pTextureMap = pMat->GetIGameTextureMap(i);

				fcyXmlElement* tNode = tXmlData.CreateElement(L"Texture");
				tNode->SetAttribute(L"Name", fcyStringHelper::MultiByteToWideChar(pTextureMap->GetTextureName()));
				tNode->SetAttribute(L"Slot", fcyStringHelper::ToWideStr(pTextureMap->GetStdMapSlot()));
				
				if(pTextureMap->IsEntitySupported())
				{
					// 位图
					tNode->SetAttribute(L"ClipU", ExportProperty(pTextureMap->GetClipUData()));
					tNode->SetAttribute(L"ClipV", ExportProperty(pTextureMap->GetClipVData()));
					tNode->SetAttribute(L"ClipW", ExportProperty(pTextureMap->GetClipWData()));
					tNode->SetAttribute(L"ClipH", ExportProperty(pTextureMap->GetClipHData()));

					tNode->SetAttribute(L"Filename", fcyStringHelper::MultiByteToWideChar(pTextureMap->GetBitmapFileName()));
				}

				tRoot->AppendNode(tNode);
			}
		}

		// 导出FX数据
		/*
		IGameFX * pFXData = pMat->GetIGameFX();
		if(pFXData)
		{
			fcyXmlElement tFXNode;
			tFXNode.SetName(L"FX");
			tFXNode.SetAttribute(L"Filename", fcyStringHelper::MultiByteToWideChar((fcStr)pFXData->GetEffectFile().GetFileName()).c_str());

			// 导出参数
			for(int i=0; i<pFXData->GetNumberOfProperties(); i++)
			{
				IGameFXProperty* pFXProp = pFXData->GetIGameFXProperty(i);
				
				fcyXmlElement tPropNode;
				tPropNode.SetName(L"Param");
				ExportFXProperty(tPropNode, pFXProp);

				tFXNode.AppendNode(tPropNode);
			}
			
			tRoot.AppendNode(tFXNode);
		}
		*/

		// 设置根
		tXmlData.SetRootElement(tRoot);

		// 创建材质信息
		fcyRefPointer<fcyModelMaterialLabel> pMat = new fcyModelMaterialLabel();
		pMat->Release();

		wstring tXMLStr;
		tXmlData.Save(tXMLStr);

		pMat->SetMaterialName(tName);
		pMat->SetMaterialXMLData(tXMLStr);

		pContext->MeshData.GetLabelList().push_back(*pMat);
	}
}

void CFMMExporter::ExportSubNode(OutputContext* pContext, IGameNode* pNode)
{
	// 处理当前节点
	IGameObject * pObj = pNode->GetIGameObject();

	if(pObj)
	{
		switch(pObj->GetIGameType())
		{
		case IGameObject::IGAME_MESH:
			ExportMesh(pContext, pNode, (IGameMesh*)pObj);
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

		ExportSubNode(pContext, pGameNode);
	}
}

void CFMMExporter::ExportMesh(OutputContext* pContext, IGameNode* pParent, IGameMesh* pObj)
{
	// 子集名称
	string tName = pParent->GetName();

	// 初始化
	pObj->SetCreateOptimizedNormalList();
	pObj->InitializeData();

	// 缓冲
	unordered_map<int, vector<fuInt> > tSubsetCache; // 缓存所有子集
	unordered_map<int, string> tSubsetNameCache;     // 子集名称缓存
	
	unordered_map<Vertex, fuInt>& tIndexCache = pContext->IndexCache;

	// 收集每个面的顶点信息
	fInt tFaceCount = pObj->GetNumberOfFaces();
	for(int i = 0; i<tFaceCount; i++)
	{
		FaceEx* pFace = pObj->GetFace(i);
		int tMat = pObj->GetFaceMaterialID(i);

		// 创建面的顶点
		Vertex tVert[3];
		FillFaceVertex(pObj, pFace, tVert);

		// 从缓存查询
		unordered_map<Vertex, fuInt>::iterator v0 = tIndexCache.find(tVert[0]);
		if(v0 == tIndexCache.end())
		{
			int tIndex = tIndexCache.size();
			tIndexCache[tVert[0]] = tIndex;
			v0 = tIndexCache.find(tVert[0]);

			pContext->VertexLabel->PushVertex(&tVert[0]);
		}
		unordered_map<Vertex, fuInt>::iterator v1 = tIndexCache.find(tVert[1]);
		if(v1 == tIndexCache.end())
		{
			int tIndex = tIndexCache.size();
			tIndexCache[tVert[1]] = tIndex;
			v1 = tIndexCache.find(tVert[1]);
			
			pContext->VertexLabel->PushVertex(&tVert[1]);
		}
		unordered_map<Vertex, fuInt>::iterator v2 = tIndexCache.find(tVert[2]);
		if(v2 == tIndexCache.end())
		{
			int tIndex = tIndexCache.size();
			tIndexCache[tVert[2]] = tIndex;
			v2 = tIndexCache.find(tVert[2]);
			
			pContext->VertexLabel->PushVertex(&tVert[2]);
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
		const wstring& tMatName = pContext->MatCache[i->first];
		
		// 创建子集
		fcyRefPointer<fcyModelSubsetLabel> pSubsetLabel = new fcyModelSubsetLabel();
		pSubsetLabel->Release();
		
		pSubsetLabel->SetSubsetName(fcyStringHelper::MultiByteToWideChar(tSubsetNameCache[i->first]));
		pSubsetLabel->SetMaterialName(tMatName);
		pSubsetLabel->SetStartIndex(pContext->IndexLabel->GetSize());
		pSubsetLabel->SetPrimitiveType(fcyModelSubsetLabel::PRIMTYPE_TRIANGLELIST);
		pSubsetLabel->SetPrimitiveCount(i->second.size() / 3);

		// 写出数据
		for(fuInt j = 0; j<i->second.size(); j++)
		{
			pContext->IndexLabel->Push(i->second[j]);
		}

		pContext->MeshData.GetLabelList().push_back(*pSubsetLabel);

		i++;
	}
}
