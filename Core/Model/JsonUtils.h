#pragma once
#include <fstream>
#include <json\json.h>
#include <string>
#include "ModelStructs.h"

using namespace std;
using namespace Json;

struct JsonData;

class JsonUtils
{
public:
	JsonUtils(void);
	~JsonUtils(void);
	JsonData ReadJsonFromFile(string fileName);
};

struct JsonData
{
	JsonData(Value aRoot)
	{
		root = aRoot;
	}

	Value root;

	Value GetValue(string path)
	{
		char delimiter = '.';
		size_t pos = 0;
		Value val = root;
		while((pos = path.find(delimiter)) != string::npos)
		{
			string next = path.substr(0,pos);
			val = val[next];
			if(val == 0)
			{
				return Value::null;
			}
			path.erase(0,pos + 1);
		}

		return val[path];
	}
};