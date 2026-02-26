# AI Game Engine (Godot AI Edition) - Usage Guide

This guide explains how to build, run, and use the AI-driven features of this engine.

## 1. Capabilities: What can you do?

The `AIEngine` module allows you to control the game world using natural language through the GLM-5 model. Key capabilities include:

- **Entity Spawning**: "Spawn a player sprite named Hero."
- **Property Modification**: "Move the Hero to position 500, 300."
- **Behavior Triggering**: "Hide the Enemy", "Make the Player jump" (requires the node to have a `jump` method).
- **Batch Processing**: Complex instructions like "Create a scene with a blue background and a player in the middle" are parsed into multiple actions.

## 2. Quick Start: How to Run

### Step A: Build the Engine
Use the minimalist profile to ensure a lean and fast engine.

```bash
scons profile=ai_profile platform=linuxbsd target=template_debug -j$(nproc)
```

**Note:** If `profile` argument is not recognized on your system, you can use the profile contents directly as command-line arguments.

### Step B: Create a Simple Scene
1. Create a new Godot scene.
2. Add an `AIEngine` node to the scene.
3. Attach a script to the root node:

```gdscript
extends Node

@onready var ai = $AIEngine # Assuming you added the node

func _ready():
    # 1. Configure
    ai.api_key = "YOUR_ZHIPUAI_API_KEY"
    ai.model_name = "glm-4" # Or "glm-5"

    # 2. Connect (optional)
    ai.ai_response_received.connect(func(res): print("AI says: ", res))

    # 3. Drive the engine!
    ai.prompt_ai("Spawn a Node2D named MyObject and move it to 100, 100")
```

## 3. Supported JSON Actions (Internal)

If you want to send raw commands without using the AI prompt, use `execute_command()` with these formats:

- **Spawn**: `{"action": "spawn", "type": "Node2D", "name": "Player"}`
- **Set**: `{"action": "set", "target": "Player", "property": "position", "value": [100, 200]}`
- **Call**: `{"action": "call", "target": "Player", "method": "hide", "args": []}`

## 4. Why this is better for AI models

- **Simplified Context**: The AI only needs to know about these 3 high-level actions rather than the entire Godot API.
- **Robustness**: JSON is less prone to syntax errors than generating full GDScript code.
- **Direct Integration**: No external Python scripts are required; the engine talks directly to the AI model.
