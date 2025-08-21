#include "core/AssetManager.h"
#include <iostream>

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
