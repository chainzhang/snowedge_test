//
//  newMainGameScene.h
//  snowedge_test
//
//  Created by 張程 on 2014/06/02.
//
//

#ifndef __NEW_MAIN_GAME_SCENE_H__
#define __NEW_MAIN_GAME_SCENE_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

#include "GLES-Render.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCCustomCommand.h"

USING_NS_CC;
USING_NS_CC_EXT;

class NewMainGame : public cocos2d::Layer
{
public:
    
    NewMainGame();
    ~NewMainGame();
    
    static Scene* createScene();
    void initPhyWorld();
    bool init();
    void update(float delta);
    void genGround(Vec2 pos);
    
protected:
    virtual void draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated) override;
    kmMat4 _modelViewMV;
    void onDraw();
    CustomCommand _customCommand;
    
private:
    b2World* _world;
    GLESDebugDraw* _debugDraw;
    
    Size _visibleSize;
    
    std::list<Sprite*>_grounds;
    Vec2 _last_ground_tail_pos;
};

#endif