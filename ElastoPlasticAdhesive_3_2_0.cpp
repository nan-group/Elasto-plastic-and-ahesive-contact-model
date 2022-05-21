#include "PluginContactModelCore.h"
#include "CElastoPlasticAdhesive_V3_2_0.h"

using namespace NApiCm;


EXPORT_MACRO IPluginContactModel* GETCMINSTANCE()
{
	return new CElastoPlasticAdhesive_V3_2_0();
}

EXPORT_MACRO void RELEASECMINSTANCE(IPluginContactModel* plugin)
{
	if (nullptr != plugin)
	{
		delete plugin;
	}
}

EXPORT_MACRO int GETCMINTERFACEVERSION()
{
	static const int INTERFACE_VERSION_MAJOR = 0x03;
	static const int INTERFACE_VERSION_MINOR = 0x01;
	static const int INTERFACE_VERSION_PATCH = 0x00;

	return (INTERFACE_VERSION_MAJOR << 16 |
		INTERFACE_VERSION_MINOR << 8 |
		INTERFACE_VERSION_PATCH);
}


