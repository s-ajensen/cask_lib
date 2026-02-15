#pragma once

template<typename ValueType>
struct Interpolated {
    ValueType previous;
    ValueType current;

    void advance() {
        previous = current;
    }
};
