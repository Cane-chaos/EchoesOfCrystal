#include "states/State.h"
#include "StateStack.h"
#include "core/AudioManager.h"
#include "core/AssetManager.h"
#include "core/RNG.h"
#include "core/SaveSystem.h"

State::Context::Context(sf::RenderWindow& window, AudioManager& audio, AssetManager& assets, 
                       RNG& rng, SaveSystem& save)
    : window(&window)
    , audio(&audio)
    , assets(&assets)
    , rng(&rng)
    , save(&save)
{
}

State::State(StateStack& stack, Context context)
    : m_stack(&stack)
    , m_context(context)
{
}

State::~State() {
}

void State::requestStackPush(StateID stateID) {
    m_stack->pushState(stateID);
}

void State::requestStackPop() {
    m_stack->popState();
}

void State::requestStateClear() {
    m_stack->clearStates();
}

State::Context State::getContext() const {
    return m_context;
}
