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
	} else if (action == "call") {
		String target_name = dict.get("target", "");
		String method = dict.get("method", "");
		Array args = dict.get("args", Array());

		Node *target = find_child(target_name);
		if (target) {
			target->callv(method, args);
		} else {
			ERR_PRINT("Target not found for call command: " + target_name);
		}
	} else {
		ERR_PRINT("Unknown AI action: " + action);
	}
}

void AIEngine::set_api_key(const String &p_key) {
	api_key = p_key;
}

String AIEngine::get_api_key() const {
	return api_key;
}

void AIEngine::set_model_name(const String &p_name) {
	model_name = p_name;
}

String AIEngine::get_model_name() const {
	return model_name;
}

Error AIEngine::prompt_ai(const String &p_prompt) {
	if (api_key.is_empty()) {
		ERR_PRINT("API key not set for AIEngine.");
		return ERR_UNCONFIGURED;
	}

	if (is_requesting) {
		return ERR_BUSY;
	}

	http.instantiate();
	Error err = http->connect_to_host("open.bigmodel.cn", 443, true);
	if (err != OK) {
		return err;
	}

	is_requesting = true;
	set_process(true);

	// Prepare payload
	Dictionary system_msg;
	system_msg["role"] = "system";
	system_msg["content"] = "You are a game design engine assistant. Output ONLY a valid JSON command like "
							 "{\"action\": \"spawn\", \"type\": \"Node2D\", \"name\": \"Player\"} or "
							 "{\"action\": \"set\", \"target\": \"Player\", \"property\": \"position\", \"value\": [100, 200]} or "
							 "{\"action\": \"call\", \"target\": \"Player\", \"method\": \"hide\", \"args\": []}. "
							 "No preamble, no explanation.";

	Dictionary user_msg;
	user_msg["role"] = "user";
	user_msg["content"] = p_prompt;

	Array messages;
	messages.push_back(system_msg);
	messages.push_back(user_msg);

	Dictionary payload;
	payload["model"] = model_name;
	payload["messages"] = messages;

	String payload_str = JSON::stringify(payload);
	response_data.clear();
	set_meta("pending_payload", payload_str);
	set_meta("request_sent", false);

	return OK;
}

void AIEngine::_notification(int p_what) {
	if (p_what == NOTIFICATION_PROCESS) {
		if (!is_requesting || http.is_null()) {
			set_process(false);
			return;
		}

		http->poll();
		HTTPClient::Status status = http->get_status();

		if (status == HTTPClient::STATUS_CONNECTED) {
			if (!get_meta("request_sent")) {
				set_meta("request_sent", true);
				String payload = get_meta("pending_payload");

				Vector<String> headers;
				headers.push_back("Content-Type: application/json");
				headers.push_back("Authorization: Bearer " + api_key);

				http->request(HTTPClient::METHOD_POST, "/api/paas/v4/chat/completions", headers, payload);
			}
		} else if (status == HTTPClient::STATUS_BODY) {
			response_data.append_array(http->read_response_body_chunk());
		} else if (status == HTTPClient::STATUS_DISCONNECTED || status == HTTPClient::STATUS_CONNECTION_ERROR) {
			is_requesting = false;
			set_process(false);

			if (response_data.size() > 0) {
				String resp;
				resp.parse_utf8((const char *)response_data.ptr(), response_data.size());

				JSON json;
				if (json.parse(resp) == OK) {
					Dictionary d = json.get_data();
					Array choices = d.get("choices", Array());
					if (choices.size() > 0) {
						Dictionary first_choice = choices[0];
						Dictionary message = first_choice.get("message", Dictionary());
						String content = message.get("content", "");
						execute_command(content);
						emit_signal("ai_response_received", content);
					}
				}
			}
			http.unref();
		}
	}
}

void AIEngine::_bind_methods() {
	ClassDB::bind_method(D_METHOD("spawn_entity", "type", "name"), &AIEngine::spawn_entity);
	ClassDB::bind_method(D_METHOD("execute_command", "json"), &AIEngine::execute_command);
	ClassDB::bind_method(D_METHOD("prompt_ai", "prompt"), &AIEngine::prompt_ai);
	ClassDB::bind_method(D_METHOD("set_api_key", "key"), &AIEngine::set_api_key);
	ClassDB::bind_method(D_METHOD("get_api_key"), &AIEngine::get_api_key);
	ClassDB::bind_method(D_METHOD("set_model_name", "name"), &AIEngine::set_model_name);
	ClassDB::bind_method(D_METHOD("get_model_name"), &AIEngine::get_model_name);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "api_key"), "set_api_key", "get_api_key");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "model_name"), "set_model_name", "get_model_name");

	ADD_SIGNAL(MethodInfo("ai_response_received", PropertyInfo(Variant::STRING, "response")));
}

AIEngine::AIEngine() {
}
