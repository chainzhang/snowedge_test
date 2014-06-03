//
//  CustomFollow.h
//  snowedge
//
//  Created by raharu on 2014/05/26.
//
//

#ifndef __snowedge__CustomFollow__
#define __snowedge__CustomFollow__

#include <iostream>
#include "cocos2d.h"

USING_NS_CC;

enum CustomFollowType{
    kCustomFollowNone,
    kCustomFollowXOnly,
    kCustomFollowYOnly
};

class CustomFollow : public Follow
{
private:
    CustomFollowType _type;
    Point _center_position = Point::ZERO;
    Rect _marginRect;
    
    CustomFollow();
    ~CustomFollow();
public:
    static CustomFollow* create(Node *followedNode);
    static CustomFollow* create(Node *followedNode, CustomFollowType type);
    static CustomFollow* create(Node *followedNode, Point _center_position);
    static CustomFollow* create(Node *followedNode, Rect marginRect);
    
    virtual void step(float dt) override;
};

#endif /* defined(__snowedge__CustomFollow__) */
