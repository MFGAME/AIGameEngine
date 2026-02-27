#ifndef AI_ENGINE_H
#define AI_ENGINE_H

#include "scene/main/node.h"
#include "core/io/http_client.h"
#include "core/variant/variant.h"
#include "core/string/ustring.h"

class AIEngine : public Node {
	GDCLASS(AIEngine, Node);

	Ref<HTTPClient> http;
	String api_key;
	String model_name = "glm-4";
	bool is_requesting = false;
	Vector<uint8_t> response_data;

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Node *spawn_entity(const String &p_type, const String &p_name);
	void execute_command(const String &p_json);

	void set_api_key(const String &p_key);
	String get_api_key() const;

	void set_model_name(const String &p_name);
	String get_model_name() const;

	Error prompt_ai(const String &p_prompt);

	AIEngine();
};

#endif // AI_ENGINE_H
