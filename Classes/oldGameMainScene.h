//
//  GameMainScene.h
//  snowedge_test
//
//  Created by 張程 on 2014/05/27.
//
//

#ifndef __snowedge_test__GameMainScene__
#define __snowedge_test__GameMainScene__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

#include "GLES-Render.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCCustomCommand.h"

USING_NS_CC;
USING_NS_CC_EXT;

class GameMain : public Layer, public b2ContactListener
{
public:
    GameMain();
    virtual ~GameMain();
    
    
    static Scene* createScene(const int level);
    virtual bool init();
    
    
    void initPhyWorld();
    void initStage();
    void initUserControl();
    void update(float delta);
    
    virtual void draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated) override;
    void createPolygonB2Body(const Vec2 *points, int const count);
    void createRectB2Body(const Vec2 top_left_point, const float width, const float height);
    
    bool tricksCollision(Sprite *_player);
    void createJumpTricks(const Vec2 top_left_point, const float width, const float height);
    void updatePlayerVector();
    
    void createWarningArea(const Vec2 top_left_point, const float width, const float height);
    bool enterWarningArea(const Sprite* _player);
    
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    
    
    b2Vec2 toB2Vec2(Vec2 vec2);
    
    CREATE_FUNC(GameMain);
    
protected:
    kmMat4 _modelViewMV;
    void onDraw();
    CustomCommand _customCommand;

    
private:
    b2World* _world;
    b2Body* _stage_body;
    b2Body* _player_body;
    
    Label *_warning_label;
    
    std::vector<Node*> _tricks;
    std::vector<Node*> _warnings;

    Sprite* _player;
    Vec2 _player_vector;
    Vec2 _player_last_pos;
    bool _player_is_landing;
    
    TMXTiledMap* _map;
    ValueVector _collision_objs;
    ValueVector _tricks_objs;
    ValueVector _warning_objs;
    int _level;
    Size _visibleSize;
    
    EventListenerTouchOneByOne *_one_by_one_listener;
    
    GLESDebugDraw* _debugDraw;
};

#endif /* defined(__snowedge_test__GameMainScene__) */
