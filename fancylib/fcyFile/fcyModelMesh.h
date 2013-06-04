////////////////////////////////////////////////////////////////////////////////
/// @file  fcyModelMesh.h
/// @brief fcy模型网格
/// @note  支持了fancyModelMesh文件的读写
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../fcyIO/fcyStream.h"
#include "../fcyIO/fcyBinaryHelper.h"
#include "../fcyMisc/fcyStringHelper.h"

#include <vector>

/// @addtogroup fancy库文件支持
/// @brief 提供部分格式文件的读写支持
/// @{

/// @brief FFM标签_8字节
#define FFM_LABEL8(str) (*((fuLong*)str))

/// @brief FFM标签_4字节
#define FFM_LABEL4(str) (*((fuInt*)str))

/// @brief FFM顶点元素类型
enum FFM_ELEMENTTYPE 
{
	FFM_ELEMENTTYPE_FLOAT,   // 4字节浮点
	FFM_ELEMENTTYPE_FLOAT2,  // 2*4字节浮点
	FFM_ELEMENTTYPE_FLOAT3,  // 3*4字节浮点
	FFM_ELEMENTTYPE_FLOAT4,  // 4*4字节浮点
	FFM_ELEMENTTYPE_COLOR,   // 4*1字节BGRA颜色分量
	FFM_ELEMENTTYPE_UBYTE4,  // 4*1字节
	FFM_ELEMENTTYPE_SHORT2,  // 2*2有符号短整数
	FFM_ELEMENTTYPE_SHORT4   // 4*2有符号短整数
};

/// @brief FFM顶点元素
struct FFMVertexElement
{
	FFM_ELEMENTTYPE Type; ///< @brief 描述元素的数据格式，定义如下
	fuLong Usage;         ///< @brief 描述元素的用途
	fByte UsageIndex;     ///< @brief 描述元素的用途索引
};

/// @brief fancyModel图元类型定义如下
enum FFM_PRIMTYPE 
{
	FFM_PRIMTYPE_NULL          = 0,  ///< @brief 无效值
	FFM_PRIMTYPE_POINTLIST     = 1,  ///< @brief 点集
	FFM_PRIMTYPE_LINELIST      = 2,  ///< @brief 线集
	FFM_PRIMTYPE_LINESTRIP     = 3,  ///< @brief 线带集
	FFM_PRIMTYPE_TRIANGLELIST  = 4,  ///< @brief 三角形集
	FFM_PRIMTYPE_TRIANGLESTRIP = 5,  ///< @brief 三角形带
	FFM_PRIMTYPE_TRIANGLEFAN   = 6   ///< @brief 三角形扇
};

/// @brief 子集数据
class fcyModelSubset
{
protected:
	std::wstring m_SubsetName;      // 子集名称，在模型数据中不可重复
	std::wstring m_MaterialName;    // 材质名称
	FFM_PRIMTYPE m_PrimitiveType;   // 图元类型
	fuInt m_StartIndex;             // 索引开始下标
	fuInt m_PrimitiveCount;         // 图元数量
public:
	std::wstring& SubsetName() { return m_SubsetName; }
	std::wstring& MaterialName() { return m_MaterialName; }
	FFM_PRIMTYPE& PrimitiveType() { return m_PrimitiveType; }
	fuInt& StartIndex() { return m_StartIndex; }
	fuInt& PrimitiveCount() { return m_PrimitiveCount; }
public:
	fcyModelSubset()
		: m_PrimitiveType(FFM_PRIMTYPE_TRIANGLELIST), m_StartIndex(0), m_PrimitiveCount(0) {}
	fcyModelSubset(const fcyModelSubset& Right)
		: m_SubsetName(Right.m_SubsetName), m_MaterialName(Right.m_MaterialName),
		m_PrimitiveType(Right.m_PrimitiveType), m_StartIndex(Right.m_StartIndex),
		m_PrimitiveCount(Right.m_PrimitiveCount) {}
};

/// @brief 材质数据
class fcyModelMaterial
{
protected:
	std::wstring m_MaterialName;
	std::wstring m_MaterialData;
public:
	std::wstring& MaterialName() { return m_MaterialName; }
	std::wstring& MaterialData() { return m_MaterialData; }
public:
	fcyModelMaterial() {}
	fcyModelMaterial(const fcyModelMaterial& Right)
		: m_MaterialName(Right.m_MaterialName), m_MaterialData(Right.m_MaterialData) {}
};

