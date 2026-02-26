# AI Game Engine (Godot AI Edition) - Usage Guide

This guide explains how to build and use the minimal, AI-driven version of Godot you've just created.

## 1. Building the Engine

To address the "bloated" issue, use the `ai_profile.py` build configuration. This profile disables 3D, XR, and many non-essential modules.

**For Linux (template_debug):**
```bash
scons profile=ai_profile platform=linuxbsd target=template_debug -j$(nproc)
```

**Note:** The `disable_3d=yes` and `disable_advanced_gui=yes` flags are primarily designed for export template targets (`template_debug` or `template_release`). If you want the editor, you may need to modify the profile or use `target=editor` without those specific flags.

## 2. Using the `AIEngine` Module in Godot (GDScript)

The `AIEngine` class is now registered and available. You can add it as a node to your scene and use its simplified API.

```gdscript
extends Node

var ai_engine = AIEngine.new()

func _ready():
    add_child(ai_engine)

    # Example 1: Spawning an entity using the simplified C++ API
    var player = ai_engine.spawn_entity("Sprite2D", "Player")
    player.position = Vector2(100, 100)

    # Example 2: Executing a JSON-based command (ideal for AI interaction)
    var cmd = '{"action": "spawn", "type": "ColorRect", "name": "Ground"}'
    ai_engine.execute_command(cmd)

    # Example 3: Modifying properties via JSON
    var set_cmd = '{"action": "set", "target": "Ground", "property": "color", "value": "red"}'
    ai_engine.execute_command(set_cmd)
```

## 3. Driving the Engine with GLM-5 (ZhipuAI)

To "drive" the engine from an external model like GLM-5, we recommend a simple Python bridge using ZhipuAI's SDK and Godot's built-in networking.

### Step A: Godot Receiver Script (GDScript)

Add this script to a node in your Godot project to listen for commands over UDP:

```gdscript
extends Node

var udp = PacketPeerUDP.new()
@onready var ai_engine = AIEngine.new()

func _ready():
    add_child(ai_engine)
    udp.listen(4242)

func _process(_delta):
    if udp.get_available_packet_count() > 0:
        var packet = udp.get_packet().get_string_from_utf8()
        print("Received command from AI: ", packet)
        ai_engine.execute_command(packet)
```

### Step B: Python AI Bridge (GLM-5 Integration)

Using the `zhipuai` Python SDK to generate commands based on natural language.

```python
import socket
import json
from zhipuai import ZhipuAI

client = ZhipuAI(api_key="YOUR_API_KEY")

def send_command_to_godot(command_dict):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    message = json.dumps(command_dict).encode('utf-8')
    sock.sendto(message, ("127.0.0.1", 4242))

def drive_engine(user_prompt):
    # Instruct GLM-5 to output JSON in our specific format
    system_prompt = (
        "You are a game designer assistant. You control a game engine. "
        "Output ONLY a JSON command in this format: "
        '{"action": "spawn", "type": "Sprite2D", "name": "NAME"} or '
        '{"action": "set", "target": "NAME", "property": "POSITION", "value": [x, y]}'
    )

    response = client.chat.completions.create(
        model="glm-4",  # Replace with "glm-5" when available in the SDK
        messages=[
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt}
        ],
    )

    ai_text = response.choices[0].message.content
    try:
        command = json.loads(ai_text)
        send_command_to_godot(command)
        print(f"Sent command to Godot: {command}")
    except json.JSONDecodeError:
        print(f"AI returned invalid JSON: {ai_text}")

# Example Usage
drive_engine("Spawn a red ground rectangle at the bottom of the screen.")
```

## 4. Key Advantages for AI

- **No Bloat:** The engine only includes what's necessary, making documentation and context for the AI easier to manage.
- **JSON Interface:** The AI doesn't need to learn complex GDScript syntax; it only needs to generate structured JSON commands.
- **High Level:** Instead of manipulating low-level RenderingServer or PhysicsServer directly, the AI uses `AIEngine` to perform common tasks in one call.
