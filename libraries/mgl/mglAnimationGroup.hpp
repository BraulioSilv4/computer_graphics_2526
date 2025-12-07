#ifndef MGL_ANIMATION_GRP_HPP
#define MGL_ANIMATION_GRP_HPP

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 

#include "mglAnimation.hpp"

namespace mgl {
    class AnimationGroup {
    private:
        std::vector<std::unique_ptr<mgl::Animation>> animations;

    public:
        AnimationGroup() = default;
        ~AnimationGroup() = default;
        
        void clearAnimations();
        void addAnimation(std::unique_ptr<mgl::Animation> anim);
        void play(double elapsedTime, bool rewind = false);

    };
}

#endif