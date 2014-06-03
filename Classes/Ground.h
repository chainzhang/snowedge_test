//
//  Ground.h
//  snowedge_test
//
//  Created by ChainZ on 2014/06/04.
//
//

#ifndef __GROUND_H__
#define __GROUND_H__

#include "cocos2d.h"
#include "Config.h"

class Ground : public cocos2d::Sprite
{
public:
    Ground();
    ~Ground();
    static Ground* createGround(const std::string &filename);
    
    CC_SYNTHESIZE(int, _type, Type);
    CREATE_FUNC(Ground);
};

#endif /* defined(__snowedge_test__Ground__) */