/// @brief   FFM模型
/// @warning 顶点结构体必须紧凑
template<typename VertType>
class fcyModelMesh
{
public:
	static const fuInt Version = 100;  ///< @brief 版本号1.00
protected:
	std::wstring m_Author;
	std::wstring m_Description;

	std::vector<FFMVertexElement> m_ElementList;
	std::vector<VertType> m_VertList;
	std::vector<fuInt> m_IndexList;
	std::vector<fcyModelSubset> m_SubsetList;
	std::vector<fcyModelMaterial> m_MaterialList;
protected:
	void checkVertexFormat()
	{
		fuInt tSize = 0;

		for(fuInt i = 0; i<m_ElementList.size(); i++)
		{
			switch(m_ElementList[i].Type)
			{
			case FFM_ELEMENTTYPE_FLOAT:
				tSize += 4; 
				break;
			case FFM_ELEMENTTYPE_FLOAT2:
				tSize += 4 * 2; 
				break;
			case FFM_ELEMENTTYPE_FLOAT3:
				tSize += 4 * 3; 
				break;
			case FFM_ELEMENTTYPE_FLOAT4:
				tSize += 4 * 4; 
				break;
			case FFM_ELEMENTTYPE_COLOR:
			case FFM_ELEMENTTYPE_UBYTE4:
				tSize += 4; 
				break;
			case FFM_ELEMENTTYPE_SHORT2:
				tSize += 2 * 2; 
				break;
			case FFM_ELEMENTTYPE_SHORT4:
				tSize += 2 * 4; 
				break;
			default:
				throw fcyException("fcyModelMesh::checkVertexFormat", "Unrecognized element type.");
			}
		}

		if(tSize != sizeof(VertType))
			throw fcyException("fcyModelMesh::checkVertexFormat", "Size not match.");
	}

	std::wstring readString(fcyStream* pStream)
	{
		fcyBinaryReader tReader(pStream);

		fuInt tSize = tReader.ReadUInt32();
		string pBuffer;
		pBuffer.resize(tSize);
		if(tSize)
			tReader.ReadChars(&pBuffer[0], tSize);
		
		return fcyStringHelper::MultiByteToWideChar(pBuffer, CP_UTF8);
	}
	void readVertexLable(fcyStream* pStream)
	{
		fcyBinaryReader tReader(pStream);
		
		fuInt tVertexCount = tReader.ReadUInt32();
		fuInt tVertexElementCount = tReader.ReadUInt32();
		fByte tStreamFlag = tReader.ReadByte();

		if(tStreamFlag != 0)
			throw fcyException("fcyModelMesh::readVertexLable", "Unsupport Stream Type.");
		m_VertList.resize(tVertexCount);
		m_ElementList.resize(tVertexElementCount);

		// 加载顶点声明
		for(fuInt i = 0; i<tVertexElementCount; i++)
		{
			m_ElementList[i].Type = (FFM_ELEMENTTYPE)tReader.ReadUInt32();
			m_ElementList[i].Usage = tReader.ReadUInt64();
			m_ElementList[i].UsageIndex = tReader.ReadByte();
		}

		// 检查
		checkVertexFormat();

		// 加载顶点数据
		for(fuInt i = 0; i<tVertexCount; i++)
		{
			fByte* pData = (fByte*)&m_VertList[i];

			// 直接读取size数据并填充
			tReader.ReadBytes(pData, sizeof(VertType));
		}
	}
	void readIndexLable(fcyStream* pStream)
	{
		fcyBinaryReader tReader(pStream);
		
		fuInt tIndexCount = tReader.ReadUInt32();
		fByte tUseInt32Index = tReader.ReadByte();
		fByte tStreamFlag = tReader.ReadByte();

		if(tStreamFlag != 0)
			throw fcyException("fcyModelMesh::readVertexLable", "Unsupport Stream Type.");
		m_IndexList.resize(tIndexCount);

		// 加载索引数据
		for(fuInt i = 0; i<tIndexCount; i++)
		{
			if(tUseInt32Index)
				m_IndexList[i] = tReader.ReadUInt32();
			else
				m_IndexList[i] = tReader.ReadUInt16();
		}
	}
	void readSubsetLable(fcyStream* pStream)
	{
		fcyBinaryReader tReader(pStream);

		fcyModelSubset tSubset;
		tSubset.SubsetName() = readString(pStream);
		tSubset.MaterialName() = readString(pStream);
		tSubset.PrimitiveType() = (FFM_PRIMTYPE)tReader.ReadUInt32();
		tSubset.StartIndex() = tReader.ReadUInt32();
		tSubset.PrimitiveCount() = tReader.ReadUInt32();

		m_SubsetList.push_back(tSubset);
	}
	void readMaterialLable(fcyStream* pStream)
	{
		fcyBinaryReader tReader(pStream);

		fcyModelMaterial tMaterial;
		tMaterial.MaterialName() = readString(pStream);
		tMaterial.MaterialData() = readString(pStream);

		m_MaterialList.push_back(tMaterial);
	}

