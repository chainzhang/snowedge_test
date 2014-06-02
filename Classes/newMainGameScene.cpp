//
//  newMainGameScene.cpp
//  snowedge_test
//
//  Created by 張程 on 2014/06/02.
//
//

#include "newMainGameScene.h"

#define GRAVITY 9.8f
#define PTM_RATIO 32

NewMainGame::NewMainGame():
_last_ground_tail_pos(Vec2(0, 0))
{
    initPhyWorld();
}

NewMainGame::~NewMainGame()
{
    delete _world;
    delete _debugDraw;
}

Scene* NewMainGame::createScene()
{
    auto scene = Scene::create();
    auto layer = NewMainGame::create();
    scene->addChild(layer);
    return scene;
}

bool NewMainGame::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    _visibleSize = Director::getInstance()->getVisibleSize();
    
    //init ground
    Vec2 start_point = Vec2(0, _visibleSize.height/2);
    
    for (int i = 0; i < 3; i++)
    {
        this->genGround(start_point);
    }
    
    
    
    
    
    return true;
}

void NewMainGame::update(float delta)
{
}

void NewMainGame::genGround(Vec2 pos)
{
    auto ground = Sprite::create("ground.png");
    ground->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    ground->setPosition(pos);
    ground->retain();
    
    _grounds.push_back(ground);
    _last_ground_tail_pos = pos + Vec2(ground->getContentSize().width, ground->getContentSize().height);
    if (_grounds.size() == 3)
    {
        _grounds.pop_front();
    }
    this->addChild(ground);
}

void NewMainGame::initPhyWorld()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, GRAVITY);
    
    _world = new b2World(gravity);
    
    _world->SetAllowSleeping(true);
    _world->SetContinuousPhysics(true);
    
    _debugDraw = new GLESDebugDraw( PTM_RATIO );
    _world->SetDebugDraw(_debugDraw);
    
    uint32 flag = 0;
    flag += b2Draw::e_shapeBit;
    
    _debugDraw->SetFlags(flag);
}


void NewMainGame::draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    Layer::draw(renderer, transform, transformUpdated);
    
    GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
    
    kmGLPushMatrix();
    kmGLGetMatrix(KM_GL_MODELVIEW, &_modelViewMV);
    
    _customCommand.init(_globalZOrder);
    _customCommand.func = CC_CALLBACK_0(NewMainGame::onDraw, this);
    renderer->addCommand(&_customCommand);
    
    kmGLPopMatrix();
}

void NewMainGame::onDraw()
{
    kmMat4 oldMV;
    kmGLGetMatrix(KM_GL_MODELVIEW, &oldMV);
    kmGLLoadMatrix(&_modelViewMV);
    _world->DrawDebugData();
    kmGLLoadMatrix(&oldMV);
}
