#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Types.h"

class SaveSystem {
public:
    SaveSystem();
    ~SaveSystem();
    
    // Save/Load operations
    bool saveGame(const SaveData& data, const std::string& filename = "");
    bool loadGame(SaveData& data, const std::string& filename = "");
    
    // File management
    bool saveExists(const std::string& filename = "") const;
    bool deleteSave(const std::string& filename = "");
    
    // JSON conversion
    nlohmann::json saveDataToJson(const SaveData& data) const;
    SaveData jsonToSaveData(const nlohmann::json& json) const;
    
    // Settings
    void setDefaultSaveFile(const std::string& filename);
    const std::string& getDefaultSaveFile() const;
    
private:
    std::string m_defaultSaveFile;
    
    std::string getFullPath(const std::string& filename) const;
};
