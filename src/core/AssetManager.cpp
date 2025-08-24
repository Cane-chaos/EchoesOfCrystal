#include "core/AssetManager.h"
#include <iostream>
#include <algorithm>

AssetManager::AssetManager() {
    createDefaultAssets();
}

AssetManager::~AssetManager() {
    clear();
}

const sf::Font& AssetManager::getFont(const std::string& filename) {
    auto found = m_fonts.find(filename);
    if (found != m_fonts.end()) {
        return *found->second;
    }

    // Try to load the font
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(filename)) {
        std::cerr << "Failed to load font: " << filename << std::endl;
        return getDefaultFont();
    }

    auto& fontRef = *font;
    m_fonts[filename] = std::move(font);
    return fontRef;
}

void AssetManager::loadFont(const std::string& id, const std::string& filename) {
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(filename)) {
        std::cerr << "Failed to load font: " << filename << std::endl;
        return;
    }

    m_fonts[id] = std::move(font);
}

const sf::Texture& AssetManager::getTexture(const std::string& filename) {
    auto found = m_textures.find(filename);
    if (found != m_textures.end()) {
        return *found->second;
    }

    // Try to load the texture
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return getDefaultTexture();
    }

    auto& textureRef = *texture;
    m_textures[filename] = std::move(texture);
    return textureRef;
}

void AssetManager::loadTexture(const std::string& id, const std::string& filename) {
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(filename)) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return;
    }

    m_textures[id] = std::move(texture);
}

const sf::Font& AssetManager::getDefaultFont() {
    return *m_defaultFont;
}

sf::Texture& AssetManager::getDefaultTexture() {
    return *m_defaultTexture;
}

void AssetManager::clear() {
    m_fonts.clear();
    m_textures.clear();
}

void AssetManager::createDefaultAssets() {
    // Create default font - try to load a system font, fallback to empty font
    m_defaultFont = std::make_unique<sf::Font>();
    // Try common system font paths
    if (!m_defaultFont->loadFromFile("C:/Windows/Fonts/arial.ttf") &&
        !m_defaultFont->loadFromFile("/System/Library/Fonts/Arial.ttf") &&
        !m_defaultFont->loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // If no system font found, create an empty font (text won't render but won't crash)
        std::cerr << "Warning: No default font found. Text may not render correctly." << std::endl;
    }

    // Create default texture (white 1x1 pixel)
    m_defaultTexture = std::make_unique<sf::Texture>();
    sf::Image whitePixel;
    whitePixel.create(1, 1, sf::Color::White);
    m_defaultTexture->loadFromImage(whitePixel);
}
// Game asset loading implementation
void AssetManager::loadAll() {
    // Load map textures (normal loading)
    loadGameTexture("player_map",      "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\pikachu.png");
    loadGameTexture("tile_rock",       "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\rock.png");
    loadGameTexture("tile_portal",     "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\warp.png");

    // Load combat sprites with transparency (remove white background)
    loadTransparentTexture("player_combat",   "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\pikachu.png");
    loadTransparentTexture("monster_bisasam", "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\bisasam.png");
    loadTransparentTexture("monster_chalamander","C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\chalamander.png");
    loadTransparentTexture("monster_boss",    "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\boss.png");

    // Combat background
    loadGameTexture("bg_combat",       "C:\\Users\\Admin\\Downloads\\PROJECT\\assets\\icon\\background.png");

    // Load fonts
    loadGameFont("arial",              "C:\\Windows\\Fonts\\arial.ttf");
}

void AssetManager::loadGameTexture(const std::string& key, const std::string& path) {
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(path)) {
        texture->setSmooth(true);
        m_gameTextures[key] = std::move(texture);
        std::cout << "Loaded texture: " << key << " from " << path << std::endl;
    } else {
        std::cerr << "Warning: Failed to load texture " << key << " from " << path << std::endl;
        // Create placeholder texture
        auto placeholder = std::make_unique<sf::Texture>();
        sf::Image placeholderImage;
        placeholderImage.create(64, 64, sf::Color::Magenta);  // Magenta placeholder
        placeholder->loadFromImage(placeholderImage);
        m_gameTextures[key] = std::move(placeholder);
    }
}

const sf::Texture& AssetManager::getGameTexture(const std::string& key) {
    auto it = m_gameTextures.find(key);
    if (it != m_gameTextures.end()) {
        return *it->second;
    }

    std::cerr << "Warning: Game texture not found: " << key << std::endl;
    return getPlaceholderTexture();
}

bool AssetManager::hasGameTexture(const std::string& key) const {
    return m_gameTextures.find(key) != m_gameTextures.end();
}

sf::Sprite AssetManager::makeSprite(const std::string& key) const {
    sf::Sprite sprite;
    auto it = m_gameTextures.find(key);
    if (it != m_gameTextures.end()) {
        sprite.setTexture(*it->second);
    } else {
        sprite.setTexture(const_cast<AssetManager*>(this)->getPlaceholderTexture());
    }
    return sprite;
}

