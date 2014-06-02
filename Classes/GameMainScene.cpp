//
//  GameMainScene.cpp
//  snowedge_test
//
//  Created by 張程 on 2014/05/27.
//
//

#include "GameMainScene.h"

#define GRAVITY -49
#define PTM_RATIO 32

GameMain::GameMain():
_world(NULL),
_player(NULL),
_player_vector(Vec2(0,0)),
_player_last_pos(Vec2(0,0)),
_player_is_landing(false)
{
    initPhyWorld();
}

GameMain::~GameMain()
{
    delete _world;
    delete _debugDraw;
}


Scene* GameMain::createScene(const int level)
{
    auto scene = Scene::create();
    auto layer = GameMain::create();
    
    layer->_level = level;
    
    scene->addChild(layer);
    return scene;
}

bool GameMain::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    _visibleSize = Director::getInstance()->getVisibleSize();

    _player = Sprite::create("star.png");
    _player->setPosition(Vec2(_visibleSize.width/3, _visibleSize.height));
    _player_vector = _player->getPosition();
    
    _warning_label = Label::createWithSystemFont("WARNING", "HiraKaku-W3", 72.0f);
    _warning_label->setColor(Color3B(255, 255, 100));
    _warning_label->setOpacity(0);
    this->addChild(_warning_label,9);
    
    
    b2BodyDef player_body_def;
    player_body_def.type = b2_dynamicBody;
    player_body_def.position.Set(_player->getPosition().x/PTM_RATIO, _player->getPosition().y/PTM_RATIO);
    _player_body = _world->CreateBody(&player_body_def);
    
    b2CircleShape circle;
    circle.m_radius = 36.0/PTM_RATIO;
    
    b2FixtureDef player_fixture_def;
    player_fixture_def.shape = &circle;
    player_fixture_def.density = 4;
    player_fixture_def.restitution = 0;
    player_fixture_def.friction = 0.1;
    
    _player_body->CreateFixture(&player_fixture_def);
    _player_body->SetUserData(_player);
    _player_body->SetFixedRotation(true);
    
    this->addChild(_player);
    
    
    this->scheduleUpdate();
    this->runAction(Follow::create(_player));
    
    initStage();
    _world->SetContactListener(this);
    
    return true;
}

void GameMain::update(float delta)
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
    
    if (tricksCollision(_player))
    {
        _player_body->ApplyLinearImpulse(b2Vec2(abs(_player_vector.x)*1.5*PTM_RATIO, abs(_player_vector.y)*2.2*PTM_RATIO), _player_body->GetWorldCenter(), true);
    }
    
    if (enterWarningArea(_player))
    {
        if (_warning_label->getNumberOfRunningActions() == 0) {
            CCLOG("Warning");
            _warning_label->runAction(Sequence::create(FadeTo::create(0.2f, 255),
                                                       DelayTime::create(0.2f),
                                                       FadeTo::create(0.2f, 140),
                                                       NULL));
        }
    } else {
        if (_warning_label->getOpacity() > 0 &&
            _warning_label->getNumberOfRunningActions() == 0){
            _warning_label->runAction(FadeOut::create(0.5));
        }
    }
    
    
    _warning_label->setPosition(_player->getPosition() + Vec2(0, 100.0f));
    

    //CCLOG("Pos:(%f, %f)", _player->getPosition().x, _player->getPosition().y);
    //CCLOG("ySpeed:%f",_player_body->GetLinearVelocity().y);
    
    updatePlayerVector();
    //CCLOG("PlayerVector(%f, %f)", _player_vector.x, _player_vector.y);
}


