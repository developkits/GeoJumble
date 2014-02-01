#include "JsonUtils.h"


JsonUtils::JsonUtils(void)
{
}


JsonUtils::~JsonUtils(void)
{
}

JsonData JsonUtils::ReadJsonFromFile(string fileName)
{
	fstream file(fileName + ".json");
	Value root;
	Reader reader;

	bool parsingSuccessful = reader.parse(file,root,false);
	if(!parsingSuccessful)
	{
		string error = reader.getFormattedErrorMessages();
	}

	JsonData toReturn(root);

	return toReturn;
}
