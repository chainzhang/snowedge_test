//
//  Player.h
//  snowedge_test
//
//  Created by 張程 on 2014/06/02.
//
//

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "Config.h"

USING_NS_CC;
USING_NS_CC_EXT;

class Player : public Sprite
{
public:
    Player();
    ~Player();
    
    static Player* createPlayer(const std::string &filename);
    
    CC_SYNTHESIZE(b2Body*, _body, B2Body);
    CC_SYNTHESIZE(bool, _on_ground, OnGround);
    CC_SYNTHESIZE(int, _type, Type);
    CREATE_FUNC(Player);

};

#endif