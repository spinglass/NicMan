#include "IGhostTarget.h"

class InkyTarget : public IGhostTarget
{
public:
    explicit InkyTarget(GridRef const& postion);

    virtual GridRef It() override;

private:
    InkyTarget(InkyTarget&);
    InkyTarget& operator=(InkyTarget&);

    GridRef const& m_Position;
};
