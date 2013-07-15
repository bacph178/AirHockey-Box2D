//
//  RankingScene.cpp
//  AirHockey
//
//

#include "RankingScene.h"
#include "cocos2d.h"
#include "rapidjson.h"
#include "document.h"
#include "Menu.h"
#include "GamePlay.h"
#include "PlayerName.h"
#include "GameManager.h"
#include "LocalRankScene.h"
using namespace cocos2d;
USING_NS_CC;
USING_NS_CC_EXT;

CCScene* RankingScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    RankingScene *layer1 = RankingScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer1);
    
    // return the scene
    return scene;
}
bool RankingScene::init()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCLabelTTF *ranking = CCLabelTTF::create("RANKING", "Time new roman", 50);
    ranking->setColor(ccc3(32, 200, 34));
    ranking->setPosition(ccp(size.width * 2 / 7, 10 * size.height / 11));
    this->addChild(ranking);
    CCLabelTTF *localRank = CCLabelTTF::create("LOCAL", "Time new roman", 50);
    CCMenuItemLabel *localMenuItem = CCMenuItemLabel::create(localRank,
                                                           this,
                                                           menu_selector(RankingScene::local));
    localMenuItem->setPosition(ccp(size.width * 3 / 4, 10 * size.height / 11));

    CCHttpRequest* request = new CCHttpRequest();
    request->setUrl("http://192.168.1.177:3000/users.json");
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, callfuncND_selector(RankingScene::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(request);
    request->release();
    
    //create startMenuItem
    CCMenuItemImage *startMenuItem = CCMenuItemImage::create(
                                                             "btn_Continue.png",
                                                             "btn_Continue.png",
                                                             this,
                                                             menu_selector(RankingScene::menuPlay));
    startMenuItem->setScale(3);
    startMenuItem->setPosition(ccp(size.width / 3.5, size.height / 8));
    
    //create rankMenuItem
    CCMenuItemImage *rankMenuItem = CCMenuItemImage::create(
                                                            "btn_menu.png",
                                                            "btn_menu.png",
                                                            this,
                                                            menu_selector(RankingScene::menuMenu));
    rankMenuItem->setScale(3);
    rankMenuItem->setPosition(ccp(size.width * 3 / 4, size.height / 8));
    CCMenu* pMenu = CCMenu::create(startMenuItem,
                                   rankMenuItem,
                                   localMenuItem,
                                   NULL);
    pMenu->setPosition(ccp(0, 0));
    this->addChild(pMenu);
    
    return true;
}
void RankingScene::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCHttpResponse *response = (CCHttpResponse*)data;
    
    if (!response)
    {
        return;
    }
    // You can get original request type from: response->request->reqType
    if (0 != strlen(response->getHttpRequest()->getTag()))
    {
        //CCLog("%s completed", response->getHttpRequest()->getTag());
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode,
            response->getHttpRequest()->getTag());
    CCLog("response code: %d", statusCode);
    
    if (!response->isSucceed())
    {
        CCLabelTTF *notConnectLabel =
        CCLabelTTF::create("PLEASE CHECK YOUR INTERNET CONNECTION", "Time new roman",
                           30);
        notConnectLabel->setPosition(ccp(size.width / 2, size.height / 2));
        this->addChild(notConnectLabel);
        return;
    }
    
    // dump data
    std::vector<char> *buffer = response->getResponseData();
    char * data2 = (char*)(malloc(buffer->size() *  sizeof(char)));
    int d = -1;
    printf("Http Test, dump data: ");
    for (unsigned int i = 0; i < buffer->size(); i++)
    {
        d++ ;
        data2[d] = (*buffer)[i];
    }
    data2[d + 1] = '\0';
    //-----------------------
    int dem = 0;
    rapidjson::Document document;
    if(data2 != NULL && !document.Parse<0>(data2).HasParseError())
    {   
        for (rapidjson::SizeType  i = 0; i < document.Size(); i++)
        {
            CCLabelTTF *nameLabel = CCLabelTTF::create(document[i]["name"].GetString(),
                                                           "Time New Roman", 50);
            nameLabel->setAnchorPoint(ccp(0, 0)) ;
            nameLabel->setPosition(ccp(size.width / 6,
                                       size.height * (12 - dem) / 15));
            this->addChild(nameLabel);
            char strP [20] = {0};
            sprintf(strP,"%i", document[i]["point"].GetInt());
            CCLabelTTF *pointLabel = CCLabelTTF::create(strP,
                                                            "Time New Roman", 50);
            pointLabel->setAnchorPoint(ccp(1, 0));
            pointLabel->setPosition(ccp(4 * size.width / 5,
                                        size.height * (12 - dem) / 15));
            this->addChild(pointLabel);
            dem++;
            }
        }
        else
        {
            CCLog(document.GetParseError());
        }
        d = -1;
        delete []data2;
}
void RankingScene::menuMenu(CCObject* pSender)
{
    CCDirector::sharedDirector()->replaceScene(Menu::scene());
}
void RankingScene::menuPlay(CCObject* pSender)
{
    if (GameManager::sharedGameManager()->getPoint() != 0) {
        CCScene *GamePlayScene = GamePlay::scene();
        CCDirector::sharedDirector()->replaceScene(GamePlayScene);
    }else {
        CCDirector::sharedDirector()->replaceScene(PlayerName::scene());
    }
}
void RankingScene::local(CCObject *pSender)
{
    CCDirector::sharedDirector()->replaceScene(LocalRankScene::scene());
}