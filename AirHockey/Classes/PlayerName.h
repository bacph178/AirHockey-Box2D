//
//  PlayerName.h
//  AirHockey
//
//  Created by macbook_016 on 2013/06/21.
//
//

#ifndef __AirHockey__PlayerName__
#define __AirHockey__PlayerName__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
using namespace cocos2d;
class PlayerName : public CCLayer, public extension::CCEditBoxDelegate{
private:
    extension::CCEditBox* m_pEditName;
    CCLabelTTF* m_pTTFShowEditReturn;
    CCMenu* pMenu;
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static CCScene* scene();
    void toExtensionsMainLayer(CCObject *sender);
    virtual void editBoxEditingDidBegin(extension::CCEditBox* editBox);
    virtual void editBoxEditingDidEnd(extension::CCEditBox* editBox);
    virtual void editBoxTextChanged(extension::CCEditBox* editBox, const std::string& text);
    virtual void editBoxReturn(extension::CCEditBox* editBox);
    void menuMedium(CCObject *pSender);
    void menuEasy(CCObject *pSender);
    void menuHard(CCObject *pSender);

    
    // a selector callback
    CREATE_FUNC(PlayerName);
};


#endif /* defined(__AirHockey__PlayerName__) */
