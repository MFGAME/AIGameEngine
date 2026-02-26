#ifndef TEST_AI_ENGINE_H
#define TEST_AI_ENGINE_H

#include "../ai_engine.h"
#include "tests/test_macros.h"
#include "scene/2d/node_2d.h"

namespace TestAIEngine {

TEST_CASE("[AIEngine] Spawn Entity") {
	AIEngine *ai = memnew(AIEngine);
	Node *node = ai->spawn_entity("Node2D", "Player");

	CHECK(node != nullptr);
	CHECK(node->get_name() == "Player");
	CHECK(ai->get_child_count() == 1);

	memdelete(ai);
}

TEST_CASE("[AIEngine] Execute Command Spawn") {
	AIEngine *ai = memnew(AIEngine);
	String json_cmd = "{\"action\": \"spawn\", \"type\": \"Node2D\", \"name\": \"Enemy\"}";
	ai->execute_command(json_cmd);

	Node *enemy = ai->find_child("Enemy");
	CHECK(enemy != nullptr);
	CHECK(ai->get_child_count() == 1);

	memdelete(ai);
}

TEST_CASE("[AIEngine] Execute Command Call") {
	AIEngine *ai = memnew(AIEngine);
	Node *node = ai->spawn_entity("Node2D", "Player");
	REQUIRE(node->is_visible());

	String json_cmd = "{\"action\": \"call\", \"target\": \"Player\", \"method\": \"hide\", \"args\": []}";
	ai->execute_command(json_cmd);

	CHECK(node->is_visible() == false);

	memdelete(ai);
}

TEST_CASE("[AIEngine] API Key and Model Name") {
	AIEngine *ai = memnew(AIEngine);
	ai->set_api_key("test_key");
	CHECK(ai->get_api_key() == "test_key");

	ai->set_model_name("glm-5");
	CHECK(ai->get_model_name() == "glm-5");

	memdelete(ai);
}

} // namespace TestAIEngine

#endif // TEST_AI_ENGINE_H
