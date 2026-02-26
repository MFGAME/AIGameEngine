#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "scene/main/node.h"
#include "core/variant/variant.h"
#include "core/string/ustring.h"

class AIEngine : public Node {
	GDCLASS(AIEngine, Node);

protected:
	static void _bind_methods();

public:
	Node *spawn_entity(const String &p_type, const String &p_name);
	void execute_command(const String &p_json);

	AIEngine();
};

#endif // AI_ENGINE_H
