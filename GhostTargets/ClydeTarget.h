#include "IGhostTarget.h"

class Movement;

class ClydeTarget : public IGhostTarget
{
public:
    explicit ClydeTarget(Movement const& target, Movement const& self, sf::Vector2i fleePosition);

    virtual sf::Vector2i It() override;

private:
    ClydeTarget(ClydeTarget&);
    ClydeTarget& operator=(ClydeTarget&);

    Movement const& m_Target;
    Movement const& m_Self;
    sf::Vector2i m_FleePosition;
};
