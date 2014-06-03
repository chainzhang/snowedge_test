//
//  newMainGameScene.cpp
//  snowedge_test
//
//  Created by 張程 on 2014/06/02.
//
//

#include "newMainGameScene.h"
#include "Player.h"
#include "CustomFollow.h"

#define GRAVITY -49.0f
#define PTM_RATIO 32
#define PI 3.1415926

namespace Tags{
    enum SpriteTag {
        TAG_GROUND = 1000
    };
}


NewMainGame::NewMainGame():
_last_ground_tail_pos(Vec2(0, 0)),
_ground_passed(0),
_jump_available(true)
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
    _last_ground_tail_pos = Vec2(0, _visibleSize.height/2.5);
    
    //init ground
    for (int i = 0; i < 3; i++)
    {
        this->genGround();
    }
    
    //init player
    _player = Player::createPlayer("star.png");
    
    b2BodyDef player_body_def;
    player_body_def.type = b2_dynamicBody;
    player_body_def.position.Set(_visibleSize.width/8/PTM_RATIO, _visibleSize.height/1.2/PTM_RATIO
                                 );
    
    _player_body = _world->CreateBody(&player_body_def);
    
    b2CircleShape circle;
    circle.m_radius = 36.0f/PTM_RATIO;
    
    b2FixtureDef player_fixture_def;
    player_fixture_def.shape = &circle;
    player_fixture_def.density = 3;
    player_fixture_def.restitution = 0;
    player_fixture_def.friction = 0.15;
    
    _player_body->CreateFixture(&player_fixture_def);
    _player_body->SetFixedRotation(true);
    _player_body->SetUserData(_player);
    
    _player->setPosition(Vec2(_visibleSize.width/8, _visibleSize.height/1.2));
    this->addChild(_player);
    
    
    this->runAction(CustomFollow::create(_player,Point(-300, -80)));
    
    this->scheduleUpdate();
    
    this->initUserControl();
    
    return true;
}

void NewMainGame::initUserControl()
{
    _one_by_one_listener = EventListenerTouchOneByOne::create();
    _one_by_one_listener->setSwallowTouches(true);
    
    _one_by_one_listener->onTouchBegan = [this](Touch *touch, Event *event){
        
        CCLOG("on ground:%i", _player->getOnGround());
        
        if (_player->getOnGround())
        {
            CCLOG("Jump");
            _player_body->ApplyLinearImpulse(b2Vec2(0,16*_player_body->GetMass()*(-GRAVITY)/PTM_RATIO), _player_body->GetWorldCenter(), true);
            _player->setOnGround(false);
            return true;
        }
        
        return false;
    };
    _eventDispatcher->addEventListenerWithFixedPriority(_one_by_one_listener, 8000);
    
    
    _all_at_once_listener = EventListenerTouchAllAtOnce::create();
    
    
    _world->SetContactListener(this);
}

void NewMainGame::update(float delta)
{
    _world->Step(delta, 8 , 1);
    
    for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != NULL) {
            Sprite* myActor = (Sprite*)b->GetUserData();
            myActor->setPosition( Point( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }
    }
    
    if (_player->getPosition().x - (_ground_passed+1)*1136 > 1136/2) {
        this->genGround();
        _ground_passed++;
    }
}

void NewMainGame::genGround()
{
    auto ground = Sprite::create("ground.png");
    ground->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    ground->setPosition(_last_ground_tail_pos);
    this->addChild(ground);
    
    b2BodyDef ground_body_def;
    ground_body_def.type = b2_staticBody;
    
    b2Body *ground_body;
    ground_body = _world->CreateBody(&ground_body_def);
    
    b2Vec2 polygon_points[] = {b2Vec2(ground->getPosition().x/PTM_RATIO, ground->getPosition().y/PTM_RATIO),
                               b2Vec2((ground->getPosition().x + ground->getContentSize().width)/PTM_RATIO, (ground->getPosition().y - ground->getContentSize().width*tan(PI/12))/PTM_RATIO),
                               b2Vec2((ground->getPosition().x + ground->getContentSize().width)/PTM_RATIO, (ground->getPosition().y - ground->getContentSize().height)/PTM_RATIO),
                               b2Vec2(ground->getPosition().x/PTM_RATIO, (ground->getPosition().y - ground->getContentSize().width*tan(PI/12))/PTM_RATIO)
    };
    
    b2PolygonShape ground_shape;
    ground_shape.Set(polygon_points, 4);
    
    ground_body->CreateFixture(&ground_shape, 0);
    _grounds.push_back(ground_body);
    _last_ground_tail_pos += Vec2(ground->getContentSize().width, -tan(PI/12)*ground->getContentSize().width);
    if (_grounds.size() > 3)
    {
        _grounds.front()->GetWorld()->DestroyBody(_grounds.front());
        _grounds.pop_front();
    }
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


void NewMainGame::BeginContact(b2Contact *contact)
{
    CCLOG("contacting");
    void* the_item = contact->GetFixtureA()->GetBody()->GetUserData();
    if (the_item)
    {
        CCLOG("userdata_OK");
        static_cast<Player*>(the_item)->setOnGround(true);
    }
    
    the_item = contact->GetFixtureB()->GetBody()->GetUserData();
    if (the_item)
    {
        CCLOG("userdata_OK");
        static_cast<Player*>(the_item)->setOnGround(true);
    }
}

void NewMainGame::EndContact(b2Contact *contact)
{
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
