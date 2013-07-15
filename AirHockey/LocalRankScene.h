//
//  LocalRankScene.h
//  AirHockey
//
//  Created by macbook_016 on 2013/07/09.
//
//

#ifndef __AirHockey__LocalRankScene__
#define __AirHockey__LocalRankScene__

#include <iostream>
#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCTableView.h"
#include "CCScrollView.h"
#include "CCTableViewCell.h"
#include <set>
#include <vector>

NS_CC_EXT_BEGIN

using namespace cocos2d;
using namespace cocos2d::extension;

class LocalRankScene : public CCLayer,public CCTableViewDelegate,CCTableViewDataSource
{
private: CCArray *players = new CCArray();
public:
    // Method 'init' in cocos2d-x returns bool, instead of 'id' in cocos2d-iphone (an object pointer)
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommend to return the class instance pointer
    static CCScene* scene();
    
    //
//    void toExtensionsMainLayer(CCObject *sender);

    //
    void menuMenu(CCObject* pSender);
    void menuRanking(CCObject *pSender);
    void menuPlay(CCObject *pSender);
    
    // preprocessor macro for "static create()" constructor ( node() deprecated )
    CREATE_FUNC(LocalRankScene);
    
    virtual void scrollViewDidScroll(CCScrollView* view){};
    virtual void scrollViewDidZoom(CCScrollView* view){}
    virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell);
    virtual cocos2d::CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
    virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(CCTableView *table);
    
    
};

NS_CC_EXT_END
#endif /* defined(__AirHockey__LocalRankScene__) */
