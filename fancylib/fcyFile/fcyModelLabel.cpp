#include "fcyModelLabel.h"

#include "fcyModelVertexLabel.h"
#include "fcyModelIndexLabel.h"
#include "fcyModelMaterialLabel.h"
#include "fcyModelSubsetLabel.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////

fcyModelLabelFactory::fcyModelLabelFactory()
{
	AutoRegister<fcyModelVertexLabel>   VertexLabel;
	AutoRegister<fcyModelIndexLabel>    IndexLabel;
	AutoRegister<fcyModelMaterialLabel> MaterialLabel;
	AutoRegister<fcyModelSubsetLabel>   SubsetLabel;
}
