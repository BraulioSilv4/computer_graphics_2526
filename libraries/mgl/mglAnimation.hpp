#ifndef MGL_ANIMATION_HPP
#define MGL_ANIMATION_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp> 
#include "mglSceneNode.hpp"

namespace mgl {
    class Animation {
    private:
        // Represents the state of the animation 0 beginning -> 1 finished
        double state = 0.0f;

        // time in seconds for the animation to complete
        float duration;

        // Initial Animation values
        glm::vec3 itranslation;
        glm::vec3 iscaling;
        glm::quat iorientation;

        // Final Animation values
        glm::vec3 ftranslation;
        glm::vec3 fscaling;
        glm::quat forientation;

        // Animation target
        mgl::SceneNode* target;

    public:
        void play(double elapsedTime, bool rewind = false);

        Animation(
            mgl::SceneNode* _target,
            float _duration,
            glm::vec3 _translation,
            glm::vec3 _scaling,
            glm::quat _orientation
        ) {
            duration = _duration;
            target = _target;
            ftranslation = _translation;
            fscaling = _scaling;
            forientation = _orientation;
            itranslation = _target->getTranslation();
            iscaling = _target->getScale();
            iorientation = _target->getOrientation();
        }

        ~Animation() = default;
    };
}

#endif