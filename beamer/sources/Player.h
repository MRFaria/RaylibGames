#include "Entity.h"

class Player : public Entity
{
    public:
        Player();
        ~Player();
        void Update(Level &level);
};