void GameMain::initStage()
{
    _map = TMXTiledMap::create("ice_map.tmx");
    _map->setPosition(Point(100.0f, -200.0f));
    this->addChild(_map);
    
    b2BodyDef stageBody;
    stageBody.type = b2_staticBody;
    _stage_body = _world->CreateBody(&stageBody);
    
    /*============================================================
     *
     * Collision
     *
     *===========================================================*/
    
    
    auto collision_layer = _map->getObjectGroup("collision");
    _collision_objs = collision_layer->getObjects();
    CCLOG("%s", Value(_collision_objs).getDescription().c_str());
    
    Vec2 offset = _map->getPosition();
    for (auto& obj : _collision_objs)
    {
        ValueMap& dict = obj.asValueMap();
        
        if (dict["width"].asInt() <= 0)
        {
            ValueVector points = dict["points"].asValueVector();
            std::vector<Vec2> the_points;
            for (auto& point:points)
            {
                ValueMap& the_point = point.asValueMap();
                the_points.push_back(Vec2(the_point["x"].asFloat()+dict["x"].asFloat()+offset.x,
                                          -the_point["y"].asFloat()+dict["y"].asFloat()+offset.y));
                CCLOG("Point(%f, %f)",the_point["x"].asFloat()+dict["x"].asFloat(),the_point["y"].asFloat()+dict["y"].asFloat());
            }
            createPolygonB2Body(&the_points[0], the_points.size());
        }
        
        if (dict["type"].asString() == "rect"){
            createRectB2Body(Vec2(dict["x"].asFloat()+offset.x, dict["y"].asFloat()+offset.y), dict["width"].asFloat(), dict["height"].asFloat());
        }
        
    }
    
    
    //===============================================================
    
    
    /*============================================================
     *
     * tricks
     *
     *===========================================================*/
    
    
    auto tricks_layer = _map->getObjectGroup("tricks");
    
    _tricks_objs = tricks_layer->getObjects();
    //CCLOG("%s", Value(_collision_objs).getDescription().c_str());
    
    for (auto& obj : _tricks_objs)
    {
        ValueMap& dict = obj.asValueMap();
        
        if (dict["width"].asInt() <= 0)
        {
            ValueVector points = dict["points"].asValueVector();
            std::vector<Vec2> the_points;
            for (auto& point:points)
            {
                ValueMap& the_point = point.asValueMap();
                the_points.push_back(Vec2(the_point["x"].asFloat()+dict["x"].asFloat()+offset.x,
                                          -the_point["y"].asFloat()+dict["y"].asFloat()+offset.y));
                CCLOG("Point(%f, %f)",the_point["x"].asFloat()+dict["x"].asFloat(),the_point["y"].asFloat()+dict["y"].asFloat());
            }
            
        }
        
        if (dict["type"].asString() == "jump"){
            //createRectB2Body(Vec2(dict["x"].asFloat()+offset.x, dict["y"].asFloat()+offset.y), dict["width"].asFloat(), dict["height"].asFloat());
            createJumpTricks(Vec2(dict["x"].asFloat()+offset.x, dict["y"].asFloat()+offset.y), dict["width"].asFloat(), dict["height"].asFloat());
                
            CCLOG("create Jump");
        }
        
    }
    
    
    //===============================================================
    
    /*============================================================
     *
     * tricks
     *
     *===========================================================*/
    
    
    auto warning_layer = _map->getObjectGroup("warning");
    
    _warning_objs = warning_layer->getObjects();
    //CCLOG("%s", Value(_collision_objs).getDescription().c_str());
    
    for (auto& obj : _warning_objs)
    {
        ValueMap& dict = obj.asValueMap();
        
        if (dict["type"].asString() == "yellow"){
            //createRectB2Body(Vec2(dict["x"].asFloat()+offset.x, dict["y"].asFloat()+offset.y), dict["width"].asFloat(), dict["height"].asFloat());
            createWarningArea(Vec2(dict["x"].asFloat()+offset.x, dict["y"].asFloat()+offset.y), dict["width"].asFloat(), dict["height"].asFloat());
            
            CCLOG("create Jump");
        }
        
    }
    
    
    //===============================================================
    
    
    
    
    initUserControl();
    
}







void GameMain::createPolygonB2Body(const Vec2 *points, int const count)
{
    b2PolygonShape polygonShape;
    std::vector<b2Vec2> b2points;
    for (int i = 0; i < count; i++)
    {
        b2points.push_back(toB2Vec2(points[i]));
    }
    
    polygonShape.Set(&b2points[0], b2points.size());
    _stage_body->CreateFixture(&polygonShape,0);
}

void GameMain::createRectB2Body(const cocos2d::Vec2 top_left_point, const float width, const float height)
{
    Vec2 points[] = {top_left_point, top_left_point+Vec2(width, 0.0f), top_left_point + Vec2(width, height), top_left_point + Vec2(0, height)};
    
    createPolygonB2Body(points, 4);
}





