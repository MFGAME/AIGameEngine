#include "ai_engine.h"
#include "core/io/json.h"
#include "core/object/class_db.h"

Node *AIEngine::spawn_entity(const String &p_type, const String &p_name) {
	Object *obj = ClassDB::instantiate(p_type);
	if (!obj) {
		ERR_PRINT("Failed to spawn entity of type: " + p_type);
		return nullptr;
	}

	Node *node = Object::cast_to<Node>(obj);
	if (!node) {
		memdelete(obj);
		ERR_PRINT("Type is not a Node: " + p_type);
		return nullptr;
	}

	node->set_name(p_name);
	add_child(node);
	return node;
}

void AIEngine::execute_command(const String &p_json) {
	JSON json;
	Error err = json.parse(p_json);
	if (err != OK) {
		ERR_PRINT("Failed to parse AI command JSON.");
		return;
	}

	Variant data = json.get_data();
	if (data.get_type() != Variant::DICTIONARY) {
		ERR_PRINT("AI command JSON must be a dictionary.");
		return;
	}

	Dictionary dict = data;
	String action = dict.get("action", "");

	if (action == "spawn") {
		String type = dict.get("type", "");
		String name = dict.get("name", "");
		spawn_entity(type, name);
	} else if (action == "set") {
		String target_name = dict.get("target", "");
		String property = dict.get("property", "");
		Variant value = dict.get("value", Variant());

		Node *target = find_child(target_name);
		if (target) {
			target->set(property, value);
		} else {
			ERR_PRINT("Target not found for set command: " + target_name);
		}
	} else {
		ERR_PRINT("Unknown AI action: " + action);
	}
}

void AIEngine::_bind_methods() {
	ClassDB::bind_method(D_METHOD("spawn_entity", "type", "name"), &AIEngine::spawn_entity);
	ClassDB::bind_method(D_METHOD("execute_command", "json"), &AIEngine::execute_command);
}

AIEngine::AIEngine() {
}
