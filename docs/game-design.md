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