//
//  DynamicsLabel.h
//  snowedge_test
//
//  Created by ChainZ on 2014/06/05.
//
//

#ifndef __DYNAMICS_NUMBER_LABEL__
#define __DYNAMICS_NUMBER_LABEL__

#include "cocos2d.h"

class DynamicsNumberLabel : public cocos2d::Label
{
public:
    static DynamicsNumberLabel* createNumLabel(const int number = 0);
    
};


#endif
