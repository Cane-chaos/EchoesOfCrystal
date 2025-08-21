#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Types.h"

// Forward declarations
class StateStack;
class AudioManager;
class AssetManager;
class RNG;
class SaveSystem;

class State {
public:
    using Ptr = std::unique_ptr<State>;
    
    struct Context {
        Context(sf::RenderWindow& window, AudioManager& audio, AssetManager& assets, 
                RNG& rng, SaveSystem& save);
        
        sf::RenderWindow* window;
        AudioManager* audio;
        AssetManager* assets;
        RNG* rng;
        SaveSystem* save;
    };
    
public:
    State(StateStack& stack, Context context);
    virtual ~State();
    
    virtual void draw() = 0;
    virtual bool update(sf::Time dt) = 0;
    virtual bool handleEvent(const sf::Event& event) = 0;
    
protected:
    void requestStackPush(StateID stateID);
    void requestStackPop();
    void requestStateClear();
    
    Context getContext() const;
    
private:
    StateStack* m_stack;
    Context m_context;
};
