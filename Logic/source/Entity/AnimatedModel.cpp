#include "../../include/Entity/AnimatedModel.h"
#include <Graphics\include\RenderQueue.h>

AnimatedModel::AnimatedModel(Resources::Models::Files modelId, const char * start_animation)
{
    model = ModelLoader::get().getModel(modelId);
    skeleton = &model->getSkeleton();

    renderInfo.model = modelId;
    renderInfo.color = Vector3(1,1,1);
    renderInfo.cull_radius = model->get_bounding_box().sphere_radius();
    renderInfo.useGridTexture = false;
    renderInfo.joint_transforms = &joint_transforms;

    frame_progress = 0.0f;
    frame_duration = 1.0f / 30.0f;

    animation_next     = "";
    animation_current  = start_animation;
    animation_duration = skeleton->getAnimationDuration(animation_current.c_str());
}

void AnimatedModel::update(float delta_ms)
{
    frame_progress += delta_ms / 1000.0f;

    if (frame_progress < frame_duration) return;

    frame_progress     -= frame_duration;
    animation_progress += frame_duration;
    joint_transforms    = skeleton->evalAnimation(animation_current.c_str(), animation_progress);

    if (animation_progress < animation_duration) return;

    frame_progress     = 0.0f;
    animation_progress = 0.0f;

    if (animation_next.length() == 0) return;

    animation_current  = animation_next;
    animation_duration = skeleton->getAnimationDuration(animation_next.c_str());    
    animation_next     = "";

    if (animation_callback == nullptr) return;
    
    animation_callback();
    animation_callback = nullptr;
}

void AnimatedModel::render() const
{
    QueueRender(renderInfo);
}

void AnimatedModel::set_next(const char * animation, std::function<void(void)> start_callback)
{
    animation_next = animation;
    animation_callback = start_callback;
}