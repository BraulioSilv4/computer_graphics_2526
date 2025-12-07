#include "mglAnimation.hpp"
#include <iostream>
#include <algorithm>

namespace mgl {

void Animation::play(double elapsedTime, bool rewind) {
    state += (rewind ? -1.0f : 1.0f) * (elapsedTime / duration); 
    // Clamp only since c++17 (project is c++14)
    state = state < 0.0 ? 0.0 : state > 1.0 ? 1.0 : state;
    std::cout << "State " << state << std::endl;

    glm::vec3 pos = glm::mix(itranslation, ftranslation, state);
    glm::vec3 scale = glm::mix(iscaling, fscaling, state);
    glm::quat ori = glm::slerp(iorientation, forientation, static_cast<float>(state));

    target->setPosition(pos);
    target->setScale(scale);
    target->setRotation(ori);
}

}
