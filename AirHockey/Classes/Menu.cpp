//
//  Menu.cpp
//  AirHockey
//
//  Created by macbook_016 on 2013/06/20.
//
//

#include "Menu.h"
#include "PlayerName.h"
#include "GameManager.h"
using namespace cocos2d;
CCScene* Menu::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    Menu *layer = Menu::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}
bool Menu::init()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    //create startMenuItem
    startMenuItem = CCMenuItemImage::create(
                                        "start.png",
                                        "start.png",
                                        this,
                                        menu_selector(Menu::menuStartgame));
    startMenuItem->setPosition(ccp((size.width / 2) - 20, size.height - 348));

    //create rankMenuItem
   rankMenuItem = CCMenuItemImage::create(
                                    "rank.png",
                                    "rank.png",
                                    this,
                                    menu_selector(Menu::menuRanking));
    rankMenuItem->setPosition(ccp((size.width / 2) - 20, size.height - 487));

    //create BMGMenuItem
    bmgMenuItem = CCMenuItemImage::create(
                                    "bgm.png",
                                    "bgm.png",
                                    this,
                                    menu_selector(Menu::menuBGM));
    bmgMenuItem->setColor(ccc3(255,255,255));
    bmgMenuItem->setPosition(ccp((size.width / 2) - 20, size.height - 635));

    // create menu, it's an autorelease object
    pMenu = CCMenu::create(startMenuItem, rankMenuItem, bmgMenuItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
    CCSprite* pSprite = CCSprite::create("menu.png");
    
    //
    GameManager::sharedGameManager()->setBgm(true);

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(size.width / 2, size.height / 2));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    return true;
}
void Menu::menuBGM(CCObject* pSender)
{
    GameManager *game = GameManager::sharedGameManager();
    if (game->getBgm()) {
        bmgMenuItem->setColor(ccc3(32,32,64));
        game->setBgm(false);
    }else{
        bmgMenuItem->setColor(ccc3(255,255,255));
        game->setBgm(true);
    }
}
void Menu::menuRanking(CCObject* pSender)
{
}
void Menu::menuStartgame(CCObject* pSender)
{
    CCScene *playerNameScene = PlayerName::scene();
    CCScene *pScene = CCTransitionFadeTR::create(2, playerNameScene);
    CCDirector::sharedDirector()->replaceScene(pScene);
}


