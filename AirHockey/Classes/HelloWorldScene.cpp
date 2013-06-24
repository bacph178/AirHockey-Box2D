//
//  HelloWorldScene.cpp
//  AirHockey
//
//  Created by Trung Kien Do on 13/06/12.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameSprite.h"
#include "GameManager.h"
using namespace cocos2d;
using namespace CocosDenshion;

#define PTM_RATIO 32

enum {
    kTagParentNode = 1,
};

PhysicsSprite::PhysicsSprite()
: m_pBody(NULL)
{

}

void PhysicsSprite::setPhysicsBody(b2Body * body)
{
    m_pBody = body;
}

// this method will only get called if the sprite is batched.
// return YES if the physics values (angles, position ) changed
// If you return NO, then nodeToParentTransform won't be called.
bool PhysicsSprite::isDirty(void)
{
    return true;
}

// returns the transform matrix according the Chipmunk Body values
CCAffineTransform PhysicsSprite::nodeToParentTransform(void)
{
    b2Vec2 pos  = m_pBody->GetPosition();

    float x = pos.x * PTM_RATIO;
    float y = pos.y * PTM_RATIO;

    if ( isIgnoreAnchorPointForPosition() ) {
        x += m_obAnchorPointInPoints.x;
        y += m_obAnchorPointInPoints.y;
    }

    // Make matrix
    float radians = m_pBody->GetAngle();
    float c = cosf(radians);
    float s = sinf(radians);

    if( ! m_obAnchorPointInPoints.equals(CCPointZero) ){
        x += c*-m_obAnchorPointInPoints.x + -s*-m_obAnchorPointInPoints.y;
        y += s*-m_obAnchorPointInPoints.x + c*-m_obAnchorPointInPoints.y;
    }

    // Rot, Translate Matrix
    m_sTransform = CCAffineTransformMake( c,  s,
        -s,    c,
        x,    y );

    return m_sTransform;
}

