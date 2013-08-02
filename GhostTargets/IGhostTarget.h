#pragma once

class IGhostTarget
{
public:
    virtual ~IGhostTarget() {}
    virtual sf::Vector2i It() = 0;
};
