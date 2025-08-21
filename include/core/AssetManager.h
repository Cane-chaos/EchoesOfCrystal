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
    
    // Default assets
    const sf::Font& getDefaultFont();
    sf::Texture& getDefaultTexture();
    
    // Cleanup
    void clear();
    
private:
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    
    // Default assets
    std::unique_ptr<sf::Font> m_defaultFont;
    std::unique_ptr<sf::Texture> m_defaultTexture;
    
    void createDefaultAssets();
};
