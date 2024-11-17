#include "Entity.h"

class Player : public Entity
{
    public:
        Player(Level &level);
        ~Player();
        void Update();

};