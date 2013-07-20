#include "IGhostTarget.h"

class Movement;

class InkyTarget : public IGhostTarget
{
public:
    explicit InkyTarget(Movement const& target, Movement const& partner);

    virtual GridRef It() override;

private:
    InkyTarget(InkyTarget&);
    InkyTarget& operator=(InkyTarget&);

    Movement const& m_Target;
    Movement const& m_Partner;
};
