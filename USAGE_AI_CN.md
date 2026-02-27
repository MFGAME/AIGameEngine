# AI 游戏引擎 (Godot AI 版) - 使用指南

本指南将介绍如何构建、运行以及使用此引擎的 AI 驱动功能。

## 1. 功能特性：AI 能做什么？

`AIEngine` 模块允许你通过 GLM-5 大模型使用自然语言来控制游戏世界。核心功能包括：

- **实体生成 (Entity Spawning)**: 例如 “生成一个名为 Hero 的玩家精灵”。
- **属性修改 (Property Modification)**: 例如 “将 Hero 移动到坐标 (500, 300)”。
- **行为触发 (Behavior Triggering)**: 例如 “隐藏敌人 (Enemy)”，“让玩家 (Player) 跳跃”（前提是节点定义了 `jump` 方法）。
- **批量处理**: 复杂的指令如 “创建一个蓝色背景并在中间放置一个玩家” 会被解析为多个动作执行。

## 2. 快速开始：如何运行

### 第一步：构建引擎
使用极简编译配置，确保引擎轻量且运行迅速。

```bash
scons profile=ai_profile platform=linuxbsd target=template_debug -j$(nproc)
```

**注意：** 如果你的系统不识别 `profile` 参数，可以直接将 `ai_profile.py` 中的内容作为命令行参数传入。

### 第二步：创建简单场景
1. 在 Godot 中创建一个新场景。
2. 在场景中添加一个 `AIEngine` 节点。
3. 为根节点添加如下脚本：

```gdscript
extends Node

@onready var ai = $AIEngine # 假设你已经添加了该节点

func _ready():
    # 1. 配置 API 密钥
    ai.api_key = "你的智谱AI_API_KEY"
    ai.model_name = "glm-4" # 或者 "glm-5"

    # 2. 连接信号（可选）
    ai.ai_response_received.connect(func(res): print("AI 响应: ", res))

    # 3. 驱动引擎！
    ai.prompt_ai("生成一个名为 MyObject 的 Node2D，并将其移动到 (100, 100)")
```

## 3. 支持的 JSON 动作（内部协议）

如果你想不通过 AI 直接发送指令，可以使用 `execute_command()` 方法，格式如下：

- **生成 (Spawn)**: `{"action": "spawn", "type": "Node2D", "name": "Player"}`
- **设置 (Set)**: `{"action": "set", "target": "Player", "property": "position", "value": [100, 200]}`
- **调用 (Call)**: `{"action": "call", "target": "Player", "method": "hide", "args": []}`

## 4. 为什么这种方式更适合 AI 模型？

- **简化上下文**: AI 只需要了解这 3 个高层动作，而不需要掌握整个复杂的 Godot API。
- **鲁棒性**: 生成结构化的 JSON 比生成完整的 GDScript 代码更不容易出现语法错误。
- **直接集成**: 无需额外的 Python 中转脚本，引擎直接与 AI 模型通信，延迟更低。
