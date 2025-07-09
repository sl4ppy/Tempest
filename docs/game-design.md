# Game Design Document

## Game Overview

Tempest is a vector graphics arcade game where players control a ship at the edge of a 3D tube, defending against waves of enemies that emerge from the tube's depth. The game features unique geometric gameplay with increasing difficulty and variety.

## Core Gameplay Mechanics

### Player Character
**Ship (Claw)**
- **Movement**: Rotates around the tube's perimeter
- **Position**: 16 segments (0-15) around the tube edge
- **Controls**: 
  - **Zap**: Rapid-fire weapon for close enemies
  - **Fire**: Standard weapon for distant enemies
  - **Movement**: Rotate left/right around tube
  - **Super Zapper**: Limited use weapon (0-2 uses per level)
- **Lives**: 3-5 (configurable)
- **Death**: Contact with enemies, enemy shots, or spikes

### Tube Geometry
**3D Tube Structure**
- **Segments**: 16 segments around the tube perimeter
- **Depth**: Enemies emerge from the tube's depth
- **Perspective**: 3D vector graphics with depth scaling
- **Scaling**: Objects scale based on distance from player
- **Segments**: Each level has different tube geometry

### Enemy Types (Based on ASM Analysis)

#### 1. Flippers (Type 1)
- **Behavior**: Move along tube segments, flip between segments
- **Speed**: Moderate movement speed
- **Threat Level**: Low to medium
- **Special**: Can grab player if they make contact
- **AI Pattern**: Segment-to-segment movement with random flipping
- **Movement Type**: 0=no adjustment, 8=clockwise, 0xC=counter-clockwise

#### 2. Pulsars (Type 2)
- **Behavior**: Pulse in place, can fire projectiles
- **Speed**: Stationary with pulsing animation
- **Threat Level**: Medium (due to shooting)
- **Special**: Fire enemy shots at player
- **AI Pattern**: Pulse timing, shooting when player is in range
- **Movement Type**: Stationary with pulsing effect

#### 3. Tankers (Type 3)
- **Behavior**: Heavy, slow-moving enemies
- **Speed**: Slow movement
- **Threat Level**: Medium to high
- **Special**: Can carry other enemies
- **AI Pattern**: Straight-line movement toward player
- **Movement Type**: Direct path toward player segment

#### 4. Spikers (Type 4)
- **Behavior**: Grow spikes on tube segments
- **Speed**: Stationary during spike growth
- **Threat Level**: High (spikes block movement)
- **Special**: Create impassable barriers on tube segments
- **AI Pattern**: Spike growth timing, segment targeting
- **Movement Type**: Stationary, focus on spike creation

#### 5. Fuzzballs (Type 5)
- **Behavior**: Fast, erratic movement
- **Speed**: High movement speed
- **Threat Level**: High (difficult to hit)
- **Special**: Complex movement patterns
- **AI Pattern**: Random direction changes, speed variations
- **Movement Type**: Erratic, unpredictable patterns

### Enemy Management System (Based on RAM Analysis)

#### Enemy Counters
- **Enemies Inside Tube**: Tracked at RAM 0108
- **Enemies at Top**: Tracked at RAM 0109
- **Current Counts**: Individual counts for each enemy type (RAM 0142-0146)
- **Maximum Limits**: Per-type limits (RAM 012E-0132)
- **Minimum Ratios**: Proportion controls (RAM 0129-012D)

#### Enemy Spawning
- **Timer System**: 64 enemy timers (RAM 0243-0282)
- **Movement Data**: 7 active enemies with movement/type info (RAM 0283-0289)
- **Additional Movement**: Secondary movement data (RAM 028A-0290)

### Weapon Systems

#### Zap Weapon
- **Range**: Short range, instant hit
- **Damage**: Destroys enemies instantly
- **Usage**: Close combat, rapid-fire
- **Limitations**: Limited range, energy consumption
- **Input**: Space bar or gamepad button

#### Fire Weapon
- **Range**: Long range, projectile-based
- **Damage**: Destroys enemies on hit
- **Usage**: Distant enemies, precision shots
- **Limitations**: Projectile travel time, limited fire rate
- **Input**: Left Control or gamepad button