HelloWorld::HelloWorld()
{
    setTouchEnabled( true );
    setAccelerometerEnabled( true );
    _mouseJoint = NULL;
    // init physics
    this->initPhysics();

    //------------------Time init--------------------------------------
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    this->minutes = 2;
    this->seconds = 60;
    this->playing = true;
    
    char strTime[20] = {0};
	sprintf(strTime, "0%i:0%i", minutes,seconds);

	CCTexture2D *texTime = new CCTexture2D();
	texTime->initWithString(strTime, "Times New Roman", 34);
	spriteTime = CCSprite::createWithTexture(texTime);
	spriteTime->setPosition(ccp(145/2, size.height * 0.5));
    spriteTime->setScale(1.4);
    
    CCRotateTo *rotate = CCRotateTo::create(0, 90 * 3);
    spriteTime->runAction(rotate);
	this->addChild(spriteTime, 10);
    
    this->schedule(schedule_selector(HelloWorld::updateTime), 1);
    //-----------------------------------------------------------------
    
    
    CCSpriteBatchNode *parent = CCSpriteBatchNode::create("blocks.png", 100);
    m_pSpriteTexture = parent->getTexture();

    addChild(parent, 0, kTagParentNode);


    _player1Score = 0;
    _player2Score = 0;
    _screenSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSprite *court = CCSprite::create("background2.png");
    court->setPosition(ccp(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(court);
    
    _player1 = GameSprite::gameSpriteWidthFile("player1.png");
    _player1->setTag(99);
    this->addChild(_player1, 1);
    b2BodyDef player1BodyDef;
    player1BodyDef.type = b2_dynamicBody;
    player1BodyDef.position.Set(_screenSize.width * 0.5 / PTM_RATIO,
                                _player1->getContentSize().width / PTM_RATIO );
    player1BodyDef.userData = _player1;
    player1BodyDef.linearDamping =5.0f;
    _player1Body = world->CreateBody(&player1BodyDef);
    b2FixtureDef player1FixtureDef;
    b2CircleShape circle1;
    float m_radius1 = _player1->getContentSize().height/2;
        CCLOG("radius2: %f", m_radius1);
    circle1.m_radius = m_radius1/PTM_RATIO;
    player1FixtureDef.shape = &circle1;
    player1FixtureDef.density = 1.1f;
    player1FixtureDef.friction = 0.3f;
    player1FixtureDef.restitution = 0.0f;
    _player1Fixture = _player1Body->CreateFixture(&player1FixtureDef);
    
    
    _player2 = GameSprite::gameSpriteWidthFile("player2.png");
    _player2->setTag(99);
    this->addChild(_player2, 1);
    b2BodyDef player2BodyDef;
    player2BodyDef.type = b2_dynamicBody;
    player2BodyDef.position.Set(_screenSize.width * 0.5 / PTM_RATIO,
                                (_screenSize.height -
                                 _player2->getContentSize().width) / PTM_RATIO);
    player2BodyDef.userData = _player2;
    player2BodyDef.linearDamping =5.0f;
    _player2Body = world->CreateBody(&player2BodyDef);
    b2FixtureDef player2FixtureDef;
    b2CircleShape circle2;
    float m_radius2 = _player2->getContentSize().height/2;
    circle2.m_radius = m_radius2/PTM_RATIO;
    player2FixtureDef.shape = &circle2;
    player2FixtureDef.density = 1.1f;
    player2FixtureDef.friction = 0.3f;
    player2FixtureDef.restitution=  0.0f;
    _player2Fixture = _player2Body->CreateFixture(&player2FixtureDef);
    
    
    _ball = CCSprite::create("ball.png");
    this->addChild(_ball, 1, 10);
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
    ballBodyDef.linearDamping = 0.3f;
    ballBodyDef.position.Set(_screenSize.width/2/PTM_RATIO,
                             _screenSize.height/2/PTM_RATIO);
    ballBodyDef.userData = _ball;
    _ballBody = world->CreateBody(&ballBodyDef);
    b2FixtureDef ballFixtureDef;
    b2CircleShape circle3;
    float m_radius3 = _ball->getContentSize().height/2;
    circle3.m_radius = m_radius3/PTM_RATIO;
    ballFixtureDef.shape = &circle3;
    ballFixtureDef.density = 1.0f;
    ballFixtureDef.friction = 0.2f;
    ballFixtureDef.restitution = 1.0f;
    ballFixtureDef.filter.groupIndex = -10;
    _ballFixture = _ballBody->CreateFixture(&ballFixtureDef);
    
    _players = CCArray::create(_player1, _player2, NULL);
    _players->retain();
    
    CCSpriteBatchNode *counter = CCSpriteBatchNode::create("counter.pvr.ccz");
    this->addChild(counter);
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("counter.plist");
    
    _player1ScoreLabel1 = CCSprite::createWithSpriteFrameName("0.png");
    _player1ScoreLabel1->setScale(0.7);
    _player1ScoreLabel1->setPosition(ccp(_screenSize.width - 115,
                                        _screenSize.height * 0.5 - 125));
    _player1ScoreLabel1->setRotation(90);
    this->addChild(_player1ScoreLabel1);
    _player1ScoreLabel2 = CCSprite::createWithSpriteFrameName("0.png");
    _player1ScoreLabel2->setScale(0.7);
    _player1ScoreLabel2->setPosition(ccp(_screenSize.width - 115,
                                         _screenSize.height * 0.5 - 95));
    _player1ScoreLabel2->setRotation(90);
    this->addChild(_player1ScoreLabel2);
    
    _player2ScoreLabel1 = CCSprite::createWithSpriteFrameName("0.png");
    _player2ScoreLabel1->setScale(0.7);
    _player2ScoreLabel1->setPosition(ccp(_screenSize.width - 115,
                                        _screenSize.height * 0.5 + 100));
    _player2ScoreLabel1->setRotation(90);
    this->addChild(_player2ScoreLabel1);
    _player2ScoreLabel2 = CCSprite::createWithSpriteFrameName("0.png");
    _player2ScoreLabel2->setScale(0.7);
    _player2ScoreLabel2->setPosition(ccp(_screenSize.width - 115,
                                         _screenSize.height * 0.5 + 130));
    _player2ScoreLabel2->setRotation(90);
    this->addChild(_player2ScoreLabel2);

    
    scheduleUpdate();
}

HelloWorld::~HelloWorld()
{
    delete world;
    world = NULL;
    
    //delete m_debugDraw;
}



void HelloWorld::initPhysics()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    world->SetContinuousPhysics(true);

    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);
    _groundBody = world->CreateBody(&groundBodyDef);
    
    HelloWorld::createEdge(0, 70, 260, 70, 0);
    HelloWorld::createEdge(s.width - 260, 70, s.width, 70, 0);
    HelloWorld::createEdge(260, 70, s.width - 260, 70, -10);
    HelloWorld::createEdge(260, 70, 260, 0, 0);
    HelloWorld::createEdge(s.width - 260, 70, s.width - 260, 0, 0);
    HelloWorld::createEdge(0, s.height - 70, 260, s.height - 70, 0);
    HelloWorld::createEdge(s.width - 260, s.height - 70, s.width, s.height - 70, 0);
    HelloWorld::createEdge(260, s.height - 70, s.width - 260, s.height - 70, -10);
    HelloWorld::createEdge(260, s.height - 70, 260, s.height, 0);
    HelloWorld::createEdge(s.width - 260, s.height - 70, s.width - 260, s.height, 0);
    HelloWorld::createEdge(55, s.height, 55, 0, 0);
    HelloWorld::createEdge(s.width - 45, s.height, s.width - 45, 0, 0);
    HelloWorld::createEdge(0, s.height/2, s.width, s.height/2, -10);
}