bool GameMain::tricksCollision(cocos2d::Sprite *_player)
{
    for (int i = 0; i < _tricks.size(); i++)
    {
        CCLOG("checkPoint(%f, %f, %f, %f)",_tricks.at(i)->getPosition().x, _tricks.at(i)->getPosition().y, _tricks.at(i)->getPosition().x+_tricks.at(i)->getContentSize().width, _tricks.at(i)->getPosition().y - _tricks.at(i)->getContentSize().height);
        
        Vec2 p_pos = _player->getPosition()-Vec2(0, _player->getContentSize().width/2);
        if (p_pos.x >= _tricks.at(i)->getPosition().x &&
            p_pos.x <= _tricks.at(i)->getPosition().x + _tricks.at(i)->getContentSize().width &&
            p_pos.y <= _tricks.at(i)->getPosition().y + _tricks.at(i)->getContentSize().height/2)
        {
            CCLOG("enter jump");
            if (_player_vector.y < -0.01) {
                CCLOG("Jump!!!!!!!!!");
                return true;
            }
        }
    }
    return false;
}


void GameMain::createJumpTricks(const Vec2 top_left_point, const float width, const float height)
{
    auto layer = LayerColor::create(Color4B(150, 255, 220, 165), width, height);
    layer->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    layer->setPosition(top_left_point);
    _tricks.push_back(layer);
    addChild(layer);
}


void GameMain::createWarningArea(const cocos2d::Vec2 top_left_point, const float width, const float height)
{
    auto layer = LayerColor::create(Color4B(255, 255, 100, 20), width, height);
    layer->setAnchorPoint(Point::ANCHOR_TOP_LEFT);
    layer->setPosition(top_left_point);
    _warnings.push_back(layer);
    addChild(layer);
}

bool GameMain::enterWarningArea(const Sprite* _player)
{
    for (int i = 0; i <_warnings.size(); i++)
    {
        Vec2 p_pos = _player->getPosition()-Vec2(0, _player->getContentSize().width/2);
        CCLOG("Player---Y = %f ; warningY------ = %f", p_pos.y,_warnings.at(i)->getPosition().y);
        if (p_pos.x >= _warnings.at(i)->getPosition().x &&
            p_pos.x <= _warnings.at(i)->getPosition().x + _warnings.at(i)->getContentSize().width &&
            p_pos.y <= _warnings.at(i)->getPosition().y + _warnings.at(i)->getContentSize().height)
        {
                return true;
        }
    }
    return false;
}












void GameMain::initPhyWorld()
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




void GameMain::initUserControl()
{
    _one_by_one_listener = EventListenerTouchOneByOne::create();
    _one_by_one_listener->setSwallowTouches(true);
    
    _one_by_one_listener->onTouchBegan = [this](Touch *touch, Event *event){
        if (!_player_is_landing) return false;
        
        _player_body->ApplyLinearImpulse(b2Vec2(0,14*PTM_RATIO), _player_body->GetWorldCenter(), true);
        _player_is_landing = false;
        return true;
    };
    
    _eventDispatcher->addEventListenerWithFixedPriority(_one_by_one_listener, 9999);
}


void GameMain::updatePlayerVector()
{
    _player_vector = _player->getPosition() - _player_last_pos;
    _player_last_pos = _player->getPosition();
}



void GameMain::BeginContact(b2Contact *contact)
{
    CCLOG("contacting");
    void* body_userdata = contact->GetFixtureA()->GetBody()->GetUserData();
    if (body_userdata)
    {
        _player_is_landing = true;
    }
    body_userdata = contact->GetFixtureB()->GetBody()->GetUserData();
    if (body_userdata)
    {
        _player_is_landing = true;
    }

}

void GameMain::EndContact(b2Contact *contact)
{
    
}


void GameMain::draw(Renderer *renderer, const kmMat4 &transform, bool transformUpdated)
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
    _customCommand.func = CC_CALLBACK_0(GameMain::onDraw, this);
    renderer->addCommand(&_customCommand);
    
    kmGLPopMatrix();
}

void GameMain::onDraw()
{
    kmMat4 oldMV;
    kmGLGetMatrix(KM_GL_MODELVIEW, &oldMV);
    kmGLLoadMatrix(&_modelViewMV);
    _world->DrawDebugData();
    kmGLLoadMatrix(&oldMV);
}

b2Vec2 GameMain::toB2Vec2(cocos2d::Vec2 vec2)
{
    return b2Vec2(vec2.x/PTM_RATIO, vec2.y/PTM_RATIO);
}