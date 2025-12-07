#include "mglAnimationGroup.hpp"

namespace mgl {
	void AnimationGroup::addAnimation(std::unique_ptr<mgl::Animation> anim) {
		animations.push_back(std::move(anim));
	}

	void AnimationGroup::clearAnimations() {
		animations.clear();
	}

	void AnimationGroup::play(double elapsedTime, bool rewind) {
		for (auto const& animation : animations) {
			animation->play(elapsedTime, rewind);
		}
	}

}