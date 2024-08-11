#include "Animation.h"

Animation::Animation(const std::string& basePath, const std::string& filePrefix, const std::string& fileExtension, int frameCount, float frameRate)
    : currentFrame(0), framesCounter(0), fps(static_cast<int>(frameRate))
{
    for (int i = 0; i < frameCount; ++i) {
        std::string path = basePath + filePrefix + std::to_string(i) + fileExtension;
        Texture2D texture = LoadTexture(path.c_str());
        if (texture.id > 0) {
            frames.push_back(texture);
        }
    }
}

Animation::~Animation() {
    for (Texture2D& texture : frames) {
        UnloadTexture(texture);
    }
}

void Animation::Update() {
    framesCounter++;
    if (framesCounter >= (GetFPS() / fps)) {
        framesCounter = 0;
        currentFrame++;
        if (currentFrame >= frames.size()) {
            currentFrame = 0;
        }
    }
}

void Animation::Draw(Vector2 position, float scale, Color tint, bool flip) {
    if (!frames.empty()) {
        //position.x = position.x - frames[currentFrame].width*scale/2;
        //position.y = position.y - frames[currentFrame].height*scale/2;
        //DrawTextureEx(frames[currentFrame], position, 0.0f, scale, tint);

        Vector2 origin = { frames[currentFrame].width * scale/ 2.0f, 1500 * scale };
        Rectangle sourceRec = { 0.0f, 0.0f, static_cast<float>((flip ? 1 : -1) * frames[currentFrame].width), static_cast<float>(frames[currentFrame].height) };
        Rectangle destRec = { position.x , position.y, static_cast<float>(frames[currentFrame].width*scale), static_cast<float>(frames[currentFrame].height*scale)};
        DrawTexturePro(frames[currentFrame], sourceRec, destRec, origin, 0.0f, tint);
    }
}
