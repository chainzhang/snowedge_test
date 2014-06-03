//
//  Ground.cpp
//  snowedge_test
//
//  Created by ChainZ on 2014/06/04.
//
//

#include "Ground.h"

USING_NS_CC;

Ground::Ground():
_type(Config::GROUND)
{
}

Ground::~Ground()
{
}

Ground* Ground::createGround(const std::string &filename)
{
    auto ground = new Ground();
    if (ground && ground->initWithFile(filename))
    {
        ground->autorelease();
        return ground;
    }
    delete ground;
    ground = NULL;
    return NULL;
}