void HelloWorld::createEdge(float x1, float y1,
                            float x2, float y2,
                            int groupIndex) {
    b2EdgeShape groundEdgeShape;
    groundEdgeShape.Set(b2Vec2(x1 / PTM_RATIO, y1 / PTM_RATIO),
                        b2Vec2(x2 / PTM_RATIO, y2 / PTM_RATIO));
    b2FixtureDef groundEdgeDef;
    groundEdgeDef.shape = &groundEdgeShape;
    groundEdgeDef.filter.groupIndex = groupIndex;
    _groundBody->CreateFixture(&groundEdgeDef);
}


void HelloWorld::draw()
{
    //
    // IMPORTANT:
    // This is only for debug purposes
    // It is recommend to disable it
    //
    CCLayer::draw();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

    kmGLPushMatrix();

    world->DrawDebugData();

    kmGLPopMatrix();
}


void HelloWorld::update(float dt)
{
    if (this->playing == true) {
        int velocityIterations = 8;
        int positionIterations = 3;
        
        
        // Instruct the world to perform a single step of simulation. It is
        // generally best to keep the time step and iterations fixed.
        world->Step(dt, velocityIterations, positionIterations);
        
        //Iterate over the bodies in the physics world
        for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
        {
            if (b->GetUserData() != NULL) {
                CCSprite* myActor = (CCSprite*)b->GetUserData();
                myActor->setPosition( CCPointMake( b->GetPosition().x * PTM_RATIO,
                                                  b->GetPosition().y * PTM_RATIO) );
                if (myActor->getTag() == 99) {
                    b->SetLinearVelocity(b2Vec2(0, 0));
                    b->SetFixedRotation(true);
                }
                
            }
        }
        
        if (_ball->getPositionY() >= s.height-55) {
            HelloWorld::playerScore(1);
            HelloWorld::gameReset();
        }
        
        if (_ball->getPositionY() <= 55) {
            HelloWorld::playerScore(2);
            HelloWorld::gameReset();
        }

    }
    
//    if (_ballY > 55 && _ballY < s.height * 2 / 3 &&
//        _ballX > s.width * 2 / 5 && _ballX < s.width * 3 / 5) {
//        _player2Body->SetTransform(b2Vec2(_ballX / PTM_RATIO,
//                                          _player2->getPositionY()/PTM_RATIO), 0);
//    }
    
    if (_ballY > s.height / 2) {
        if (_player2->getPositionY() > s.height / 2)
            _player2Body->SetLinearVelocity(
                b2Vec2((_ballX - _player2->getPositionX())/10,
                       (_ballY - _player2->getPositionY())/10));
        else _player2Body->SetLinearVelocity(
                b2Vec2((- _ballX + _player2->getPositionX())/10,
                       (- _ballY + _player2->getPositionY())/10));
    }
}

void HelloWorld::ccTouchesBegan(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    if (_mouseJoint != NULL) return;
    CCTouch *touch = (CCTouch*)touches->anyObject();
    CCPoint tap = touch->getLocation();
    b2Vec2 locationWorld = b2Vec2(tap.x / PTM_RATIO, tap.y / PTM_RATIO);
    
    if (_player1Fixture->TestPoint(locationWorld)) {
        b2MouseJointDef md;
        md.bodyA = _groundBody;
        md.bodyB = _player1Body;
        md.target = locationWorld;
        md.collideConnected = true;
        md.maxForce = 100000.0f * _player1Body->GetMass();
        md.dampingRatio = 0;
        md.frequencyHz = 1000;
        
        _mouseJoint = (b2MouseJoint *)world->CreateJoint(&md);
        _player1Body->SetAwake(true);
    }
}

