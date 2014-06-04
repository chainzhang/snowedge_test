#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "Box2D/Box2D.h"

class HelloWorld : public cocos2d::Layer, public b2ContactListener
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    // 衝突が始まった時に実行
    virtual void BeginContact(b2Contact* contact);
    
    // 衝突が終わった時に実行
    virtual void EndContact(b2Contact* contact);
};

#endif // __HELLOWORLD_SCENE_H__
