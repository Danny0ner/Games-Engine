#include "Application.h"
#include "JConfig.h"
#include <list>
#include "Module.h"

static int malloc_count;
static void *counted_malloc(size_t size);
static void counted_free(void *ptr);


static void *counted_malloc(size_t size) {
	void *res = malloc(size);
	if (res != NULL) {
		malloc_count++;
	}
	return res;
}

static void counted_free(void *ptr) {
	if (ptr != NULL) {
		malloc_count--;
	}
	free(ptr);
}



JConfig::JConfig() {

	json_set_allocation_functions(counted_malloc, counted_free);
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
}

JConfig::~JConfig()
{
	//if(root_value !=nullptr)
	//json_value_free(root_value);

}

void JConfig::Init()
{
	root_value = json_parse_file(file_name);
	if (root_value == NULL) {
		root_value = json_value_init_object();
		json_serialize_to_file(root_value, file_name);


	}
	else {
		root_object = json_value_get_object(root_value);
	}
	char* serialized_string = json_serialize_to_string_pretty(root_value);
	std::list<Module*>::iterator item = App->list_modules.begin();

	while (item != App->list_modules.end()) {
		if (json_object_get_object(root_object, (*item)->name.c_str()) == NULL)
			json_object_set_value(root_object, (*item)->name.c_str(), json_value_init_object());
		item++;
	}
	serialized_string = json_serialize_to_string_pretty(root_value);

}

void JConfig::Load() {
	std::list<Module*>::iterator item = App->list_modules.begin();
	char* serialized_string = json_serialize_to_string_pretty(root_value);

	JSON_Object* object;

	while (item != App->list_modules.end()) {
		object = json_object_get_object(root_object, (*item)->name.c_str());
		(*item)->LoadConfig(object);
		item++;
	}

}

void JConfig::Save()const {
	std::list<Module*>::iterator item = App->list_modules.begin();

	JSON_Object* object;

	while (item != App->list_modules.end()) {
		object = json_object_get_object(root_object, (*item)->name.c_str());
		(*item)->SaveConfig(object);
		item++;
	}

	char* serialized_string = json_serialize_to_string_pretty(root_value);
	json_serialize_to_file(root_value, file_name);
}
