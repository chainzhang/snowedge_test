//
//  newMainGameScene.cpp
//  snowedge_test
//
//  Created by 張程 on 2014/06/02.
//
//

#include "Config.h"
#include "newMainGameScene.h"
#include "GameOverScene.h"
#include "Player.h"
#include "Ground.h"

#define GRAVITY -72.0f
#define PTM_RATIO 32
#define PI 3.1415926


NewMainGame::NewMainGame():
_last_ground_tail_pos(Vec2(0, 0)),
_ground_passed(0),
_jump_available(true),
_map_size(20),
_map_now(0),
_camera_moving_factor(1.0f),
_camera_offset(Vec2(0,0)),
_camera_current_offset(Vec2(0,0)),
_game_state(GameState::NORMAL)
{
    initPhyWorld();
    _map = new int[_map_size];
}

NewMainGame::~NewMainGame()
{
    delete _world;
    delete _debugDraw;
    delete [] _map;
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
    
    for (int i = 0; i < _map_size; i++)
    {
        if (i == 10 || i == 12 || i == 13 || i ==4 || i == 6)
        {
            _map[i] = 0;
        }
        else
        {
            _map[i] = 1;
        }
    }
    
    //init ground
    for (int i = 0; i < 3; i++)
    {
        this->genGround();
    }
    
    //init player
    _player = Player::createPlayer("star.png");
    _player->setTag(Tags::TAG_PLAYER);
    
    _camera = Node::create();
    _camera->setPosition(_visibleSize.width/2, _visibleSize.height/2);
    _camera->setTag(Tags::TAG_CAMERA);
    
    
    b2BodyDef player_body_def;
    player_body_def.type = b2_dynamicBody;
    player_body_def.position.Set(_visibleSize.width/8/PTM_RATIO, _visibleSize.height/1.2/PTM_RATIO
                                 );
    
    _player_body = _world->CreateBody(&player_body_def);
    
    b2CircleShape circle;
    circle.m_radius = 36.0f/PTM_RATIO;
    
    b2FixtureDef player_fixture_def;
    player_fixture_def.shape = &circle;
    player_fixture_def.density = 1000;
    player_fixture_def.restitution = 0;
    player_fixture_def.friction = 0;
    
    _player_body->CreateFixture(&player_fixture_def);
    _player_body->SetFixedRotation(true);
    _player_body->SetUserData(_player);
    
    _player->setPosition(Vec2(_visibleSize.width/8, _visibleSize.height/1.2));
    this->addChild(_player);
    this->addChild(_camera);
    
    this->runAction(Follow::create(_camera));
    
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
        
        for (b2ContactEdge* ce = _player_body->GetContactList(); ce;ce = ce->next)
        {
            b2Contact* c = ce->contact;
            if (c->IsTouching()) {
                _player->setOnGround(true);
            } else {
                _player->setOnGround(false);
            }
        }
        
        if (_player->getOnGround())
        {
            CCLOG("Jump");
            _camera_offset = Vec2(-_player_body->GetLinearVelocity().x*PTM_RATIO/10, 0);
            _player_body->ApplyLinearImpulse(b2Vec2(0,14*_player_body->GetMass()*(-GRAVITY)/PTM_RATIO), _player_body->GetWorldCenter(), true);
            _player->setOnGround(false);
            return true;
        }
        
        return false;
    };
    
    _one_by_one_listener->onTouchMoved = NULL;
    _one_by_one_listener->onTouchEnded = NULL;
    
    _eventDispatcher->addEventListenerWithFixedPriority(_one_by_one_listener, 8000);
    
    
    _all_at_once_listener = EventListenerTouchAllAtOnce::create();
    
    
    _world->SetContactListener(this);
}

