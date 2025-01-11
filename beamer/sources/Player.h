#include "Entity.h"

class Player : public Entity
{
    public:
        void Draw(Camera2D camera);
        Player(Level &level);
        ~Player();
        void Update(std::vector<Entity> &entities);

};