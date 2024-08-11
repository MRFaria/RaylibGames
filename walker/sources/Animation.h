#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include <vector>
#include <string>

class Animation {
public:
    Animation(const std::string& basePath, const std::string& filePrefix, const std::string& fileExtension, int frameCount, float frameRate);
    ~Animation();
    void Update();
    void Draw(Vector2 position, float scale, Color tint, bool flip);

private:
    std::vector<Texture2D> frames;
    int currentFrame;
    int framesCounter;
    int fps;
};

#endif // ANIMATION_H