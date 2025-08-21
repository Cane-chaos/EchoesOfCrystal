#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include "states/State.h"
#include "Types.h"

class StateStack : private sf::NonCopyable {
public:
    enum Action {
        Push,
        Pop,
        Clear
    };
    
public:
    explicit StateStack(State::Context context);
    
    template <typename T>
    void registerState(StateID stateID);
    
    void update(sf::Time dt);
    void draw();
    void handleEvent(const sf::Event& event);
    
    void pushState(StateID stateID);
    void popState();
    void clearStates();
    
    bool isEmpty() const;
    
private:
    State::Ptr createState(StateID stateID);
    void applyPendingChanges();
    
private:
    struct PendingChange {
        Action action;
        StateID stateID;
    };
    
private:
    std::vector<State::Ptr> m_stack;
    std::vector<PendingChange> m_pendingList;
    
    State::Context m_context;
    std::map<StateID, std::function<State::Ptr()>> m_factories;
};

template <typename T>
void StateStack::registerState(StateID stateID) {
    m_factories[stateID] = [this]() {
        return State::Ptr(new T(*this, m_context));
    };
}
