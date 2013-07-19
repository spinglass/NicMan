#include "IGhostTarget.h"

class ClydeTarget : public IGhostTarget
{
public:
    explicit ClydeTarget(GridRef const& postion);

    virtual GridRef It() override;

private:
    ClydeTarget(ClydeTarget&);
    ClydeTarget& operator=(ClydeTarget&);

    GridRef const& m_Position;
};
