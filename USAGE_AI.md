# AI Game Engine (Godot AI Edition) - Usage Guide

This guide explains how to build and use the minimal, AI-driven version of Godot you've just created.

## 1. Building the Engine

To address the "bloated" issue, use the `ai_profile.py` build configuration. This profile disables 3D, XR, and many non-essential modules.

**For Linux (template_debug):**
```bash
scons profile=ai_profile platform=linuxbsd target=template_debug -j$(nproc)
```

## 2. Using the `AIEngine` Module in Godot (GDScript)

The `AIEngine` class is now integrated with GLM-5. You can drive the engine directly from GDScript using natural language.

```gdscript
extends Node

@onready var ai_engine = AIEngine.new()

func _ready():
    add_child(ai_engine)

    # 1. Setup your ZhipuAI API Key
    ai_engine.api_key = "YOUR_ZHIPUAI_API_KEY"
    ai_engine.model_name = "glm-4" # or "glm-5" when available

    # 2. Connect to the response signal (optional)
    ai_engine.ai_response_received.connect(_on_ai_response)

    # 3. Prompt the AI to build something!
    # This will automatically call spawn_entity and execute_command internally.
    ai_engine.prompt_ai("Spawn a player sprite and set its position to 200, 200")

func _on_ai_response(response: String):
    print("AI Response (JSON): ", response)
```

## 3. Simplified API Reference (C++)

The `AIEngine` node provides several key methods exposed to GDScript:

- `prompt_ai(prompt: String)`: Asynchronously queries GLM-5 and executes the returned JSON commands.
- `execute_command(json: String)`: Manually execute a structured JSON command.
- `spawn_entity(type: String, name: String)`: Spawns a node of any valid engine type.
- `api_key`: Your ZhipuAI API key.
- `model_name`: The model to use (default "glm-4").

## 4. How it Works

1.  **Direct Integration:** The `AIEngine` module uses Godot's `HTTPClient` to communicate directly with ZhipuAI's servers.
2.  **Automatic Parsing:** It enforces a JSON-only response format via a system prompt.
3.  **Engine Reflection:** It uses Godot's `ClassDB` to instantiate any registered class by name, making it incredibly flexible.
4.  **Minimalist Build:** By using `ai_profile.py`, you get an engine that is fast, lightweight, and perfect for embedding in AI-driven applications.
