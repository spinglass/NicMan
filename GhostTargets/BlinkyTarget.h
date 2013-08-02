#include "IGhostTarget.h"

class Movement;

class BlinkyTarget : public IGhostTarget
{
public:
    explicit BlinkyTarget(Movement const& target);

    virtual sf::Vector2i It() override;

private:
    BlinkyTarget(BlinkyTarget&);
    BlinkyTarget& operator=(BlinkyTarget&);

    Movement const& m_Target;
};
