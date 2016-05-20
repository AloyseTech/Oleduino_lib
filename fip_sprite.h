#include <arduino.h>

class Sprite
{
public:
    int width;
    int height;
    const uint16_t* data;
};

class SpriteInst
{
public:
    const Sprite* sprite = 0;
    int x = 0;
    int y = 0;
    bool enabled = false;
    bool flip = false;
    uint8_t frames = 0;
    uint8_t currentFrame=0;
    void update(){
        currentFrame++;
        if(currentFrame>=frames)
            currentFrame=0;
    }
};

