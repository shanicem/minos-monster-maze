#include "../include/characters/spider.hpp"
#include "../include/common.hpp"
#include "../include/physics.hpp"

// Put implementation for Spider enemies here

Texture Spider::spider_texture;

bool Spider::init(vec2 initialPosition, Physics * physicsHandler)
{
	Simple::init(initialPosition, physicsHandler);

    const char* textureFile = textures_path("spider-sprite-sheet.png");
	if (!RenderManager::load_texture(textureFile, &spider_texture, this)) return false;

	float spriteSheetWidth = 9.0f;
	float spriteSheetHeight = 3.0f;
    int horizontalTrim = 16;
    int verticalTrim = 19;

	spriteSheet.init(&spider_texture, { spriteSheetWidth, spriteSheetHeight }, this);

	spriteSheet.set_render_data(this, 0);

	initStateTree();
	set_properties(initialPosition, 3.0f, speed);
	m_frozen = false;
	set_dimensions(&spider_texture, spriteSheetWidth, spriteSheetHeight, horizontalTrim, verticalTrim);
	characterState->changeState(running);

	return true;
}

void Spider::draw(const mat3& projection)
{
    set_animation();
	RenderManager::draw(projection, m_position, m_rotation, m_scale, &spider_texture, this);

}

void Spider::set_animation()
{
    int numTiles;
    int tileIndex;
    bool isRepeat = true;

    float animSpeed = 0.2f;

    if (is_alive())
    {
        is_anim_once = false;
        switch (characterState->currentState) {
            case idle:
                numTiles = 5;
                tileIndex = 0;
                break;
            case running:
                numTiles = 6;
                tileIndex = 5;
                break;
            default:
                numTiles = 1;
                tileIndex = 0;

        }
    } else {
        isRepeat = false;

        if (is_anim_once)
        {
            numTiles = 1;
            tileIndex = 19;
        } else
        {
            numTiles = 9;
            tileIndex = 11;
        }
    }

    // Increment animation time
    m_animTime += animSpeed;

    // Apply animation
    tileIndex = tileIndex + (int)m_animTime % numTiles;

    // do not repeat death animation
    if (!isRepeat && tileIndex == 19) is_anim_once = true;

    spriteSheet.update_render_data(this, tileIndex);
}