	void writeString(fcyStream* pStream, const std::wstring& Str)
	{
		fcyBinaryWriter tWritter(pStream);

		std::string tStr = fcyStringHelper::WideCharToMultiByte(Str, CP_UTF8);
		tWritter.Write((fuInt)tStr.size());
		tWritter.Write(&tStr[0], tStr.size());
	}
	void writeVertexLable(fcyStream* pStream)
	{
		fcyBinaryWriter tWritter(pStream);

		tWritter.Write((fuInt)m_VertList.size());
		tWritter.Write((fuInt)m_ElementList.size());
		tWritter.Write((fByte)0);  // 流标记

		// 写出顶点声明
		for(fuInt i = 0; i<m_ElementList.size(); i++)
		{
			tWritter.Write((fuInt)m_ElementList[i].Type);
			tWritter.Write((fuLong)m_ElementList[i].Usage);
			tWritter.Write((fByte)m_ElementList[i].UsageIndex);
		}

		// 写出顶点
		for(fuInt i = 0; i<m_VertList.size(); i++)
		{
			fByte* pData = (fByte*)&m_VertList[i];

			tWritter.Write(pData, sizeof(VertType));
		}
	}
	void writeIndexLable(fcyStream* pStream)
	{
		fcyBinaryWriter tWritter(pStream);

		fBool tIndex32 = m_VertList.size() > 65535 ? true : false;

		tWritter.Write((fuInt)m_IndexList.size());
		tWritter.Write((fByte)tIndex32);
		tWritter.Write((fByte)0);  // 流标记

		// 写出索引
		for(fuInt i = 0; i<m_IndexList.size(); i++)
		{
			if(tIndex32)
				tWritter.Write((fuInt)m_IndexList[i]);
			else
				tWritter.Write((fuShort)m_IndexList[i]);
		}
	}
	void writeSubsetLable(fcyStream* pStream, fuInt Index)
	{
		fcyBinaryWriter tWritter(pStream);

		fcyModelSubset& tSubset = m_SubsetList[Index];

		writeString(pStream, tSubset.SubsetName());
		writeString(pStream, tSubset.MaterialName());
		tWritter.Write((fuInt)tSubset.PrimitiveType());
        tWritter.Write((fuInt)tSubset.StartIndex());
		tWritter.Write((fuInt)tSubset.PrimitiveCount());
	}
	void writeMaterialLable(fcyStream* pStream, fuInt Index)
	{
		fcyBinaryWriter tWritter(pStream);

		fcyModelMaterial& tMaterial = m_MaterialList[Index];

		writeString(pStream, tMaterial.MaterialName());
		writeString(pStream, tMaterial.MaterialData());
	}

	void saveHelper(fcyStream* pStream, fuLong Lable, void (fcyModelMesh::*lpFunc)(fcyStream*))
	{
		fcyBinaryWriter tWritter(pStream);

		tWritter.Write(Lable);

		fLen tPos = pStream->GetPosition();
		tWritter.Write((fuInt)0);

		(this->*lpFunc)(pStream);

		fLen tEndPos = pStream->GetPosition();
		pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);
		tWritter.Write((fuInt)(tEndPos - tPos - 4));
		pStream->SetPosition(FCYSEEKORIGIN_BEG, tEndPos);
	}
	void saveHelper(fcyStream* pStream, fuLong Lable, fuInt DataIndex, void (fcyModelMesh::*lpFunc)(fcyStream*, fuInt))
	{
		fcyBinaryWriter tWritter(pStream);

		tWritter.Write(Lable);

		fLen tPos = pStream->GetPosition();
		tWritter.Write((fuInt)0);

		(this->*lpFunc)(pStream, DataIndex);

		fLen tEndPos = pStream->GetPosition();
		pStream->SetPosition(FCYSEEKORIGIN_BEG, tPos);
		tWritter.Write((fuInt)(tEndPos - tPos - 4));
		pStream->SetPosition(FCYSEEKORIGIN_BEG, tEndPos);
	}
