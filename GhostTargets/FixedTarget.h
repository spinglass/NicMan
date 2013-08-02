#include "IGhostTarget.h"

class FixedTarget : public IGhostTarget
{
public:
    explicit FixedTarget(sf::Vector2i target);

    virtual sf::Vector2i It() override;

private:
    FixedTarget(FixedTarget&);
    FixedTarget& operator=(FixedTarget&);

    sf::Vector2i m_Target;
};
