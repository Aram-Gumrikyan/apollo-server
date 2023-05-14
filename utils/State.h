//
// Created by aram on 5/13/23.
//

#ifndef APOLLO_SERVER_STATE_H
#define APOLLO_SERVER_STATE_H

template<typename T>
class State {
private:
    T state;
public:
    explicit State(T initialState): state(initialState) {};

    void setState(T newState) {
        state = newState;
    };

    T getState() {
        return state;
    };
};


#endif //APOLLO_SERVER_STATE_H
