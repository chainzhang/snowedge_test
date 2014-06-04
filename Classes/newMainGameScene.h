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

class Player;

class NewMainGame : public cocos2d::Layer, public b2ContactListener
{
public:
    
    NewMainGame();
    ~NewMainGame();
    
    static Scene* createScene();
    void initPhyWorld();
    bool init();
    void initUserControl();
    void update(float delta);
    void genGround();
    void GameOver();
    
protected:
    virtual void draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated) override;
    kmMat4 _modelViewMV;
    void onDraw();
    CustomCommand _customCommand;
    
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    
    CREATE_FUNC(NewMainGame);
    
private:
    int _game_state;
    
    
    b2World* _world;
    GLESDebugDraw* _debugDraw;
    Sprite *_camera;
    int *_map;
    int _map_size;
    int _map_now;
    
    Size _visibleSize;
    
    std::list<b2Body*>_grounds;
    Vec2 _last_ground_tail_pos;
    
    
    
    Player* _player;
    b2Body* _player_body;
    int _ground_passed;
    bool _jump_available;
    
    EventListenerTouchOneByOne *_one_by_one_listener;
    EventListenerTouchAllAtOnce *_all_at_once_listener;
};

#endif