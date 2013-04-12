#include "f2dSoundDecoder.h"

#include "fcyException.h"

#include "fcyOS/fcyDebug.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////
const char* f2dWaveDecoder::tagRIFF = "RIFF";
const char* f2dWaveDecoder::tagFMT = "fmt ";
const char* f2dWaveDecoder::tagFACT = "fact";
const char* f2dWaveDecoder::tagDATA = "data";

f2dWaveDecoder::CWaveChunk::CWaveChunk(fChar* ID, fuInt Size, fcyBinaryReader* pReader)
	: IChunk(ID, Size)
{
	pReader->ReadChars(m_Type, 4);
}

f2dWaveDecoder::CWaveChunk::~CWaveChunk()
{}

f2dWaveDecoder::CFormatChunk::CFormatChunk(fChar* ID, fuInt Size, fcyBinaryReader* pReader)
	: IChunk(ID, Size)
{
	m_FormatTag = pReader->ReadUInt16();
	m_Channels = pReader->ReadUInt16();
	m_SamplesPerSec = pReader->ReadUInt32();
	m_AvgBytesPerSec = pReader->ReadUInt32();
	m_BlockAlign = pReader->ReadUInt16();
	m_BitsPerSample = pReader->ReadUInt16();
	if(Size > 16)
		m_Reserved = pReader->ReadUInt16();
	else
		m_Reserved = 0;
}

f2dWaveDecoder::CFormatChunk::~CFormatChunk()
{}

f2dWaveDecoder::CFactChunk::CFactChunk(fChar* ID, fuInt Size, fcyBinaryReader* pReader)
	: IChunk(ID, Size)
{
	m_Data = pReader->ReadUInt32();
}

f2dWaveDecoder::CFactChunk::~CFactChunk()
{}

f2dWaveDecoder::CDataChunk::CDataChunk(fChar* ID, fuInt Size, fcyBinaryReader* pReader)
	: IChunk(ID, Size)
{
	m_BasePointer = pReader->GetBaseStream()->GetPosition();
	// 跳过Size
	pReader->GetBaseStream()->SetPosition(FCYSEEKORIGIN_CUR, Size);
}

f2dWaveDecoder::CDataChunk::~CDataChunk()
{}

f2dWaveDecoder::f2dWaveDecoder(f2dStream* pStream)
	: m_pStream(pStream), m_cPointer(0)
{
	if(!m_pStream)
		throw fcyException("f2dWaveDecoder::f2dWaveDecoder", "Invalid Pointer.");
	m_pStream->AddRef();

	// 锁定
	m_pStream->Lock();
	m_pStream->SetPosition(FCYSEEKORIGIN_BEG, 0);

	// 读取所有RIFF块
	try
	{
		fcyBinaryReader tReader(pStream);
		while(pStream->GetPosition() < pStream->GetLength())
		{
			char tID[4];
			fuInt tSize = 0;
			tReader.ReadChars(tID, 4);
			tSize = tReader.ReadUInt32();

			IChunk* pChunk = NULL;
			if(strncmp(tID, tagRIFF, 4) == 0)
				pChunk = new CWaveChunk(tID, tSize, &tReader);
			else if(strncmp(tID, tagFMT, 4) == 0)
				pChunk = new CFormatChunk(tID, tSize, &tReader);
			else if(strncmp(tID, tagFACT, 4) == 0)
				pChunk = new CFactChunk(tID, tSize, &tReader);
			else if(strncmp(tID, tagDATA, 4) == 0)
				pChunk = new CDataChunk(tID, tSize, &tReader);
			else
			{
				// 跳过不支持的chunk
				pStream->SetPosition(FCYSEEKORIGIN_CUR, tSize);
			}

			if(pChunk)
			{
				m_ChunkList.push_back(pChunk);
				m_Chunk[pChunk->m_ID] = pChunk;
			}
		}
	}
	catch(const fcyException& e) // 处理读取异常
	{
		FCYDEBUGEXCPT(e);

		m_pStream->Unlock();
		clear();                // 清除无用数据
		FCYSAFEKILL(m_pStream); // 释放引用
		throw;
	}

	// 解锁
	m_pStream->Unlock();

	// 检查区块完整性
	if(m_Chunk.find(tagRIFF) == m_Chunk.end() || m_Chunk.find(tagFMT) == m_Chunk.end() || m_Chunk.find(tagDATA) == m_Chunk.end())
	{
		clear();
		FCYSAFEKILL(m_pStream);
		throw fcyException("f2dWaveDecoder::f2dWaveDecoder", "Chunk Error.");
	};
}

f2dWaveDecoder::~f2dWaveDecoder()
{
	clear();
	FCYSAFEKILL(m_pStream);
}

void f2dWaveDecoder::clear()
{
	vector<IChunk*>::iterator i = m_ChunkList.begin();
	while(i != m_ChunkList.end())
	{
		FCYSAFEDEL(*i);
		i++;
	}
}

fuInt f2dWaveDecoder::GetBufferSize()
{
	CDataChunk* tChunk = (CDataChunk*)(m_Chunk[tagDATA]);
	return tChunk->m_Size;
}

