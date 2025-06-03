#pragma once
#include "Constants.h"

using Shape = std::array<std::array<int, BLOCK_SIZE>, BLOCK_SIZE>;

class Block {
public:
    Block();

    void initShape(int typeIdx, int rotations);
    void rotateCurrentShape(int times);
    int getShapeAmount() const;

    int x, y;
    int type;
    Shape shape;

private:

    static Shape rotatedShape(const Shape& src, int times);
    static constexpr std::array<Shape, 7> shapes = {{
        // I shape
        {{
            {0,0,0,0},
            {0,0,0,0},
            {1,1,1,1},
            {0,0,0,0}
        }},
        // J shape
        {{
            {0,0,0,0},
            {1,0,0,0},
            {1,1,1,0},
            {0,0,0,0}
        }},
        // L shape
        {{
            {0,0,0,0},
            {0,0,1,0},
            {1,1,1,0},
            {0,0,0,0}
        }},
        // O shape
        {{
            {0,0,0,0},
            {0,1,1,0},
            {0,1,1,0},
            {0,0,0,0}
        }},
        // S shape
        {{
            {0,0,0,0},
            {0,1,1,0},
            {1,1,0,0},
            {0,0,0,0}
        }},
        // T shape
        {{
            {0,0,0,0},
            {1,1,1,0},
            {0,1,0,0},
            {0,0,0,0}
        }},
        // Z shape
        {{
            {0,0,0,0},
            {1,1,0,0},
            {0,1,1,0},
            {0,0,0,0}
        }}
    }};
};