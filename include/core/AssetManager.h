#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>

class AssetManager {
public:
    AssetManager();
    ~AssetManager();
    
    // Font management
    const sf::Font& getFont(const std::string& filename);
    void loadFont(const std::string& id, const std::string& filename);
    
    // Texture management
    const sf::Texture& getTexture(const std::string& filename);
    void loadTexture(const std::string& id, const std::string& filename);

    // Game asset loading
    void loadAll();
    void loadGameTexture(const std::string& key, const std::string& path);
    void loadTransparentTexture(const std::string& key, const std::string& path);
    void loadGameFont(const std::string& key, const std::string& path);
    const sf::Texture& getGameTexture(const std::string& key);
    const sf::Font& getGameFont(const std::string& key);
    bool hasGameTexture(const std::string& key) const;
    bool hasGameFont(const std::string& key) const;

    // Sprite helpers
    sf::Sprite makeSprite(const std::string& key) const;
    sf::Sprite makeSprite(const std::string& key, float scaleToWidth, float scaleToHeight) const;

    // Combat sprite helpers
    sf::Sprite makeFullScreenBackground(const std::string& key, const sf::Vector2u& winSize) const;
    void scaleToHeight(sf::Sprite& sprite, float targetHeight) const;
    void placeBottomLeft(sf::Sprite& sprite, sf::Vector2f anchorBL) const;
    void placeTopRight(sf::Sprite& sprite, sf::Vector2f anchorTR) const;

    // Default assets
    const sf::Font& getDefaultFont();
    sf::Texture& getDefaultTexture();

    // Cleanup
    void clear();
    
private:
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::map<std::string, std::unique_ptr<sf::Texture>> m_textures;

    // Game textures with key-based access
    std::map<std::string, std::unique_ptr<sf::Texture>> m_gameTextures;
    std::map<std::string, std::unique_ptr<sf::Font>> m_gameFonts;

    // Default assets
    std::unique_ptr<sf::Font> m_defaultFont;
    std::unique_ptr<sf::Texture> m_defaultTexture;

    void createDefaultAssets();
    sf::Texture& getPlaceholderTexture();
};