void NewMainGame::update(float delta)
{
    if (_game_state == GameState::GAMEOVER){
        this->unscheduleAllSelectors();
        this->GameOver();
    }
    
    
    
    Vec2 player_current_pos = _player->getPosition();
    
    _world->Step(delta, 8 , 1);
    
    for (b2Body* b = _world->GetBodyList(); b; b = b->GetNext())
    {
        if (b->GetUserData() != NULL && b->GetType() == b2_dynamicBody) {
            Sprite* myActor = (Sprite*)b->GetUserData();
            myActor->setPosition( Vec2( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO) );
            myActor->setRotation( -1 * CC_RADIANS_TO_DEGREES(b->GetAngle()) );
        }
    }
    
    if (_player->getPosition().x - (_ground_passed+1)*1136 > 1136/2) {
        this->genGround();
        _ground_passed++;
    }
    
    if (_player_body->GetLinearVelocity().x > 80)
    {
        _player_body->SetLinearVelocity(b2Vec2(80, _player_body->GetLinearVelocity().y));
    }
    
    Vec2 camera_pos = (_player->getPosition()-player_current_pos);
    camera_pos.y = -camera_pos.x*tan(PI/12);
    
    if (_camera_offset != _camera_current_offset)
    {
        _camera->setPosition(_camera->getPosition() + camera_pos + (_camera_offset - _camera_current_offset)*delta/0.2f);
        _camera_current_offset += (_camera_offset - _camera_current_offset)*delta/0.2f;
    }
    else
    {
        _camera->setPosition(_camera->getPosition() + camera_pos);
    }
        
    
    if (_camera->getPosition().y - _player->getPosition().y > _visibleSize.height) {
        _game_state = GameState::GAMEOVER;
    }
    
    CCLOG("SPEED::::::::%f", _player_body->GetLinearVelocity().x);
}

void NewMainGame::genGround()
{
    if (_map[_map_now] == 1) {
    
    auto ground = Sprite::create("ground.png");
    ground->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    ground->setPosition(_last_ground_tail_pos);
    ground->setTag(Tags::TAG_GROUND);
    this->addChild(ground);
    
    b2BodyDef ground_body_def;
    ground_body_def.type = b2_staticBody;
    
    b2Body *ground_body;
    ground_body = _world->CreateBody(&ground_body_def);
    ground_body->SetUserData(ground);
    
    b2Vec2 polygon_points[] = {b2Vec2(ground->getPosition().x/PTM_RATIO, ground->getPosition().y/PTM_RATIO-0.01f),
                               b2Vec2((ground->getPosition().x + ground->getContentSize().width)/PTM_RATIO, (ground->getPosition().y - ground->getContentSize().width*tan(PI/12))/PTM_RATIO),
                               b2Vec2((ground->getPosition().x + ground->getContentSize().width)/PTM_RATIO, (ground->getPosition().y - ground->getContentSize().height)/PTM_RATIO),
                               b2Vec2(ground->getPosition().x/PTM_RATIO, (ground->getPosition().y - ground->getContentSize().width*tan(PI/12))/PTM_RATIO)
                               };
    
    b2PolygonShape ground_shape;
    ground_shape.Set(polygon_points, 4);
    
    ground_body->CreateFixture(&ground_shape, 0);
    _grounds.push_back(ground_body);
    _last_ground_tail_pos += Vec2(ground->getContentSize().width, -tan(PI/12)*ground->getContentSize().width);
    } else {
        _last_ground_tail_pos += Vec2(1136.0f, -tan(PI/12)*1136.0f);
    }
    if (_grounds.size() > 3)
    {
        _grounds.front()->GetWorld()->DestroyBody(_grounds.front());
        _grounds.pop_front();
    }
    _map_now++;
    if (_map_now > _map_size)
    {
        _map_now = 0;
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
    void* the_item_a = contact->GetFixtureA()->GetBody()->GetUserData();
    void* the_item_b = contact->GetFixtureB()->GetBody()->GetUserData();
    if (the_item_a && the_item_b)
    {
        if ((static_cast<Sprite*>(the_item_a)->getTag() == Tags::TAG_PLAYER &&
             static_cast<Sprite*>(the_item_b)->getTag() == Tags::TAG_GROUND))
        {
            _camera_offset = Vec2(0,0);
        }
        else if ((static_cast<Sprite*>(the_item_a)->getTag() == Tags::TAG_GROUND &&
                  static_cast<Sprite*>(the_item_b)->getTag() == Tags::TAG_PLAYER))
        {
            _camera_offset = Vec2(0,0);
        }

    }
    
}

void NewMainGame::EndContact(b2Contact *contact)
{
}


void NewMainGame::GameOver()
{
    _eventDispatcher->removeAllEventListeners();
    Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0f, GameOver::createScene()));
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
