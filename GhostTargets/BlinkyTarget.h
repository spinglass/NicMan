#include "IGhostTarget.h"

class BlinkyTarget : public IGhostTarget
{
public:
    explicit BlinkyTarget(GridRef const& player);

    virtual GridRef It() override;

private:
    BlinkyTarget(BlinkyTarget&);
    BlinkyTarget& operator=(BlinkyTarget&);

    GridRef const& m_Player;
};
