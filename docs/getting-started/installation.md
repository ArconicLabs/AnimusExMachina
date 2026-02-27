# Installation

## Requirements

- Unreal Engine 5.5+
- A project with NavMesh support (most templates include this)

## Install via Git

Clone the plugin into your project's `Plugins/` directory:

```bash
cd YourProject/Plugins
git clone https://github.com/ArconicLabs/AnimusExMachina.git
```

## Install via Download

1. Download the latest release from [GitHub Releases](https://github.com/ArconicLabs/AnimusExMachina/releases)
2. Extract into `YourProject/Plugins/AnimusExMachina/`

## Enable the Plugin

Add to your `.uproject` file:

```json
{
  "Plugins": [
    {
      "Name": "AnimusExMachina",
      "Enabled": true
    }
  ]
}
```

## Rebuild

Rebuild your project. The plugin depends on these engine modules (resolved automatically):

- `AIModule`
- `StateTreeModule`
- `GameplayStateTreeModule`
- `NavigationSystem`
- `GameplayTags`

And these engine plugins:

- `StateTree`
- `GameplayStateTree`
