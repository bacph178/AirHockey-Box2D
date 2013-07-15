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
#include "RankingScene.h"
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
    startMenuItem->setPosition(ccp(size.width / 2.10989011, size.height / 1.5147929));

    //create rankMenuItem
    rankMenuItem = CCMenuItemImage::create(
                                           "rank.png",
                                           "rank.png",
                                           this,
                                           menu_selector(Menu::menuRanking));
    rankMenuItem->setPosition(ccp(size.width / 2.10989011, size.height / 1.90689013));

    //create BMGMenuItem
    bmgMenuItem = CCMenuItemImage::create(
                                          "bgm.png",
                                          "bgm.png",
                                          this,
                                          menu_selector(Menu::menuBGM));
    bmgMenuItem->setPosition(ccp(size.width / 2.10989011, size.height / 2.63239075));
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("bmg")) {
        bmgMenuItem->setColor(ccc3(255, 255, 255));
    }else{
        bmgMenuItem->setColor(ccc3(32, 32, 64));
    }

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(startMenuItem, rankMenuItem, bmgMenuItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
    CCSprite* pSprite = CCSprite::create("menu.png");

    // position the sprite on the center of the screen
    pSprite->setPosition(ccp(size.width / 2, size.height / 2));

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    CCLOG("điểm mặc định: %d",GameManager::sharedGameManager()->getPoint());
    
   // GameManager::sharedGameManager()->setBgm(true);
    return true;
}
void Menu::menuBGM(CCObject* pSender)
{
    GameManager *game = GameManager::sharedGameManager();
    if (CCUserDefault::sharedUserDefault()->getBoolForKey("bmg")) {
        bmgMenuItem->setColor(ccc3(32, 32, 64));
        game->setBgm(false);
        CCUserDefault::sharedUserDefault()->setBoolForKey("bmg", false);
    } else {
        bmgMenuItem->setColor(ccc3(255, 255, 255));
        game->setBgm(true);
        CCUserDefault::sharedUserDefault()->setBoolForKey("bmg", true);
    }

}
void Menu::menuRanking(CCObject* pSender)
{
    CCScene *rankScene = CCScene::create();
    RankingScene *rankLayer = RankingScene::create();
    rankScene->addChild(rankLayer);
    CCScene *pScene = CCTransitionFadeTR::create(2, rankScene);
    CCDirector::sharedDirector()->replaceScene(pScene);
}
void Menu::menuStartgame(CCObject* pSender)
{
    CCScene *playerNameScene = PlayerName::scene();
    CCScene *pScene = CCTransitionFadeTR::create(2, playerNameScene);
    CCDirector::sharedDirector()->replaceScene(pScene);
}


