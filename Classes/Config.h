//
//  Config.h
//  snowedge_test
//
//  Created by ChainZ on 2014/06/04.
//
//

#ifndef __SNOWEDGE_CONFIG_H__
#define __SNOWEDGE_CONFIG_H__

namespace Config {
    enum ObjectType{
        PLAYER,
        GROUND
    };
}

namespace Tags{
    enum SpriteTag {
        TAG_PLAYER,
        TAG_GROUND,
        TAG_CAMERA
    };
}

namespace GameState {
    enum state{
        NORMAL,
        PAUSE,
        GAMEOVER
    };
}

namespace GameMap{
    enum Map{
        NORMAL_GROUND
    };
}


#endif