fuInt f2dWaveDecoder::GetAvgBytesPerSec()
{
	CFormatChunk* tChunk = (CFormatChunk*)(m_Chunk[tagFMT]);
	return tChunk->m_AvgBytesPerSec;
}

fuShort f2dWaveDecoder::GetBlockAlign()
{
	CFormatChunk* tChunk = (CFormatChunk*)(m_Chunk[tagFMT]);
	return tChunk->m_BlockAlign;
}

fuShort f2dWaveDecoder::GetChannelCount()
{
	CFormatChunk* tChunk = (CFormatChunk*)(m_Chunk[tagFMT]);
	return tChunk->m_Channels;
}

fuInt f2dWaveDecoder::GetSamplesPerSec()
{
	CFormatChunk* tChunk = (CFormatChunk*)(m_Chunk[tagFMT]);
	return tChunk->m_SamplesPerSec;
}

fuShort f2dWaveDecoder::GetFormatTag()
{
	CFormatChunk* tChunk = (CFormatChunk*)(m_Chunk[tagFMT]);
	return tChunk->m_FormatTag;
}

fuShort f2dWaveDecoder::GetBitsPerSample()
{
	CFormatChunk* tChunk = (CFormatChunk*)(m_Chunk[tagFMT]);
	return tChunk->m_BitsPerSample;
}

fLen f2dWaveDecoder::GetPosition()
{
	return m_cPointer;
}

fResult f2dWaveDecoder::SetPosition(F2DSEEKORIGIN Origin, fInt Offset)
{
	switch(Origin)
	{
	case FCYSEEKORIGIN_CUR:
		break;
	case FCYSEEKORIGIN_BEG:
		m_cPointer = 0;
		break;
	case FCYSEEKORIGIN_END:
		m_cPointer = GetBufferSize();
		break;
	default:
		return FCYERR_INVAILDPARAM;
	}
	if(Offset<0 && ((fuInt)(-Offset))>m_cPointer)
	{
		m_cPointer = 0;
		return FCYERR_OUTOFRANGE;
	}
	else if(Offset>0 && Offset+m_cPointer>=GetBufferSize())
	{
		m_cPointer = GetBufferSize();
		return FCYERR_OUTOFRANGE;
	}
	m_cPointer+=Offset;
	return FCYERR_OK;
}

fResult f2dWaveDecoder::Read(fData pBuffer, fuInt SizeToRead, fuInt* pSizeRead)
{
	// 锁定流
	m_pStream->Lock();

	// 寻址
	CDataChunk* tChunk = (CDataChunk*)(m_Chunk[tagDATA]);
	m_pStream->SetPosition(FCYSEEKORIGIN_BEG, tChunk->m_BasePointer + m_cPointer);

	// 计算读取大小
	fuInt tRest = tChunk->m_Size - m_cPointer;
	if(SizeToRead > tRest)
		SizeToRead = tRest;

	// 从流中读取数据
	fLen tStreamRead = 0;
	m_pStream->ReadBytes(pBuffer, SizeToRead, &tStreamRead);
	if(SizeToRead != tStreamRead)
		SizeToRead = (fuInt)tStreamRead;
	if(pSizeRead)
		*pSizeRead = SizeToRead;

	// 指针后移
	m_cPointer += SizeToRead;
	
	// 解锁流
	m_pStream->Unlock();

	return FCYERR_OK;
}

////////////////////////////////////////////////////////////////////////////////

size_t f2dVorbisDecoder::streamReadFunc(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	size_t tLen = (size_t)(((fcyStream*)datasource)->GetLength() - ((fcyStream*)datasource)->GetPosition());
	
	fuInt tCountToRead = (tLen / size);
	if(tCountToRead > nmemb)
		tCountToRead = nmemb;

	((fcyStream*)datasource)->ReadBytes((fData)ptr, tCountToRead * size, NULL);

	return tCountToRead;
}

int f2dVorbisDecoder::streamSeekFunc(void *datasource, ogg_int64_t offset, int whence)
{
	FCYSEEKORIGIN tOrigin = FCYSEEKORIGIN_BEG;
	switch(whence)
	{
	case 0:
		tOrigin = FCYSEEKORIGIN_BEG;
		break;
	case 1:
		tOrigin = FCYSEEKORIGIN_CUR;
		break;
	case 2:
		tOrigin = FCYSEEKORIGIN_END;
		break;
	}
	if(FCYFAILED(((fcyStream*)datasource)->SetPosition(tOrigin, offset)))
		return -1;
	else
		return 0;
}

int f2dVorbisDecoder::streamCloseFunc(void *datasource)
{
	return 0;
}

long f2dVorbisDecoder::streamTellFunc(void *datasource)
{
	return (long)((fcyStream*)datasource)->GetPosition();
}

