#include "IGhostTarget.h"

class PinkyTarget : public IGhostTarget
{
public:
    explicit PinkyTarget(GridRef const& postion, Direction const& direction);

    virtual GridRef It() override;

private:
    PinkyTarget(PinkyTarget&);
    PinkyTarget& operator=(PinkyTarget&);

    GridRef const& m_Position;
    Direction const& m_Direction;
};
