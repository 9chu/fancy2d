#include "fcyModelBoundingBoxLabel.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

const char fcyModelBoundingBoxLabel::LABELNAME[8] = { 'B', 'O', 'U', 'N', 'D', 'B', 'O', 'X'};

void fcyModelBoundingBoxLabel::ReadData(fcyStream* pStream)
{
	fcyBinaryReader tReader(pStream);

	m_Min = fcyVec3(tReader.ReadFloat(), tReader.ReadFloat(), tReader.ReadFloat());
	m_Max = fcyVec3(tReader.ReadFloat(), tReader.ReadFloat(), tReader.ReadFloat());
}

void fcyModelBoundingBoxLabel::WriteData(fcyStream* pStream)
{
	fcyBinaryWriter tWriter(pStream);

	tWriter.Write(m_Min.x);
	tWriter.Write(m_Min.y);
	tWriter.Write(m_Min.z);
	tWriter.Write(m_Max.x);
	tWriter.Write(m_Max.y);
	tWriter.Write(m_Max.z);
}