f2dVorbisDecoder::f2dVorbisDecoder(fcyStream* pStream)
	: m_CurrentSec(0), m_CurrentPos(0), m_pStream(pStream)
{
	ov_callbacks tCallbacks = 
	{
		(size_t (*)(void *, size_t, size_t, void *))  streamReadFunc,
		(int (*)(void *, ogg_int64_t, int))           streamSeekFunc,
		(int (*)(void *))                             streamCloseFunc,
		(long (*)(void *))                            streamTellFunc
	};

	if(!m_pStream)
		throw fcyException("f2dVorbisDecoder::f2dVorbisDecoder", "Invalid Pointer.");
	m_pStream->AddRef();

	m_pStream->Lock();
	m_pStream->SetPosition(FCYSEEKORIGIN_BEG, m_CurrentPos);

	int tRet = ov_open_callbacks(m_pStream, &m_OggFile, NULL, 0, tCallbacks);
	
	m_CurrentPos = m_pStream->GetPosition();
 	m_pStream->Unlock();

	if(tRet != 0)
	{
		FCYSAFEKILL(m_pStream);
		throw fcyException("f2dVorbisDecoder::f2dVorbisDecoder", "Invalid Stream.");
	}
}

f2dVorbisDecoder::~f2dVorbisDecoder()
{
	m_pStream->Lock();
	m_pStream->SetPosition(FCYSEEKORIGIN_BEG, m_CurrentPos);
	
	ov_clear(&m_OggFile);
	
	m_pStream->Unlock();

	FCYSAFEKILL(m_pStream);
}

fuInt f2dVorbisDecoder::GetBufferSize()
{
	return (fuInt)ov_pcm_total(&m_OggFile, -1) * GetBlockAlign();
}

fuInt f2dVorbisDecoder::GetAvgBytesPerSec()
{
	return GetBlockAlign() * GetSamplesPerSec();
}

fuShort f2dVorbisDecoder::GetBlockAlign()
{
	return GetChannelCount() * 2;  // 16 bits
}

fuShort f2dVorbisDecoder::GetChannelCount()
{
	vorbis_info* tInfo = ov_info(&m_OggFile, -1);
	return (fuShort)tInfo->channels;
}

fuInt f2dVorbisDecoder::GetSamplesPerSec()
{
	vorbis_info* tInfo = ov_info(&m_OggFile, -1);
	return (fuInt)tInfo->rate;
}

fuShort f2dVorbisDecoder::GetFormatTag()
{
	return 1;
}

fuShort f2dVorbisDecoder::GetBitsPerSample()
{
	return 16; // 16bits
}

fLen f2dVorbisDecoder::GetPosition()
{
	return ov_pcm_tell(&m_OggFile);
}

fResult f2dVorbisDecoder::SetPosition(F2DSEEKORIGIN Origin, fInt Offset)
{
	// 计算目标位置
	fuInt tPCMPointer = (fuInt)GetPosition();

	switch(Origin)
	{
	case FCYSEEKORIGIN_CUR:
		break;
	case FCYSEEKORIGIN_BEG:
		tPCMPointer = 0;
		break;
	case FCYSEEKORIGIN_END:
		tPCMPointer = GetBufferSize();
		break;
	default:
		return FCYERR_INVAILDPARAM;
	}
	if(Offset<0 && ((fuInt)(-Offset))>tPCMPointer)
	{
		tPCMPointer = 0;
		return FCYERR_OUTOFRANGE;
	}
	else if(Offset>0 && Offset+tPCMPointer>=GetBufferSize())
	{
		tPCMPointer = GetBufferSize();
		return FCYERR_OUTOFRANGE;
	}
	tPCMPointer+=Offset;

	// 进行Seek操作
	m_CurrentSec = 0;
	
	m_pStream->Lock();  // 锁定流
	m_pStream->SetPosition(FCYSEEKORIGIN_BEG, m_CurrentPos);

	int tRet = ov_time_seek(&m_OggFile, tPCMPointer / (double)(GetSamplesPerSec() * GetBlockAlign()));
	
	m_CurrentPos = m_pStream->GetPosition();
	m_pStream->Unlock();

    return tRet == 0;
}

fResult f2dVorbisDecoder::Read(fData pBuffer, fuInt SizeToRead, fuInt* pSizeRead)
{
	if(pSizeRead)
		*pSizeRead = 0;

	m_pStream->Lock(); // 锁定流
	m_pStream->SetPosition(FCYSEEKORIGIN_BEG, m_CurrentPos);

	fuInt tSizeRead = 0;
	char* tBuffer = (char*)pBuffer;
	while(tSizeRead < SizeToRead)
	{
		long tRet = ov_read(&m_OggFile, tBuffer, SizeToRead - tSizeRead, 0, 2, 1, &m_CurrentSec);
		if(tRet<0)     // 错误
		{
			m_CurrentPos = m_pStream->GetPosition();
			m_pStream->Unlock();
			return FCYERR_INTERNALERR;
		}
		if(tRet == 0)  // 到尾部
			break;
		tSizeRead += tRet;
		tBuffer += tRet;
	}

	if(pSizeRead)
		*pSizeRead = tSizeRead;

	m_CurrentPos = m_pStream->GetPosition();
	m_pStream->Unlock();

    return FCYERR_OK;
}