void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches, cocos2d::CCEvent* event)
{
    if (_mouseJoint == NULL) return;
    CCTouch  *myTouch = (CCTouch *)touches->anyObject();
    CCPoint location = myTouch->getLocation();
    b2Vec2 locationWorld = b2Vec2(location.x / PTM_RATIO, location.y / PTM_RATIO);
    _mouseJoint->SetTarget(locationWorld);
}

void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event) {
    if (_mouseJoint != NULL) {
        world->DestroyJoint(_mouseJoint);
        _mouseJoint = NULL;
    }
}

void HelloWorld::playerScore(int player)
{
    if (GameManager::sharedGameManager()->getBgm())
    {
        SimpleAudioEngine::sharedEngine()->playEffect("score.wav");

    }
    char scoreBuff[10];
    
    if (player == 1) {
        _player1Score++;
        _sc1Tens = _player1Score % 10;
        _sc1SingleDigit = _player1Score / 10;
        sprintf(scoreBuff, "%d.png", _sc1Tens);
        CCSprite *tens = CCSprite::createWithSpriteFrameName(scoreBuff);
        _player1ScoreLabel1->setTextureRect(tens->getTextureRect());
        _player1ScoreLabel1->setTexture(tens->getTexture());
        sprintf(scoreBuff, "%d.png", _sc1SingleDigit);
        CCSprite *singleDigit = CCSprite::createWithSpriteFrameName(scoreBuff);
        _player1ScoreLabel2->setTextureRect(singleDigit->getTextureRect());
        _player1ScoreLabel2->setTexture(singleDigit->getTexture());
        
        
        _ballBody->SetLinearVelocity(b2Vec2(0, 0));
        _ballBody->SetTransform(b2Vec2(_screenSize.width/2/PTM_RATIO,
                                       (_screenSize.height/2 +
                                        _ball->getContentSize().width/2)/PTM_RATIO), 0);
    }
    if (player == 2) {
        _player2Score++;
        _sc2Tens = _player2Score % 10;
        _sc2SingleDigit = _player2Score / 10;
        sprintf(scoreBuff, "%d.png", _sc2Tens);
        CCSprite *tens = CCSprite::createWithSpriteFrameName(scoreBuff);
        _player2ScoreLabel1->setTextureRect(tens->getTextureRect());
        _player2ScoreLabel1->setTexture(tens->getTexture());
        sprintf(scoreBuff, "%d.png", _sc2SingleDigit);
        CCSprite *singleDigit = CCSprite::createWithSpriteFrameName(scoreBuff);
        _player2ScoreLabel2->setTextureRect(singleDigit->getTextureRect());
        _player2ScoreLabel2->setTexture(singleDigit->getTexture());
        
        _ballBody->SetLinearVelocity(b2Vec2(0, 0));
        _ballBody->SetTransform(b2Vec2(_screenSize.width/2/PTM_RATIO,
                                       (_screenSize.height/2 -
                                        _ball->getContentSize().width/2)/PTM_RATIO), 0);
    }
    
}

void HelloWorld::gameReset()
{
    this->playing = true;
    _player1Body->SetLinearVelocity(b2Vec2(0, 0));
    _player1Body->SetTransform(b2Vec2(_screenSize.width/2/PTM_RATIO,
                                      _player1->getContentSize().width/PTM_RATIO), 0);
    
    _player2Body->SetLinearVelocity(b2Vec2(0, 0));
    _player2Body->SetTransform(b2Vec2(_screenSize.width/2/PTM_RATIO,
                                      (_screenSize.height -
                                       _player2->getContentSize().width)/PTM_RATIO), 0);

    if (_mouseJoint != NULL) {
        world->DestroyJoint(_mouseJoint);
        _mouseJoint = NULL;
    }
}

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    CCLayer* layer = new HelloWorld();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}

void HelloWorld::updateTime(float dt) {
    if(playing == true && minutes >= 0) {
		if(seconds > 0)	seconds--;
		else {
			if(minutes > 0) minutes--;
			else  minutes = 0;
			seconds=60;
		}
	}
    
	char strTime[20] = {0};
	if(minutes < 10 && seconds < 10)
        sprintf(strTime, "0%i:0%i", minutes, seconds);
	else if(minutes < 10 && seconds >= 10)
        sprintf(strTime, "0%i:%i", minutes, seconds);
    
	CCTexture2D *texTime=new CCTexture2D();
	texTime->initWithString(strTime, "Times New Roman", 34);
	spriteTime->setTexture(texTime);
    
    if (minutes == 0 && seconds == 0) {
        this->unscheduleAllSelectors();
        this->unscheduleUpdate();
    }
}
