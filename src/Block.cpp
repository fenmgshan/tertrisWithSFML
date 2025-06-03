#include "Block.h"
#include "Constants.h"

Block::Block () : x(WIDTH/2 - BLOCK_SIZE/2), y(0), type(0) {}

void Block::initShape(int typeIdx, int rotations) {
    this->type = typeIdx;
    if (typeIdx < 0 || typeIdx >= static_cast<int>(shapes.size())) {
        typeIdx = 0;
        this->type = 0;
    }
    Shape final_roatated_shape = rotatedShape(shapes[typeIdx], rotations);
    this->shape = final_roatated_shape;
}

void Block::rotateCurrentShape(int times) {
    Shape rotated = rotatedShape(this->shape, times);
    this->shape = rotated;
}

int Block::getShapeAmount() const {
    return static_cast<int>(shapes.size());
}

// 旋转
Shape Block::rotatedShape(const Shape& src, int times) {
    Shape result_of_one_rotation;
    Shape current_shape_to_rotate = src;
    times = times % 4;
    if (times == 0) {
        return src;
    }

    for (auto& row : result_of_one_rotation) {
        row.fill(0);
    }

    for (int r=0; r<times; r++) {
        for (int i=0; i<BLOCK_SIZE; i++) {
            for (int j=0; j<BLOCK_SIZE; j++) {
                result_of_one_rotation[j][BLOCK_SIZE-1-i] = current_shape_to_rotate[i][j];
            }
        }
        current_shape_to_rotate = result_of_one_rotation;
    }
    return current_shape_to_rotate;
}
