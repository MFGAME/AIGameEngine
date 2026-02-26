#ifndef TEST_AI_ENGINE_H
#define TEST_AI_ENGINE_H

#include "../ai_engine.h"
#include "tests/test_macros.h"
#include "scene/2d/sprite_2d.h"

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

TEST_CASE("[AIEngine] Execute Command Set Property") {
	AIEngine *ai = memnew(AIEngine);
	ai->spawn_entity("Node2D", "Player");

	String json_cmd = "{\"action\": \"set\", \"target\": \"Player\", \"property\": \"position\", \"value\": [100, 200]}";
	ai->execute_command(json_cmd);

	Node2D *player = Object::cast_to<Node2D>(ai->find_child("Player"));
	REQUIRE(player != nullptr);
	CHECK(player->get_position() == Vector2(100, 200));

	memdelete(ai);
}

} // namespace TestAIEngine

#endif // TEST_AI_ENGINE_H