#### Super Zapper
- **Range**: Full screen, instant kill
- **Damage**: Destroys all enemies on screen
- **Usage**: Emergency weapon, limited uses (0-2 per level)
- **Limitations**: Very limited availability
- **Input**: Special key or gamepad combination

### Projectile System (Based on RAM Analysis)

#### Player Bullets
- **Maximum Active**: 8 bullets (RAM 0135)
- **Segment Tracking**: Bullet target segments (RAM 02AD-02B4)
- **Position Tracking**: Bullet Y positions (RAM 02D3-02DA)
- **Movement**: Travel down tube toward enemies

#### Enemy Bullets
- **Maximum Active**: 2 enemy shots (RAM 00A6)
- **Position Tracking**: Enemy bullet Y positions (RAM 02DB-02DC)
- **Movement**: Travel up tube toward player

### Level Progression

#### Level Structure
- **Wave System**: Multiple waves per level
- **Enemy Spawning**: Enemies emerge from tube depth
- **Difficulty Scaling**: More enemies, faster movement
- **Level Completion**: Clear all enemies to advance

#### Level Types
- **Standard Levels**: Normal enemy patterns
- **Bonus Levels**: Special challenges or rewards
- **Boss Levels**: Unique enemy encounters

#### Difficulty Progression
- **Enemy Count**: Increases with level number
- **Movement Speed**: Enemies move faster
- **Spawn Rate**: More frequent enemy spawning
- **Enemy Types**: More dangerous enemies appear

### Scoring System

#### Points
- **Enemy Destruction**: Points for each enemy killed
- **Level Completion**: Bonus points for completing levels
- **Survival Bonus**: Points for surviving longer
- **Multiplier**: Score multipliers for consecutive hits

#### High Scores
- **Persistent Storage**: High scores saved between sessions
- **Player Initials**: 3-character initials for high scores
- **Multiple Entries**: Top 10 scores maintained
- **Storage**: EAROM-based persistent storage

### Game States (Based on ASM Analysis)

#### Main Menu
- **Start Game**: Begin new game
- **Continue**: Resume saved game (if available)
- **Options**: Configuration settings
- **High Scores**: View top scores

#### Playing State
- **Active Gameplay**: Normal game operation
- **Pause**: Game paused, menu overlay
- **Level Transition**: Between level loading

#### Game Over
- **Death Animation**: Player ship destruction
- **Score Display**: Final score calculation
- **High Score Entry**: If applicable
- **Continue Option**: Extra life or restart

#### Attract Mode
- **Demo Play**: Automated gameplay demonstration
- **High Score Display**: Show top scores
- **Game Information**: Title, credits, instructions

## Level Design

### Tube Geometry Variations
- **Simple Tubes**: Basic circular tubes
- **Complex Tubes**: Irregular shapes, obstacles
- **Dynamic Tubes**: Changing geometry during play
- **Special Tubes**: Unique designs for specific levels

### Enemy Placement
- **Spawn Points**: Strategic enemy spawning locations
- **Wave Timing**: Coordinated enemy wave attacks
- **Difficulty Balance**: Progressive challenge increase
- **Variety**: Mix of enemy types for engagement

### Environmental Elements
- **Spikes**: Static obstacles on tube segments (RAM 03AC-03BB)
- **Power-ups**: Temporary enhancements (future consideration)
- **Visual Effects**: Particle effects, explosions
- **Background Elements**: Tube texture, depth effects

## User Interface

### HUD Elements
- **Score Display**: Current score (RAM 0040-0045)
- **Lives Indicator**: Remaining lives (RAM 0048-0049)
- **Level Display**: Current level number (RAM 009F)
- **Weapon Status**: Zap/Fire weapon indicators
- **Super Zapper**: Remaining uses display (RAM 03AA)

### Menu Systems
- **Main Menu**: Game start, options, high scores
- **Pause Menu**: Resume, options, quit
- **Options Menu**: Graphics, audio, input settings
- **High Score Menu**: Score display and entry

### Visual Feedback
- **Explosions**: Enemy destruction effects
- **Player Feedback**: Hit indicators, damage effects
- **Level Transitions**: Visual effects between levels
- **Game Over**: Death animation and effects

## Audio Design

