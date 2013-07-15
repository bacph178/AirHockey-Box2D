//
//  LocalRankScene.cpp
//  AirHockey
//
//  Created by macbook_016 on 2013/07/09.
//
//

#include "LocalRankScene.h"
#include "cocos2d.h"
#include "GameManager.h"
#include "GamePlay.h"
#include "PlayerName.h"
#include "Menu.h"
#include "RankingScene.h"
#include "IOFile.h"
#include "CCTableView.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace std;

#include "string.h"
CCScene* LocalRankScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    LocalRankScene *layer1 = LocalRankScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer1, 123);
    
    // return the scene
    return scene;
}
bool LocalRankScene::init()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    IOFile *io = new IOFile("localRank.plist");
    players = io->read();
    CCTableView *tableView=CCTableView::create(this, CCSizeMake(600, 700));
    tableView->setDirection(kCCScrollViewDirectionVertical);
    tableView->setAnchorPoint(ccp(0, 0));
    tableView->setPosition(ccp(size.width / 8, 200));
    tableView->setDelegate(this);
    tableView->setVerticalFillOrder(kCCTableViewFillTopDown);
    this->addChild(tableView, 0);
    tableView->reloadData();
    
    //create localRankLabel
    CCLabelTTF *localRankLabel = CCLabelTTF::create("LOCAL", "Time New Roman", 50);
    localRankLabel->setColor(ccc3(32, 200, 32));
    localRankLabel->setPosition(ccp(size.width * 3 / 4, 10 * size.height / 11));
    this->addChild(localRankLabel,120);
    
    //creat rankingMenuItem
    CCLabelTTF *ranking = CCLabelTTF::create("RANKING", "Time new roman", 50);
    CCMenuItemLabel *rankMenuItem =
    CCMenuItemLabel::create(ranking, this, menu_selector(LocalRankScene::menuRanking));
    rankMenuItem->setPosition(ccp(size.width * 2 / 7, 10 * size.height / 11));
    
    //create startMenuItem
    CCMenuItemImage *startMenuItem =
    CCMenuItemImage::create("btn_Continue.png", "btn_Continue.png",
                            this,menu_selector(LocalRankScene::menuPlay));
    startMenuItem->setScale(3);
    startMenuItem->setPosition(ccp(size.width / 3.5, size.height / 8));
    
    //create MenuItem
    CCMenuItemImage *MenuItem =
    CCMenuItemImage::create("btn_menu.png", "btn_menu.png",
                            this, menu_selector(LocalRankScene::menuMenu));
    MenuItem->setScale(3);
    MenuItem->setPosition(ccp(size.width * 3 / 4, size.height / 8));
    CCMenu* pMenu = CCMenu::create(startMenuItem,
                                   MenuItem,
                                   rankMenuItem,
                                   NULL);
    pMenu->setPosition(ccp(0, 0));
    this->addChild(pMenu,120);
    return true;
}
void LocalRankScene::menuPlay(CCObject* pSender)
{
    if (GameManager::sharedGameManager()->getPoint() != 0) {
        CCScene *GamePlayScene = GamePlay::scene();
        CCDirector::sharedDirector()->replaceScene(GamePlayScene);
    }else {
        CCDirector::sharedDirector()->replaceScene(PlayerName::scene());
    }
}
void LocalRankScene::menuMenu(CCObject* pSender)
{
    CCDirector::sharedDirector()->replaceScene(Menu::scene());
}
void LocalRankScene::menuRanking(CCObject *pSender)
{
    CCDirector::sharedDirector()->replaceScene(RankingScene::scene());
}
void LocalRankScene::tableCellTouched(CCTableView* table, CCTableViewCell* cell)
{
    CCLOG("cell touched at index: %i", cell->getIdx());
}

CCSize LocalRankScene::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
    return CCSizeMake(600, 80);
}

CCTableViewCell* LocalRankScene::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    
    
    CCTableViewCell *cell = table->dequeueCell();
    cell = new CCTableViewCell();
    cell->autorelease();    
    //
    Player * p = (Player*)players->objectAtIndex(idx);
    CCString *string = CCString::createWithFormat(" :  %d",p->getMark());
    CCLabelTTF *Pointlabel = CCLabelTTF::create(string->getCString(), "Helvetica", 50.0);
    Pointlabel->setAnchorPoint(ccp(1, 0));
    Pointlabel->setPosition(ccp(600,0));
    Pointlabel->setTag(123);
    cell->addChild(Pointlabel);
    
    std::string name = p->getName().c_str();
    int i = 0;
    while (name[i] != ' ') {
        i++;
    }
    name.erase(0,i+1);
    CCLabelTTF *Namelabel = CCLabelTTF::create(name.c_str(), "Helvetica", 50.0);
    Namelabel->setAnchorPoint(CCPointZero);
    Namelabel->setPosition(CCPointZero);
    if (name.c_str() == CCUserDefault::sharedUserDefault()->getStringForKey("playerName") &&
        p->getMark() == GameManager::sharedGameManager()->getPoint())
    {
        Namelabel->setColor(ccc3(32, 200, 32));
    }
    Namelabel->setTag(123);
    cell->addChild(Namelabel);
    return cell;
}

unsigned int LocalRankScene::numberOfCellsInTableView(CCTableView *table)
{
    return players->count();
}
