#include "IGhostTarget.h"

class Movement;

class PinkyTarget : public IGhostTarget
{
public:
    explicit PinkyTarget(Movement const& target);

    virtual GridRef It() override;

private:
    PinkyTarget(PinkyTarget&);
    PinkyTarget& operator=(PinkyTarget&);

    Movement const& m_Target;
};