### Sound Effects
- **Weapon Sounds**: Zap and fire weapon audio
- **Explosions**: Enemy destruction sounds
- **Player Sounds**: Movement, damage, death
- **UI Sounds**: Menu navigation, selections
- **Spinner Sounds**: Movement feedback

### Music
- **Background Music**: Level-specific themes
- **Menu Music**: Title screen and menu themes
- **Tension Music**: High-difficulty situation music
- **Victory Music**: Level completion themes

### Audio Implementation
- **Synthesis**: Modern recreation of POKEY chip sounds
- **Spatial Audio**: 3D positioning for immersion
- **Dynamic Mixing**: Volume adjustment based on game state
- **Platform Support**: Cross-platform audio compatibility

## Accessibility Features

### Visual Accessibility
- **Color Blind Support**: Alternative color schemes
- **High Contrast**: Enhanced visibility options
- **Scalable UI**: Adjustable interface size
- **Visual Indicators**: Clear feedback for all game states

### Audio Accessibility
- **Volume Controls**: Individual volume sliders
- **Audio Cues**: Non-visual feedback for important events
- **Subtitles**: Text indicators for audio events

### Input Accessibility
- **Customizable Controls**: Remappable key bindings
- **Multiple Input Methods**: Keyboard, mouse, gamepad support
- **Sensitivity Options**: Adjustable input sensitivity
- **Alternative Controls**: One-handed play options

## Performance Considerations

### Frame Rate Targets
- **Target FPS**: 60 FPS consistent
- **Frame Budget**: 16.67ms per frame
- **CPU Usage**: <5% on modern hardware
- **Memory Usage**: <100MB total

### Optimization Strategies
- **Spatial Partitioning**: For collision detection
- **Level-of-Detail**: Graphics quality scaling
- **Culling**: Frustum and occlusion culling
- **Batching**: Render command batching
- **Multithreading**: Parallel processing where safe

### Critical Performance Paths
1. **Enemy AI Updates**: Must complete within 2ms per frame
2. **Collision Detection**: Spatial partitioning for tube segments
3. **Vector Rendering**: Command batching for efficient GPU usage
4. **Audio Synthesis**: Real-time POKEY chip emulation
5. **Input Processing**: Low-latency response for player controls 

## Rendering System Design

### Basic Rendering Infrastructure

#### Graphics API Abstraction
The rendering system uses a platform-independent graphics API abstraction layer that supports both OpenGL and Vulkan:

**IGraphicsAPI Interface**:
- **Initialization**: Graphics context setup and configuration
- **Window Management**: Cross-platform window creation and management
- **Context Management**: Graphics state management and viewport control
- **Buffer Management**: Vertex and index buffer management for efficient rendering

**OpenGL Implementation**:
- Modern OpenGL 4.6 with core profile
- Vertex Array Objects (VAO) for efficient state management
- Vertex Buffer Objects (VBO) for geometry data
- Element Buffer Objects (EBO) for indexed rendering
- Frame Buffer Objects (FBO) for post-processing

**Vulkan Implementation** (Optional):
- Modern Vulkan 1.1+ for maximum performance
- Command buffer batching for efficient GPU usage
- Memory management with device-local memory
- Multi-threaded command recording

#### Renderer Class Architecture
The main Renderer class coordinates all rendering operations:

**Basic Rendering**:
- Primitive rendering (lines, triangles, quads, circles)
- Color and alpha management
- Matrix state management (view, projection, model)
- Render command batching for performance

**Vector Graphics**:
- Vector command processing based on original vsdraw commands
- Command batching and optimization
- Depth-sorted rendering for proper perspective
- Vector shape generation for game objects

**Tube Geometry**:
- 3D tube rendering with 16 segments
- Perspective projection with depth scaling
- Segment-based positioning and scaling
- Background and border rendering

**Game Object Rendering**:
- Player ship rendering with vector graphics
- Enemy rendering for all 5 types (Flipper, Pulsar, Tanker, Spiker, Fuzzball)
- Projectile rendering with depth-based scaling
- Explosion effects with particle systems

#### Vector Command System
Based on the original game's vsdraw commands, the vector command system provides:

