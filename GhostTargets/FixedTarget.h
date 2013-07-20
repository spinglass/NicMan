#include "IGhostTarget.h"

class FixedTarget : public IGhostTarget
{
public:
    explicit FixedTarget(GridRef const& target);

    virtual GridRef It() override;

private:
    FixedTarget(FixedTarget&);
    FixedTarget& operator=(FixedTarget&);

    GridRef m_Target;
};
