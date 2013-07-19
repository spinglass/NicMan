#include "IGhostTarget.h"

class ClydeTarget : public IGhostTarget
{
public:
    explicit ClydeTarget(GridRef const& postion, GridRef const& ownPostion, GridRef const& fleePosition);

    virtual GridRef It() override;

private:
    ClydeTarget(ClydeTarget&);
    ClydeTarget& operator=(ClydeTarget&);

    GridRef const& m_Position;
    GridRef const& m_OwnPosition;
    GridRef m_FleePosition;
};
