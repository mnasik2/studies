#pragma once

class Builder {
public:
    void HoldDoor() const {
        // Просто держит дверь 100 условных секунд
        int i = 100;
        while (0 != i) {
            i--;
        }
    }
};