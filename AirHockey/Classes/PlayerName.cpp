//
//  PlayerName.cpp
//  AirHockey
//
//  Created by macbook_016 on 2013/06/21.
//
//

#include "PlayerName.h"
#include "GameManager.h"
#include "HelloWorldScene.h"

using namespace cocos2d;
CCScene* PlayerName::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    PlayerName *layer = PlayerName::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}
bool PlayerName::init()
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCPoint visibleOrigin = CCEGLView::sharedOpenGLView()->getVisibleOrigin();
    CCSize visibleSize = CCEGLView::sharedOpenGLView()->getVisibleSize();

    CCSize editBoxSize = CCSizeMake(visibleSize.width - 100, 60);
    
    // top
    m_pEditName = extension::CCEditBox::create(editBoxSize, extension::CCScale9Sprite::create("green_edit.png"));
    m_pEditName->setPosition(ccp(visibleOrigin.x + visibleSize.width / 2,
                                 visibleOrigin.y+visibleSize.height * 3 / 4));
    m_pEditName->setFontSize(40);
    m_pEditName->setFontColor(ccRED);
    m_pEditName->setPlaceHolder("Name:");
    m_pEditName->setPlaceholderFontColor(ccWHITE);
    m_pEditName->setMaxLength(15);
    m_pEditName->setReturnType(cocos2d::extension::kKeyboardReturnTypeDone);
    m_pEditName->setDelegate(this);
    this->addChild(m_pEditName);
    CCMenuItemFont *mediumMenuItem = CCMenuItemFont::create("MEDIUM",
                                                            this,
                                                            menu_selector(PlayerName::menuMedium));
    mediumMenuItem->setPosition(ccp(size.width / 2, size.height * 0.4));
    mediumMenuItem->setFontSizeObj(70);
    mediumMenuItem->setFontSize(70);
    CCMenuItemFont *easyMenuItem = CCMenuItemFont::create("EASY",
                                                          this,
                                                          menu_selector(PlayerName::menuEasy));
    easyMenuItem->setPosition(ccp(size.width / 2, size.height * 0.6));
    CCMenuItemFont *hardMenuItem = CCMenuItemFont::create("HARD",
                                                          this,
                                                          menu_selector(PlayerName::menuHard));
    hardMenuItem->setPosition(ccp(size.width / 2, size.height * 0.2));
    pMenu = CCMenu::create(easyMenuItem, mediumMenuItem, hardMenuItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
    
    return true;
}
void PlayerName::editBoxEditingDidBegin(cocos2d::extension::CCEditBox* editBox)
{
    CCLog("editBox %p DidBegin !", editBox);
}

void PlayerName::editBoxEditingDidEnd(cocos2d::extension::CCEditBox* editBox)
{
    CCLog("editBox %p DidEnd !", editBox);
}

void PlayerName::editBoxTextChanged(cocos2d::extension::CCEditBox* editBox, const std::string& text)
{
    CCLog("editBox %p TextChanged, text: %s ", editBox, text.c_str());
}

void PlayerName::editBoxReturn(cocos2d::extension::CCEditBox* editBox)
{
    CCLog("editBox %p was returned !");
    GameManager::sharedGameManager()->setName(m_pEditName->getText());
}
void PlayerName::menuHard(CCObject *pSender)
{
    if (m_pEditName->getText() != "") {
        GameManager::sharedGameManager()->setLever(3);
        CCScene *helloScene = HelloWorld::scene();
        CCDirector::sharedDirector()->replaceScene(helloScene);
    }
}
void PlayerName::menuMedium(CCObject *pSender)
{
    if (m_pEditName->getText() != "") {
        GameManager::sharedGameManager()->setLever(3);
        CCScene *helloScene = HelloWorld::scene();
        CCDirector::sharedDirector()->replaceScene(helloScene);
    }
}
void PlayerName::menuEasy(CCObject *pSender)
{
    if (m_pEditName->getText() != "") {
        GameManager::sharedGameManager()->setLever(3);
        CCScene *helloScene = HelloWorld::scene();
        CCDirector::sharedDirector()->replaceScene(helloScene);
    }
}

