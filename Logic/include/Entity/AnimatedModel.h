#pragma once
#include <functional>
#include <Graphics\include\RenderInfo.h>
#include <Graphics\include\Utility\ModelLoader.h>

class AnimatedModel
{
public:
    AnimatedModel(Resources::Models::Files modelId, const char * start_animation);

    void update(float delta_ms);
    void render() const;

    void set_next(const char * animation, std::function<void(void)> start_callback = nullptr);
private:
    using Matrix = DirectX::SimpleMath::Matrix;
    using Vector3 = DirectX::SimpleMath::Vector3;

    HybrisLoader::Model * model;
    HybrisLoader::Skeleton * skeleton;

    NewAnimatedRenderInfo renderInfo;
    std::vector<Matrix> joint_transforms;

    float frame_progress;
    float frame_duration;
    float animation_progress;
    float animation_duration;

    std::string animation_current;
    std::string animation_next;
    std::function<void(void)> animation_callback;
};