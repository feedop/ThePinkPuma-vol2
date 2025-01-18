export module puma;

import std;
import glm;

import object;
import colors;

export class Puma : public Object
{
public:
    Puma() : Object(colors::transparentPink)
    {
    }

    virtual void setData(const std::array<glm::mat4, 6>& modelMatrices) = 0;
};