**Command Types**:
- **Move**: Move to position without drawing
- **Draw**: Draw line to position
- **Scale**: Set rendering scale for depth effects
- **Color**: Set rendering color
- **Return**: End shape definition

**Command Processing**:
- Command batching for efficient GPU usage
- Depth sorting for proper perspective rendering
- Adjacent line merging for performance
- Redundant command removal

**Game Object Generation**:
- Player ship vector commands based on segment position
- Enemy vector commands based on type and position
- Projectile vector commands with depth scaling
- Explosion vector commands with particle effects

#### Tube Geometry Rendering
The 3D tube system recreates the original game's perspective:

**Tube Structure**:
- 16 segments around tube perimeter (0-15)
- Depth-based scaling for perspective effect
- Segment positioning with 3D coordinates
- Background and border rendering

**Perspective System**:
- Depth-based object scaling
- Segment position calculation
- Perspective matrix generation
- Depth range management

**Rendering Features**:
- Segment background rendering
- Segment border rendering
- Depth-based visibility culling
- Position validation within tube

#### Shader Management
Comprehensive shader system with built-in Tempest shaders:

**Built-in Shaders**:
- **Vector Shaders**: Vector graphics rendering with color and scale
- **Tube Shaders**: 3D tube geometry with perspective projection
- **Particle Shaders**: Particle effects with alpha blending
- **UI Shaders**: User interface rendering with text support

**Shader Features**:
- Automatic shader compilation and linking
- Uniform management for matrix and color data
- Error handling and validation
- Performance optimization with shader caching

#### Camera and Viewport System
3D camera system for perspective rendering:

**Camera Features**:
- Position, target, and up vector management
- Field of view and near/far plane configuration
- View and projection matrix generation
- Camera movement (rotate, zoom, pan)

**Viewport Management**:
- Viewport configuration and aspect ratio
- Screen coordinate management
- Multi-viewport support for UI overlays
- Resolution-independent rendering

#### Frame Buffer Management
Post-processing and effects support:

**Frame Buffer Features**:
- Color and depth texture attachment
- Multiple render target support
- Frame buffer blitting for screen output
- Resize handling for window changes

**Post-Processing Pipeline**:
- Multi-pass rendering for effects
- Texture-based post-processing
- Screen-space effects
- Performance monitoring and optimization

### Rendering Performance Considerations

#### Optimization Strategies
- **Command Batching**: Group similar render commands for efficiency
- **Depth Sorting**: Sort objects by depth for proper rendering order
- **Frustum Culling**: Skip rendering objects outside view frustum
- **Level-of-Detail**: Adjust detail based on distance and performance
- **Texture Atlasing**: Combine textures to reduce draw calls

#### Memory Management
- **Vertex Buffer Optimization**: Efficient vertex data storage
- **Index Buffer Usage**: Indexed rendering for reduced memory usage
- **Texture Compression**: Compressed textures for reduced memory footprint
- **Shader Program Caching**: Cache compiled shaders for performance

#### Performance Targets
- **60 FPS**: Consistent frame rate target
- **<16ms**: Frame time budget for rendering
- **<4MB**: Graphics memory usage
- **<2ms**: Vector command processing time
- **<1ms**: Tube geometry rendering time

### Integration with Game Systems

#### Game Engine Integration
The rendering system integrates with all game systems:

**Entity Component System**:
- Transform components for positioning
- Render components for visual data
- Material components for appearance
- Animation components for movement

**Event System**:
- Render events for visual updates
- Animation events for movement
- Effect events for explosions
- UI events for interface updates

**Input System**:
- Camera control for debugging
- UI interaction for menus
- Visual feedback for controls
- Debug rendering for development

#### Platform Support
Cross-platform rendering support:

**Windows**:
- OpenGL 4.6 with GLFW/SDL2
- DirectX 12 support (optional)
- High DPI display support
- Multi-monitor support

**macOS**:
- OpenGL 4.1 with GLFW
- Metal support (future consideration)
- Retina display support
- Window management integration

**Linux**:
- OpenGL 4.6 with GLFW/X11
- Wayland support (optional)
- Multi-monitor support
- Desktop integration

**Web** (Future):
- WebGL 2.0 support
- Canvas-based rendering
- Browser compatibility
- Progressive web app features 