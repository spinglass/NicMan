#include "IGhostTarget.h"

class InkyTarget : public IGhostTarget
{
public:
    explicit InkyTarget(GridRef const& postion, Direction const& direction, GridRef const& otherPosition);

    virtual GridRef It() override;

private:
    InkyTarget(InkyTarget&);
    InkyTarget& operator=(InkyTarget&);

    GridRef const& m_Position;
    Direction const& m_Direction;
    GridRef const& m_OtherPosition;
};
