//
//  GameOverScene.h
//  snowedge_test
//
//  Created by 張程 on 2014/06/04.
//
//

#ifndef __GAME_OVER_H__
#define __GAME_OVER_H__

#include "cocos2d.h"

class GameOver : public cocos2d::Layer
{
public:
    GameOver();
    ~GameOver();
    
    static cocos2d::Scene* createScene();
    virtual bool init();
    void reChallenge();
    
    CREATE_FUNC(GameOver);
};

#endif /* defined(__snowedge_test__GameOverScene__) */