public:
	std::wstring& Author()
	{
		return m_Author;
	}
	std::wstring& Description()
	{
		return m_Description;
	}

	std::vector<FFMVertexElement>& ElementList() { return m_ElementList; }
	std::vector<VertType>& VertList() { return m_VertList; }
	std::vector<fuInt>& IndexList() { return m_IndexList; }
	std::vector<fcyModelSubset>& SubsetList() { return m_SubsetList; }
	std::vector<fcyModelMaterial>& MaterialList() { return m_MaterialList; }
public:
	void Load(fcyStream* pStream)
	{
		m_Author = m_Description = L"";
		m_ElementList.clear();
		m_VertList.clear();
		m_IndexList.clear();
		m_SubsetList.clear();
		m_MaterialList.clear();
		
		fcyBinaryReader tReader(pStream);

		// 读取文件头
		if(FFM_LABEL4("FFM\0") != tReader.ReadUInt32())
			throw fcyException("fcyModelMesh::Load", "File format error.");
		if(Version != tReader.ReadUInt32())
			throw fcyException("fcyModelMesh::Load", "Version not support.");
		fuInt tBlockCount = tReader.ReadUInt32();
		m_Author = readString(pStream);
		m_Description = readString(pStream);

		// 读取块
		for(fuInt i = 0; i<tBlockCount; i++)
		{
			fuLong tLable = tReader.ReadUInt64(); // 块标签头
			fuInt tLen = tReader.ReadUInt32();    // 块大小

			if(tLable == FFM_LABEL8("VERTEX\0\0"))
				readVertexLable(pStream);
			else if(tLable == FFM_LABEL8("INDEX\0\0\0"))
				readIndexLable(pStream);
			else if(tLable == FFM_LABEL8("SUBSET\0\0"))
				readSubsetLable(pStream);
			else if(tLable == FFM_LABEL8("MATERIAL"))
				readMaterialLable(pStream);
			else
			{
				// 跳过不识别的块
				pStream->SetPosition(FCYSEEKORIGIN_CUR, tLen);
			}
		}
	}
	void Save(fcyStream* pStream)
	{
		// 检查格式
		checkVertexFormat();

		fcyBinaryWriter tWritter(pStream);

		// 写出文件头
		tWritter.Write(FFM_LABEL4("FFM\0"));
		tWritter.Write(Version);

		// 块数量
		tWritter.Write((fuInt)(2 + m_SubsetList.size() + m_MaterialList.size()));
		writeString(pStream, m_Author);
		writeString(pStream, m_Description);

		// 写出块
		saveHelper(pStream, FFM_LABEL8("VERTEX\0\0"), &fcyModelMesh::writeVertexLable);
		saveHelper(pStream, FFM_LABEL8("INDEX\0\0\0"), &fcyModelMesh::writeIndexLable);
		for(fuInt i = 0; i<m_SubsetList.size(); i++)
			saveHelper(pStream, FFM_LABEL8("SUBSET\0\0"), i, &fcyModelMesh::writeSubsetLable);
		for(fuInt i = 0; i<m_MaterialList.size(); i++)
			saveHelper(pStream, FFM_LABEL8("MATERIAL"), i, &fcyModelMesh::writeMaterialLable);
	}
public:
	fcyModelMesh() {}
	fcyModelMesh(const fcyModelMesh& Right)
		: m_Author(Right.m_Author), m_Description(Right.m_Description),
		m_ElementList(Right.m_ElementList), m_VertList(Right.m_VertList),
		m_IndexList(Right.m_IndexList), m_SubsetList(Right.m_SubsetList),
		m_MaterialList(Right.m_MaterialList) {}
	~fcyModelMesh() {}
};

/// @}


