//
//  Player.cpp
//  snowedge_test
//
//  Created by 張程 on 2014/06/02.
//
//

#include "Player.h"

Player::Player():
_type(Config::PLAYER)
{
}

Player::~Player()
{
}

Player* Player::createPlayer( const std::string &filename )
{
    auto player = new Player();
    if (player && player->initWithFile(filename))
    {
        player->autorelease();
        player->_on_ground = false;
        return player;
    }
    delete player;
    return NULL;
}