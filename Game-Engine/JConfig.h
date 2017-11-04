#pragma once

#include "Application.h"

#include "Globals.h"
#include "Json\parson.h"

#define FILE_NAME "config.json"
class JConfig
{
public:
	JConfig();

	JConfig::JConfig(JSON_Object* section) : root_object(section)
	{}
	~JConfig();


	void Init();
	void Load();
	void Save()const;

public:
	JSON_Object*	root_object;
	JSON_Value*		root_value;

	char* file_name = FILE_NAME;
};
