#include "IGhostTarget.h"

class Movement;

class ClydeTarget : public IGhostTarget
{
public:
    explicit ClydeTarget(Movement const& target, Movement const& self, GridRef const& fleePosition);

    virtual GridRef It() override;

private:
    ClydeTarget(ClydeTarget&);
    ClydeTarget& operator=(ClydeTarget&);

    Movement const& m_Target;
    Movement const& m_Self;
    GridRef m_FleePosition;
};
