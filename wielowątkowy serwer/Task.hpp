#pragma once
#include <string>

struct Task {
    int id;
    int priority;
    std::string data;
};

// komparator dla kolejki priorytetowej
struct TaskCompare {
    bool operator()(const Task& a, const Task& b) const {
        // wyższy priorytet ma większą wartość
        return a.priority < b.priority;
    }
};
