//
//  GameOverScene.cpp
//  snowedge_test
//
//  Created by 張程 on 2014/06/04.
//
//

#include "GameOverScene.h"
#include "newMainGameScene.h"

USING_NS_CC;

GameOver::GameOver()
{
}

GameOver::~GameOver()
{
}

Scene* GameOver::createScene()
{
    auto scene = Scene::create();
    auto layer = GameOver::create();
    scene->addChild(layer);
    return scene;
}

bool GameOver::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    auto gameover = Label::createWithSystemFont("GAMEOVER", "HiraKaku-W3", 72.0f);
    gameover->setColor(Color3B(255, 255, 255));
    gameover->setPosition(visibleSize.width/2, visibleSize.height/2);
    
    this->addChild(gameover);
    
    listener->onTouchBegan = [this, &listener](Touch *touch, Event *event){
        this->reChallenge();
        
        listener->setSwallowTouches(false);
        return true;
    };
    
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 9999);
    
    return true;
}

void GameOver::reChallenge()
{
    Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0f, NewMainGame::createScene()));
}