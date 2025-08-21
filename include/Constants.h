#pragma once

namespace Constants {
    // Screen dimensions
    constexpr int SCREEN_WIDTH = 1280;
    constexpr int SCREEN_HEIGHT = 720;
    constexpr int TARGET_FPS = 60;
    
    // Map settings
    constexpr int MAP_WIDTH = 30;
    constexpr int MAP_HEIGHT = 30;
    constexpr int TILE_SIZE = 32;
    constexpr int START_X = 1;
    constexpr int START_Y = 1;
    constexpr int GOAL_X = 28;
    constexpr int GOAL_Y = 28;
    
    // Animation timings (milliseconds)
    constexpr int DICE_ANIMATION_TIME = 800;
    constexpr int COIN_ANIMATION_TIME = 2500;
    
    // Combat settings
    constexpr int BASE_PLAYER_HP = 100;
    constexpr int BASE_PLAYER_MP = 50;
    constexpr int BASE_PLAYER_ATK = 20;
    constexpr int BASE_PLAYER_DEF = 10;
    constexpr float BASE_PLAYER_CRIT = 0.1f;
    constexpr float BASE_PLAYER_EVADE = 0.05f;
    
    constexpr int BASE_ENEMY_HP = 70;
    constexpr int BASE_ENEMY_MP = 30;
    constexpr int BASE_ENEMY_ATK = 15;
    constexpr int BASE_ENEMY_DEF = 8;
    
    constexpr int BASE_BOSS_HP = 180;
    constexpr int BASE_BOSS_MP = 60;
    constexpr int BASE_BOSS_ATK = 25;
    constexpr int BASE_BOSS_DEF = 12;
    constexpr float BOSS_DAMAGE_REDUCTION = 0.9f; // Resolute passive
    
    // Damage calculation
    constexpr float DAMAGE_VARIANCE_MIN = 0.9f;
    constexpr float DAMAGE_VARIANCE_MAX = 1.1f;
    constexpr float CRITICAL_MULTIPLIER = 1.5f;
    
    // Type effectiveness
    constexpr float TYPE_SUPER_EFFECTIVE = 1.5f;
    constexpr float TYPE_NOT_VERY_EFFECTIVE = 0.5f;
    constexpr float TYPE_NORMAL_EFFECTIVE = 1.0f;
    
    // Progression
    constexpr int EVOLUTION_LEVEL = 5;
    constexpr int UPGRADE_POINTS_PER_VICTORIES = 2;
    constexpr int VICTORIES_FOR_UPGRADE = 2;
    
    // Level up bonuses
    constexpr int ATK_PER_LEVEL = 2;
    constexpr int DEF_PER_LEVEL = 1;
    constexpr int HP_PER_LEVEL = 5;
    constexpr int MP_PER_LEVEL = 2;
    
    // Boss recovery after defeat
    constexpr float BOSS_DEFEAT_HP_RECOVERY = 0.5f;
    constexpr float BOSS_DEFEAT_MP_RECOVERY = 0.3f;
    
    // Tile distribution percentages (out of 100)
    constexpr int TILE_ENEMY_PERCENT = 30;
    constexpr int TILE_PIT_PERCENT = 15;
    constexpr int TILE_RIFT_PERCENT = 10;
    constexpr int TILE_ROCK_PERCENT = 15;
    constexpr int TILE_ARROW_PERCENT = 15;
    constexpr int TILE_EMPTY_PERCENT = 15;
    
    // File paths
    const char* const SAVE_FILE_PATH = "save.json";
    const char* const FONT_PATH = "assets/fonts/arial.ttf";
    
    // Audio paths
    const char* const BGM_MENU = "assets/audio/bgm_menu.ogg";
    const char* const BGM_MAP = "assets/audio/bgm_map.ogg";
    const char* const BGM_COMBAT = "assets/audio/bgm_combat.ogg";
    const char* const SFX_COIN = "assets/audio/sfx_coin.ogg";
    const char* const SFX_DICE = "assets/audio/sfx_dice.ogg";
    const char* const SFX_HIT = "assets/audio/sfx_hit.ogg";
}
