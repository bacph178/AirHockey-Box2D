//
//  GamePlay.h
//  AirHockey
//
//  Created by Trung Kien Do on 13/06/12.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#ifndef __GAME_PLAY_H__
#define __GAME_PLAY_H__

// When you import this file, you import all the cocos2d classes
#include "cocos2d.h"
#include "Box2D.h"
#include "MyContactListener.h"
#include "GameSprite.h"

#define PTM_RATIO 32

USING_NS_CC;

class PhysicsSprite : public cocos2d::CCSprite
{
public:
    PhysicsSprite();
    void setPhysicsBody(b2Body * body);
    virtual bool isDirty(void);
    virtual cocos2d::CCAffineTransform nodeToParentTransform(void);
private:
    b2Body* m_pBody;    // strong ref
};

class GamePlay : public cocos2d::CCLayer {
    CCSize _screenSize;
public:
    ~GamePlay();
    GamePlay();
    
    // returns a Scene that contains the GamePlay as the only child
    static cocos2d::CCScene* scene();
    
    void initPhysics();
    // adds a new sprite at a given coordinate
    void addNewSpriteAtPosition(cocos2d::CCPoint p);
    void gameReset();
    void playerScore(int player);
    void createEdge(float x1, float y1, float x2, float y2, int groupIndex);
    
    virtual void draw();
    virtual void ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    virtual void ccTouchesEnded(cocos2d::CCSet* touches, cocos2d::CCEvent* event);
    
    void update(float dt);
    
    void handleProcess();
    void wander();
    void defenseCenter();
    void defenseLeft();
    void defenseRight();
    void attack();
    
    void _moveTo(float ox, float oy, float px, float py);
    
    b2Vec2 ptm(CCPoint point) {
        return b2Vec2(point.x / PTM_RATIO, point.y / PTM_RATIO);
    }
    b2Vec2 ptm2(float x, float y) {
        return b2Vec2(x / PTM_RATIO, y / PTM_RATIO);
    }
    
    void updateTime(float dt);
    
    void addBgWin();
    void addBgLose();
    void addBgPause();

    void moveBgWin(int i);
    void moveBgLose(int i);
    void moveBgPause(int i);
    void rePlay();
    void drawReflectedRay(b2Vec2 p1, b2Vec2 p2);
    void menuReplay(CCObject *psceene);
    void menuMenu(CCObject *psceene);
    void menuPause(CCObject *psceene);
    void checkHightScore();
    void onHttpRequestCompleted(CCNode *sender, void *data);
    
private:
    CCSize s = CCDirector::sharedDirector()->getWinSize();
    float h = s.height;
    float w = s.width;
    
    
    b2Body *_groundBody;
    b2Body *_midleLineBody;
    b2Body *_player1Body;
    b2Body *_player2Body;
    b2Body *_ballBody;
    
    b2Fixture *_player1Fixture;
    b2Fixture *_player2Fixture;
    b2Fixture *_ballFixture;

    b2Fixture *lowerMiddle;
    b2Fixture *higherMiddle;
    
    GameSprite *_player1;
    GameSprite *_player2;
    CCSprite *_ball;
    
    CCSprite *_player1ScoreLabel1;
    CCSprite *_player1ScoreLabel2;
    CCSprite *_player2ScoreLabel1;
    CCSprite *_player2ScoreLabel2;
    
    
    
    int _player1Score = 0;
    int _sc1Tens;
    int _sc1SingleDigit;
    int _player2Score = 0;
    int _sc2Tens;
    int _sc2SingleDigit;
    
    int lastHit = 0;
    float m_radius1;
    float m_radius2;
    float m_radius3;
    
    bool p1touched = false;
    bool p2touched = false;
    bool attacked = false;
    
    CCArray *_players;
    
    b2World* world;
    cocos2d::CCTexture2D* m_pSpriteTexture; // weak ref
    
    b2MouseJoint *_mouseJoint;
    
    MyContactListener *_contactListener;
    
    int minutes;
	int seconds;
    bool playing;
	cocos2d:: CCSprite *spriteTime ;
    
    //when finish
    CCSprite *bgWin;
    CCSprite *bgLose;
    CCMenuItemImage *replayWin;
    CCMenuItemImage *replayLose;
    CCMenuItemImage *menuWin;
    CCMenuItemImage *menuLose;
    CCMenu *menuFinish;
    CCLabelTTF *lb_point;
    bool win ;
    bool lose ;
    //when pause
    int pauseGame;
    CCMenu *menuMenuPause;
    CCSprite *bgPause;
    CCMenuItemImage *replayPause;
    CCMenuItemImage *menuMnPause;
    CCMenuItemImage *menuItemImagePause;
};

#endif // __GAME_PLAY_H__
