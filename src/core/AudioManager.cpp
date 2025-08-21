#include "core/AudioManager.h"
#include <iostream>

AudioManager::AudioManager()
    : m_masterVolume(100.0f)
    , m_musicVolume(50.0f)
    , m_soundVolume(75.0f)
    , m_muted(false)
{
}

AudioManager::~AudioManager() {
    stopMusic();
}

void AudioManager::playMusic(const std::string& filename, bool loop) {
    if (m_muted) return;
    
    if (!m_music.openFromFile(filename)) {
        std::cerr << "Failed to load music: " << filename << std::endl;
        return;
    }
    
    m_music.setLoop(loop);
    m_music.setVolume(m_musicVolume * m_masterVolume / 100.0f);
    m_music.play();
}

void AudioManager::stopMusic() {
    m_music.stop();
}

void AudioManager::pauseMusic() {
    m_music.pause();
}

void AudioManager::resumeMusic() {
    if (!m_muted) {
        m_music.play();
    }
}

void AudioManager::setMusicVolume(float volume) {
    m_musicVolume = std::max(0.0f, std::min(100.0f, volume));
    m_music.setVolume(m_musicVolume * m_masterVolume / 100.0f);
}

void AudioManager::playSound(const std::string& filename) {
    if (m_muted) return;
    
    loadSound(filename);
    
    auto found = m_soundBuffers.find(filename);
    if (found == m_soundBuffers.end()) {
        std::cerr << "Failed to find sound: " << filename << std::endl;
        return;
    }
    
    cleanupSounds();
    
    auto sound = std::make_unique<sf::Sound>();
    sound->setBuffer(found->second);
    sound->setVolume(m_soundVolume * m_masterVolume / 100.0f);
    sound->play();
    
    m_sounds.push_back(std::move(sound));
}

void AudioManager::setSoundVolume(float volume) {
    m_soundVolume = std::max(0.0f, std::min(100.0f, volume));
}

void AudioManager::setMasterVolume(float volume) {
    m_masterVolume = std::max(0.0f, std::min(100.0f, volume));
    m_music.setVolume(m_musicVolume * m_masterVolume / 100.0f);
}

float AudioManager::getMasterVolume() const {
    return m_masterVolume;
}

void AudioManager::setMuted(bool muted) {
    m_muted = muted;
    if (muted) {
        pauseMusic();
    } else {
        resumeMusic();
    }
}

bool AudioManager::isMuted() const {
    return m_muted;
}

void AudioManager::loadSound(const std::string& filename) {
    if (m_soundBuffers.find(filename) != m_soundBuffers.end()) {
        return; // Already loaded
    }
    
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename)) {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return;
    }
    
    m_soundBuffers[filename] = std::move(buffer);
}

void AudioManager::cleanupSounds() {
    m_sounds.erase(
        std::remove_if(m_sounds.begin(), m_sounds.end(),
            [](const std::unique_ptr<sf::Sound>& sound) {
                return sound->getStatus() == sf::Sound::Stopped;
            }),
        m_sounds.end()
    );
}
