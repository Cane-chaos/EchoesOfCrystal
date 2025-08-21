#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();
    
    // Music management
    void playMusic(const std::string& filename, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(float volume);
    
    // Sound effects
    void playSound(const std::string& filename);
    void setSoundVolume(float volume);
    
    // Global volume
    void setMasterVolume(float volume);
    float getMasterVolume() const;
    
    // Mute functionality
    void setMuted(bool muted);
    bool isMuted() const;
    
private:
    void loadSound(const std::string& filename);
    
private:
    sf::Music m_music;
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::vector<std::unique_ptr<sf::Sound>> m_sounds;
    
    float m_masterVolume;
    float m_musicVolume;
    float m_soundVolume;
    bool m_muted;
    
    // Clean up finished sounds
    void cleanupSounds();
};
