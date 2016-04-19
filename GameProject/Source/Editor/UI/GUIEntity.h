#pragma once
#include <string>

class GUIEntity
{
	public:
		GUIEntity(const char *name = "GUI_entity");
		virtual ~GUIEntity() {};

		virtual void SetName(const char* name);
		const char* GetName() const;

	protected:
		std::string name;
};