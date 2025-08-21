#include "core/SaveSystem.h"
#include "Constants.h"
#include <fstream>
#include <iostream>

SaveSystem::SaveSystem()
    : m_defaultSaveFile(Constants::SAVE_FILE_PATH)
{
}

SaveSystem::~SaveSystem() {
}

bool SaveSystem::saveGame(const SaveData& data, const std::string& filename) {
    std::string path = getFullPath(filename.empty() ? m_defaultSaveFile : filename);
    
    try {
        nlohmann::json json = saveDataToJson(data);
        
        std::ofstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open save file for writing: " << path << std::endl;
            return false;
        }
        
        file << json.dump(4);
        file.close();
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving game: " << e.what() << std::endl;
        return false;
    }
}

bool SaveSystem::loadGame(SaveData& data, const std::string& filename) {
    std::string path = getFullPath(filename.empty() ? m_defaultSaveFile : filename);
    
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open save file for reading: " << path << std::endl;
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        file.close();
        
        data = jsonToSaveData(json);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        return false;
    }
}

bool SaveSystem::saveExists(const std::string& filename) const {
    std::string path = getFullPath(filename.empty() ? m_defaultSaveFile : filename);
    std::ifstream file(path);
    return file.good();
}

bool SaveSystem::deleteSave(const std::string& filename) {
    std::string path = getFullPath(filename.empty() ? m_defaultSaveFile : filename);
    return std::remove(path.c_str()) == 0;
}

nlohmann::json SaveSystem::saveDataToJson(const SaveData& data) const {
    nlohmann::json json;
    
    json["mapSeed"] = data.mapSeed;
    json["position"] = {data.position.x, data.position.y};
    json["direction"] = static_cast<int>(data.direction);
    json["level"] = data.level;
    json["baseStats"] = {
        {"hp", data.baseStats.hp},
        {"mp", data.baseStats.mp},
        {"atk", data.baseStats.atk},
        {"def", data.baseStats.def},
        {"crit", data.baseStats.crit},
        {"evade", data.baseStats.evade}
    };
    json["currentHP"] = data.currentHP;
    json["currentMP"] = data.currentMP;
    json["skillCooldowns"] = data.skillCooldowns;
    json["victories"] = data.victories;
    json["rngSeed"] = data.rngSeed;
    json["pokemonType"] = static_cast<int>(data.pokemonType);
    json["upgradePoints"] = data.upgradePoints;
    
    return json;
}

SaveData SaveSystem::jsonToSaveData(const nlohmann::json& json) const {
    SaveData data;
    
    data.mapSeed = json["mapSeed"];
    data.position = Vec2i(json["position"][0], json["position"][1]);
    data.direction = static_cast<Direction>(json["direction"]);
    data.level = json["level"];
    
    auto stats = json["baseStats"];
    data.baseStats = Stats(
        stats["hp"], stats["mp"], stats["atk"], stats["def"],
        stats["crit"], stats["evade"]
    );
    
    data.currentHP = json["currentHP"];
    data.currentMP = json["currentMP"];
    data.skillCooldowns = json["skillCooldowns"].get<std::vector<int>>();
    data.victories = json["victories"];
    data.rngSeed = json["rngSeed"];
    data.pokemonType = static_cast<PokemonType>(json["pokemonType"]);
    data.upgradePoints = json["upgradePoints"];
    
    return data;
}

void SaveSystem::setDefaultSaveFile(const std::string& filename) {
    m_defaultSaveFile = filename;
}

const std::string& SaveSystem::getDefaultSaveFile() const {
    return m_defaultSaveFile;
}

std::string SaveSystem::getFullPath(const std::string& filename) const {
    return filename; // For now, just return the filename as-is
}
