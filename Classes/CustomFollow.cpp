//
//  CustomFollow.cpp
//  snowedge
//
//  Created by raharu on 2014/05/26.
//
//

#include "CustomFollow.h"

CustomFollow::CustomFollow(){
    _type = kCustomFollowNone;
    _marginRect = Rect::ZERO;
}

CustomFollow::~CustomFollow(){
    CC_SAFE_RELEASE(_followedNode);
}

CustomFollow* CustomFollow::create(Node* followNode){
    CustomFollow *follow = new CustomFollow();
    if (follow && follow->initWithTarget(followNode, Rect::ZERO))
    {
        follow->autorelease();
        return follow;
    }
    CC_SAFE_DELETE(follow);
    return nullptr;
}

CustomFollow* CustomFollow::create(Node* followNode, CustomFollowType type){
    CustomFollow *follow = new CustomFollow();
    follow->_type = type;
    if (follow && follow->initWithTarget(followNode, Rect::ZERO))
    {
        follow->autorelease();
        return follow;
    }
    CC_SAFE_DELETE(follow);
    return nullptr;
}


//センターの変更する
CustomFollow* CustomFollow::create(Node* followNode,  Point center){
    CustomFollow *follow = new CustomFollow();
    follow->_center_position = center;
    if (follow && follow->initWithTarget(followNode, Rect::ZERO))
    {
        follow->autorelease();
        return follow;
    }
    CC_SAFE_DELETE(follow);
    return nullptr;
}

CustomFollow* CustomFollow::create(Node* followNode, Rect marginRect){
    CustomFollow *follow = new CustomFollow();
    follow->_marginRect = marginRect;
    if (follow && follow->initWithTarget(followNode, Rect::ZERO))
    {
        follow->autorelease();
        return follow;
    }
    CC_SAFE_DELETE(follow);
    return nullptr;
}


//フレーム処理
void CustomFollow::step(float dt)
{
    CC_UNUSED_PARAM(dt);
    
    if(_boundarySet)
    {
        if(_boundaryFullyCovered)
            return;
        
        Point tempPos = _halfScreenSize - _followedNode->getPosition();
        
        float x = clampf(tempPos.x, _leftBoundary, _rightBoundary);
        float y = clampf(tempPos.y, _bottomBoundary, _topBoundary);
        
        if(_type == kCustomFollowXOnly){
            y = _target->getPositionY();
        }
        else if (_type == kCustomFollowYOnly){
            x = _target->getPositionX();
        }
        
        //センターポジションの移動
        if(_center_position != Point::ZERO){
            y = y + _center_position.y;
            x = x + _center_position.x;
        }
        
        _target->setPosition(Point(x , y));
        
    }
    else
    {
        Point calcPos = _target->convertToWorldSpace(_followedNode->getPosition());
        
        if(_marginRect.containsPoint(calcPos)){
            return;
        }
        
        float x = _halfScreenSize.x - _followedNode->getPositionX();
        float y = _halfScreenSize.y - _followedNode->getPositionY();
        
        if(_type == kCustomFollowXOnly){
            y = _target->getPositionY();
        }
        else if (_type == kCustomFollowYOnly){
            x = _target->getPositionX();
        }
        
        //センターポジションの移動
        if(_center_position != Point::ZERO){
            y = y + _center_position.y;
            x = x + _center_position.x;
        }
        
        _target->setPosition(Point(x , y));
    }
}