#pragma once

#include "../common.hpp"
#include "../renderManager.hpp"
#include "menu.hpp"

class Credits : public Menu
{
    static Texture texture;
public:

    bool init(vec2 position);

    void draw(const mat3& projection)override;

    vec2 get_position();
};