/*
	template<typename T>
	void createMesh(fcyModelMesh<T>* pMesh, f2dMeshData** pOut)
	{
		if(!pMesh || !pOut)
			throw fcyException("createMesh", "Invalid Pointer.");
		*pOut = NULL;

		f2dVertexElement* tElement = new f2dVertexElement[pMesh->ElementList().size()];
		for(fuInt i = 0; i<pMesh->ElementList().size(); i++)
		{
			tElement[i].Type = (F2DVDTYPE)pMesh->ElementList()[i].Type;
			tElement[i].UsageIndex = pMesh->ElementList()[i].UsageIndex;

			fuLong tUsage = pMesh->ElementList()[i].Usage;

			if(tUsage == FFM_LABEL8("POSITION"))
				tElement[i].Usage = F2DVDUSAGE_POSITION;
			else if(tUsage == FFM_LABEL8("WEIGHT\0\0"))
				tElement[i].Usage = F2DVDUSAGE_BLENDWEIGHT;
			else if(tUsage == FFM_LABEL8("INDICES\0"))
				tElement[i].Usage = F2DVDUSAGE_BLENDINDICES;
			else if(tUsage == FFM_LABEL8("NORMAL\0\0"))
				tElement[i].Usage = F2DVDUSAGE_NORMAL;
			else if(tUsage == FFM_LABEL8("PSIZE\0\0\0"))
				tElement[i].Usage = F2DVDUSAGE_PSIZE;
			else if(tUsage == FFM_LABEL8("TEXCOORD"))
				tElement[i].Usage = F2DVDUSAGE_TEXCOORD;
			else if(tUsage == FFM_LABEL8("TANGENT\0"))
				tElement[i].Usage = F2DVDUSAGE_TANGENT;
			else if(tUsage == FFM_LABEL8("BINORMAL"))
				tElement[i].Usage = F2DVDUSAGE_BINORMAL;
			else if(tUsage == FFM_LABEL8("TESSFACT"))
				tElement[i].Usage = F2DVDUSAGE_TESSFACTOR;
			else if(tUsage == FFM_LABEL8("POST\0\0\0\0"))
				tElement[i].Usage = F2DVDUSAGE_POSITIONT;
			else if(tUsage == FFM_LABEL8("COLOR\0\0\0"))
				tElement[i].Usage = F2DVDUSAGE_COLOR;
			else if(tUsage == FFM_LABEL8("FOG\0\0\0\0\0"))
				tElement[i].Usage = F2DVDUSAGE_FOG;
			else if(tUsage == FFM_LABEL8("DEPTH\0\0\0"))
				tElement[i].Usage = F2DVDUSAGE_DEPTH;
			else if(tUsage == FFM_LABEL8("SAMPLE\0\0"))
				tElement[i].Usage = F2DVDUSAGE_SAMPLE;
			else 
			{
				FCYSAFEDELARR(tElement);
				throw fcyException("createMesh", "Unknwon Vertex Type.");
			}
		}

		fBool tInt32Index = pMesh->VertList().size()>65535 ? true:false;
		fResult tFR = m_pDev->CreateMeshData(
				tElement, 
				pMesh->ElementList().size(), 
				pMesh->VertList().size(), 
				pMesh->IndexList().size(),
				tInt32Index,
				pOut);
		FCYSAFEDELARR(tElement);
		if(FCYFAILED(tFR))
		{
			throw fcyException("createMesh", "CreateMeshData Failed.");
		}

		// 填充数据
		fByte* pVert = NULL;
		(*pOut)->LockVertexData(0, pMesh->VertList().size(), (void**)&pVert);
		for(fuInt i = 0; i<pMesh->VertList().size(); i++)
		{
			memcpy(pVert, &pMesh->VertList()[i], sizeof(T));
			pVert += sizeof(T);
		}
		(*pOut)->UnlockVertexData();

		fByte* pIndex = NULL;
		(*pOut)->LockIndexData(0, pMesh->IndexList().size(), (void**)&pIndex);
		for(fuInt i = 0; i<pMesh->IndexList().size(); i++)
		{
			if(tInt32Index)
			{
				*((fuInt*)pIndex) = pMesh->IndexList()[i];
				pIndex += sizeof(fuInt);
			}
			else
			{
				*((fuShort*)pIndex) = pMesh->IndexList()[i];
				pIndex += sizeof(fuShort);
			}
		}
		(*pOut)->UnlockIndexData();

		// 拷贝子集
		for(fuInt i = 0; i<pMesh->SubsetList().size(); i++)
		{
			(*pOut)->SetSubset(i, 
				(F2DPRIMITIVETYPE)pMesh->SubsetList()[i].PrimitiveType(),
				pMesh->SubsetList()[i].StartIndex(),
				pMesh->SubsetList()[i].PrimitiveCount()
				);
		}
	}
	*/
