#include "Player.h"

Player::Player(float screenWidth, float screenHeight)   
    : _charIdleAnimation(ASSETS_PATH, "char/idle_", ".png", 6, 10.0f)
{ 
    _position = {screenWidth / 2.0f, screenHeight / 2.0f};
    _velocity = {0,0};
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
}

Player::~Player()   
{ 
    _charIdleAnimation.~Animation();
}

void Player::Update()
{
    _charIdleAnimation.Update();
    // Character movement
    _velocity = {0,0};
    if (IsKeyDown(KEY_RIGHT) and _position.x <= _screenWidth) 
        _velocity.x = _speed;
    if (IsKeyDown(KEY_LEFT) and _position.x >= 0) 
        _velocity.x = -_speed;
    if (IsKeyDown(KEY_UP) and _position.y >= 0) 
        _velocity.y = -_speed;
    if (IsKeyDown(KEY_DOWN) and _position.y <= _screenHeight/2) 
        _velocity.y = _speed;

    _position.x = _position.x + _velocity.x * GetFrameTime();
    _position.y = _position.y + _velocity.y * GetFrameTime();
    printf("Position.y is %lf\n", _position.y);
}

void Player::Draw()
{
    // Draw the animation at the center of the screen with scaling
    if (_velocity.x > 0 )
        flip = true;
    else if(_velocity.x < 0 )
        flip = false;
    _charIdleAnimation.Draw(_position, _scale, WHITE, flip);        
}