sf::Sprite AssetManager::makeSprite(const std::string& key, float scaleToWidth, float scaleToHeight) const {
    sf::Sprite sprite = makeSprite(key);

    // Calculate scale to fit within target size while maintaining aspect ratio
    sf::Vector2u textureSize = sprite.getTexture()->getSize();
    float scaleX = scaleToWidth / textureSize.x;
    float scaleY = scaleToHeight / textureSize.y;
    float scale = std::min(scaleX, scaleY);  // Use smaller scale to maintain aspect ratio

    sprite.setScale(scale, scale);
    return sprite;
}

sf::Texture& AssetManager::getPlaceholderTexture() {
    // Create a simple placeholder if not exists
    static sf::Texture placeholder;
    static bool created = false;

    if (!created) {
        sf::Image placeholderImage;
        placeholderImage.create(32, 32, sf::Color::Red);
        placeholder.loadFromImage(placeholderImage);
        created = true;
    }

    return placeholder;
}

// Combat sprite helpers implementation
sf::Sprite AssetManager::makeFullScreenBackground(const std::string& key, const sf::Vector2u& winSize) const {
    sf::Sprite sprite = makeSprite(key);
    const sf::Texture* tex = sprite.getTexture();
    if (!tex) return sprite;

    auto tw = tex->getSize().x, th = tex->getSize().y;
    float sx = static_cast<float>(winSize.x) / tw;
    float sy = static_cast<float>(winSize.y) / th;
    float scale = std::max(sx, sy); // cover (fill screen)
    sprite.setScale(scale, scale);

    // Center the background
    float w = tw * scale, h = th * scale;
    sprite.setPosition((winSize.x - w) / 2.f, (winSize.y - h) / 2.f);
    return sprite;
}

void AssetManager::scaleToHeight(sf::Sprite& sprite, float targetHeight) const {
    auto bounds = sprite.getLocalBounds();
    if (bounds.height > 0) {
        float scale = targetHeight / bounds.height;
        sprite.setScale(scale, scale);
    }
}

void AssetManager::placeBottomLeft(sf::Sprite& sprite, sf::Vector2f anchorBL) const {
    auto bounds = sprite.getLocalBounds();
    sprite.setPosition(anchorBL.x - bounds.left, anchorBL.y - (bounds.top + bounds.height));
}

void AssetManager::placeTopRight(sf::Sprite& sprite, sf::Vector2f anchorTR) const {
    auto bounds = sprite.getLocalBounds();
    sprite.setPosition(anchorTR.x - (bounds.left + bounds.width), anchorTR.y - bounds.top);
}

void AssetManager::loadGameFont(const std::string& key, const std::string& path) {
    auto font = std::make_unique<sf::Font>();
    if (font->loadFromFile(path)) {
        m_gameFonts[key] = std::move(font);
        std::cout << "Loaded font: " << key << " from " << path << std::endl;
    } else {
        std::cerr << "Warning: Failed to load font " << key << " from " << path << std::endl;
    }
}

const sf::Font& AssetManager::getGameFont(const std::string& key) {
    auto it = m_gameFonts.find(key);
    if (it != m_gameFonts.end()) {
        return *it->second;
    }

    std::cerr << "Warning: Game font not found: " << key << ", using default" << std::endl;
    return getDefaultFont();
}

bool AssetManager::hasGameFont(const std::string& key) const {
    return m_gameFonts.find(key) != m_gameFonts.end();
}

void AssetManager::loadTransparentTexture(const std::string& key, const std::string& path) {
    sf::Image image;
    if (image.loadFromFile(path)) {
        // Create mask from white color to make it transparent
        image.createMaskFromColor(sf::Color::White, 0);

        auto texture = std::make_unique<sf::Texture>();
        if (texture->loadFromImage(image)) {
            texture->setSmooth(true);
            m_gameTextures[key] = std::move(texture);
            std::cout << "Loaded transparent texture: " << key << " from " << path << std::endl;
        } else {
            std::cerr << "Warning: Failed to create texture from image " << key << std::endl;
            // Create placeholder
            auto placeholder = std::make_unique<sf::Texture>();
            sf::Image placeholderImage;
            placeholderImage.create(64, 64, sf::Color::Magenta);
            placeholder->loadFromImage(placeholderImage);
            m_gameTextures[key] = std::move(placeholder);
        }
    } else {
        std::cerr << "Warning: Failed to load transparent texture " << key << " from " << path << std::endl;
        // Create placeholder
        auto placeholder = std::make_unique<sf::Texture>();
        sf::Image placeholderImage;
        placeholderImage.create(64, 64, sf::Color::Magenta);
        placeholder->loadFromImage(placeholderImage);
        m_gameTextures[key] = std::move(placeholder);
    }
}
