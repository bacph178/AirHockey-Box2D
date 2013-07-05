//
//  GamePlayScene.cpp
//  AirHockey
//
//  Created by Trung Kien Do on 13/06/12.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#include "GamePlay.h"
#include "SimpleAudioEngine.h"
#include "Menu.h"
#include "GameManager.h"
#include "HttpClient.h"
#include "rapidjson.h"
#include "document.h"
#include "InputEmail.h"
using namespace cocos2d;
using namespace CocosDenshion;
using namespace cocos2d::extension;
using namespace std;

#define PTM_RATIO 32

#pragma mark GamePlay Init

GamePlay::GamePlay()
{
    srand(time(NULL));
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
    this->win = false ;
    this->lose = false ;
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
    

    this->schedule(schedule_selector(GamePlay::updateTime), 1);
    
    //------------------add backgroungd finish-------------------------
    pauseGame = 1;
    this->addBgLose();
    this->addBgWin();
    this->addBgPause();
    
    menuFinish = CCMenu::create(replayWin, menuWin, replayLose, menuLose,
                             menuMnPause, replayPause, NULL);
    menuFinish->setPosition(0, size.height/6);
    this->addChild(menuFinish, 30);
    menuMenuPause = CCMenu::create(menuItemImagePause, NULL);
    menuMenuPause->setPosition(ccp(0, 0));
    this->addChild(menuMenuPause,0);
    //-----------------------------------------------------------------


    _player1Score = 0;
    _player2Score = 0;
    
    CCSprite *court = CCSprite::create("court.png");
    court->setPosition(ccp(s.width * 0.5, s.height * 0.5));
    this->addChild(court);
    
    _player1 = GameSprite::gameSpriteWidthFile("mallet1.png");
    _player1->setTag(99);
    this->addChild(_player1, 1);
    b2BodyDef player1BodyDef;
    player1BodyDef.type = b2_dynamicBody;
    player1BodyDef.position.Set(s.width * 0.5 / PTM_RATIO,
                                _player1->getContentSize().width / PTM_RATIO );
    player1BodyDef.userData = _player1;
    player1BodyDef.linearDamping = 5.0f;
    _player1Body = world->CreateBody(&player1BodyDef);
    b2FixtureDef player1FixtureDef;
    b2CircleShape circle1;
    m_radius1 = _player1->getContentSize().height/2;
    circle1.m_radius = m_radius1/PTM_RATIO;
    player1FixtureDef.shape = &circle1;
    player1FixtureDef.density = 5.0f;
    player1FixtureDef.friction = 0.3f;
    player1FixtureDef.restitution = 0.0f;
    _player1Fixture = _player1Body->CreateFixture(&player1FixtureDef);
    
    
    _player2 = GameSprite::gameSpriteWidthFile("mallet2.png");
    _player2->setTag(98);
    this->addChild(_player2, 1);
    b2BodyDef player2BodyDef;
    player2BodyDef.type = b2_dynamicBody;
    player2BodyDef.position.Set(
        s.width / 2 / PTM_RATIO,
        (s.height - _player2->getContentSize().height / 2) / PTM_RATIO);
    player2BodyDef.userData = _player2;
    player2BodyDef.linearDamping = 2.0f;
    _player2Body = world->CreateBody(&player2BodyDef);
    b2FixtureDef player2FixtureDef;
    b2CircleShape circle2;
    m_radius2 = _player2->getContentSize().height/2;
    circle2.m_radius = m_radius2/PTM_RATIO;
    player2FixtureDef.shape = &circle2;
    player2FixtureDef.density = 5.0f;
    player2FixtureDef.friction = 0.3f;
    player2FixtureDef.restitution = 0.1f;
    _player2Fixture = _player2Body->CreateFixture(&player2FixtureDef);
    
    
    _ball = CCSprite::create("puck.png");
    this->addChild(_ball, 1, 10);
    b2BodyDef ballBodyDef;
    ballBodyDef.type = b2_dynamicBody;
//    ballBodyDef.linearDamping = 1.0f;
    ballBodyDef.position.Set(s.width/2/PTM_RATIO,
                             s.height/2/PTM_RATIO);
    ballBodyDef.userData = _ball;
    ballBodyDef.bullet = true;
    _ballBody = world->CreateBody(&ballBodyDef);
    b2FixtureDef ballFixtureDef;
    b2CircleShape circle3;
    m_radius3 = _ball->getContentSize().height/2;
    circle3.m_radius = m_radius3/PTM_RATIO;
    ballFixtureDef.shape = &circle3;
    ballFixtureDef.density = 1.0f;
    ballFixtureDef.friction = 0.0f;
    ballFixtureDef.restitution = 0.7f;
    ballFixtureDef.filter.groupIndex = -10;
    _ballFixture = _ballBody->CreateFixture(&ballFixtureDef);
    
    _players = CCArray::create(_player1, _player2, NULL);
    _players->retain();
    
    CCSpriteBatchNode *counter = CCSpriteBatchNode::create("counter.pvr.ccz");
    this->addChild(counter);
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("counter.plist");
    
    _player1ScoreLabel1 = CCSprite::createWithSpriteFrameName("0.png");
    _player1ScoreLabel1->setScale(0.7);
    _player1ScoreLabel1->setPosition(ccp(s.width - 115,
                                        s.height * 0.5 - 125));
    _player1ScoreLabel1->setRotation(90);
    this->addChild(_player1ScoreLabel1);
    _player1ScoreLabel2 = CCSprite::createWithSpriteFrameName("0.png");
    _player1ScoreLabel2->setScale(0.7);
    _player1ScoreLabel2->setPosition(ccp(s.width - 115,
                                         s.height * 0.5 - 95));
    _player1ScoreLabel2->setRotation(90);
    this->addChild(_player1ScoreLabel2);
    
    _player2ScoreLabel1 = CCSprite::createWithSpriteFrameName("0.png");
    _player2ScoreLabel1->setScale(0.7);
    _player2ScoreLabel1->setPosition(ccp(s.width - 115,
                                        s.height * 0.5 + 100));
    _player2ScoreLabel1->setRotation(90);
    this->addChild(_player2ScoreLabel1);
    _player2ScoreLabel2 = CCSprite::createWithSpriteFrameName("0.png");
    _player2ScoreLabel2->setScale(0.7);
    _player2ScoreLabel2->setPosition(ccp(s.width - 115,
                                         s.height * 0.5 + 130));
    _player2ScoreLabel2->setRotation(90);
    this->addChild(_player2ScoreLabel2);
    

    schedule(schedule_selector(GamePlay::update));
    schedule(schedule_selector(GamePlay::handleProcess), 0.025);
}

GamePlay::~GamePlay()
{
    CC_SAFE_DELETE(world);
    _groundBody = NULL;
    delete _contactListener;
    
    //delete m_debugDraw;
}

#pragma mark Create Ground

void GamePlay::initPhysics()
{
    b2Vec2 gravity;
    gravity.Set(0.0f, 0.0f);
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    world->SetContinuousPhysics(true);
    _contactListener = new MyContactListener();
    world->SetContactListener(_contactListener);
    uint32 flags = 0;
    flags += b2Draw::e_shapeBit;
    
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0);
    _groundBody = world->CreateBody(&groundBodyDef);
    
    this->createEdge(0, 10, 210, 10, 0);
    this->createEdge(s.width - 210, 10, s.width, 10, 0);
    this->createEdge(210, 10, s.width - 210, 10, -10);
    this->createEdge(210, 10, 210, 0, 0);
    this->createEdge(s.width - 210, 10, s.width - 210, 0, 0);
    this->createEdge(0, s.height - 10, 210, s.height - 10, 0);
    this->createEdge(s.width - 210, s.height - 10, s.width, s.height - 10, 0);
    this->createEdge(210, s.height - 10, s.width - 210, s.height - 10, -10);
    this->createEdge(210, s.height - 10, 210, s.height, 0);
    this->createEdge(s.width - 210, s.height - 10, s.width - 210, s.height, 0);
    this->createEdge(10, s.height, 10, 0, 0);                                                                                                                                   
    this->createEdge(s.width - 10, s.height, s.width - 10, 0, 0);
//    this->createEdge(0, s.height/2, s.width, s.height/2, -10);
    
    
    b2BodyDef midleLineDef;
    midleLineDef.position.Set(0, 0);
    _midleLineBody = world->CreateBody(&midleLineDef);
    b2EdgeShape midleEdgeShape;
    midleEdgeShape.Set(b2Vec2(0, s.height / 2 / PTM_RATIO),
                        b2Vec2(s.width / PTM_RATIO, s.height / 2 / PTM_RATIO));
    b2FixtureDef midleEdgeDef;
    midleEdgeDef.shape = &midleEdgeShape;
    midleEdgeDef.filter.groupIndex = -10;
    _midleLineBody->CreateFixture(&midleEdgeDef);
    
}

void GamePlay::createEdge(float x1, float y1,
                          float x2, float y2,
                          int groupIndex) {
    b2EdgeShape groundEdgeShape;
    groundEdgeShape.Set(b2Vec2(x1 / PTM_RATIO, y1 / PTM_RATIO),
                        b2Vec2(x2 / PTM_RATIO, y2 / PTM_RATIO));
    b2FixtureDef groundEdgeDef;
    groundEdgeDef.shape = &groundEdgeShape;
    groundEdgeDef.restitution = 0.9;
    groundEdgeDef.filter.groupIndex = groupIndex;
    _groundBody->CreateFixture(&groundEdgeDef);
}

#pragma mark DRAW
void GamePlay::draw()
{
    CCLayer::draw();
    
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position );

    kmGLPushMatrix();

    world->DrawDebugData();

    kmGLPopMatrix();
    

}

#pragma mark Update

void GamePlay::update(float dt)
{
    if (this->playing == true && pauseGame == 1) {
        int velocityIterations = 8;
        int positionIterations = 3;
        
        world->Step(dt, velocityIterations, positionIterations);
        
        for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
        {
            if (b->GetUserData() != NULL) {
                CCSprite* myActor = (CCSprite*)b->GetUserData();
                myActor->setPosition( CCPointMake(b->GetPosition().x * PTM_RATIO,
                                                  b->GetPosition().y * PTM_RATIO) );
                if (myActor->getTag() == 99) {
                    b->SetLinearVelocity(b2Vec2(0, 0));
                    b->SetFixedRotation(true);
                }
                if (myActor->getTag() == 98) {
                    if (
                    myActor->getPositionX() >= w / 2 - myActor->getContentSize().width
                    &&
                    myActor->getPositionX() <= w / 2 + myActor->getContentSize().width
                    &&
                    myActor->getPositionY() >= h - myActor->getContentSize().height / 2 - 20
                    &&
                    b->GetLinearVelocity().y  >= 0) {
                        b->SetAwake(false);
                        b->SetFixedRotation(true);
                    }
                    if (myActor->getPositionY() <= h / 2 + _ball->getContentSize().height) {
                        lastHit = 0;
                        this->defenseCenter();
                    }
                }
            }
        }
        #pragma mark Goal !!!
        if (_ball->getPositionY() >= s.height + _ball->getContentSize().height/2) {
            this->playerScore(1);
            this->gameReset();
        }
        
        if (_ball->getPositionY() <= -_ball->getContentSize().height/2) {
            this->playerScore(2);
            this->gameReset();
        }
    }
    
        #pragma mark When Finish Game
    if ((minutes == 0 && seconds == 0) || _player1Score == 1 || _player2Score == 1) {
        playing = false ;
        this->unscheduleAllSelectors() ;
        this->unscheduleUpdate() ;
        
        if (_player1Score > _player2Score) {
            this->moveBgWin(1) ;
            win = true;
            //---------send request to server ------------
            CCHttpRequest* request = new CCHttpRequest();
            int p = (_player1Score + 1) * (180 - (minutes * 60 + seconds)) *
            (GameManager::sharedGameManager()->getLevel() * 2000);
            GameManager::sharedGameManager()->setPoint(p);
            string name = GameManager::sharedGameManager()->getName();
            char strP[20] = {0};
            sprintf(strP, "%i", p);
            string url = "http://localhost:3000/users?name="+name+"&point="+strP+"&email=ngocduk54a2@gmail.com"+"&reward=0";
            request->setUrl(url.c_str());
            request->setRequestType(CCHttpRequest::kHttpPost);
            CCHttpClient::getInstance()->send(request);
            request->release();
            
            this->checkHightScore();
        }
        else if(_player1Score <= _player2Score) {
            this->moveBgLose(1) ;
            lose = true;
        }
        
    }

    std::vector<MyContact>::iterator pos;
    for(pos = _contactListener->_contacts.begin();
        pos != _contactListener->_contacts.end(); ++pos) {
        MyContact contact = *pos;
        
//        if ((_ballBody->GetLinearVelocity().y > 2 || _ballBody->GetLinearVelocity().x > 2) &&
//            (contact.fixtureA == _ballFixture || contact.fixtureB == _ballFixture)) {
//            SimpleAudioEngine::sharedEngine()->playEffect("hitPuck.wav");
//        }
        
        if ((contact.fixtureA == _ballFixture && contact.fixtureB == _player2Fixture) ||
            (contact.fixtureA == _player2Fixture && contact.fixtureB == _ballFixture)) {
//            b2Vec2 p1 = _ballBody->GetPosition();
//            b2Vec2 p2 = 1.1 * p1;
//            drawReflectedRay(p1, p2);
            lastHit = 0;
            this->defenseCenter();
        }
    }
    if (lastHit >= 450) {
        this->defenseCenter();
    }
}

#pragma mark AI Player

void GamePlay::handleProcess() {
    lastHit += 25;
    float x = _ball->getPositionX();
    float y = _ball->getPositionY();
    
    float vx = _ballBody->GetLinearVelocity().x;
    float vy = _ballBody->GetLinearVelocity().y;
    float br = _ball->getContentSize().width / 2;
    
    if (lastHit >= 450) {
        if ((y >= h / 2 - br && y <= 3 * h /4) ||
            (y > 3 * h / 4 && x > w / 2 && x < 3 * w / 4)) {
            this->attack();
        }
    } else {
        this->defenseCenter();
    }
    
}

void GamePlay::wander() {
    if (lastHit % 500 == 0) {
        float vx = rand() % 10000 - 5000;
        float vy = _player2->radius();
        
        _player2Body->ApplyLinearImpulse(this->ptm2(vx, vy),
                                         _player2Body->GetWorldCenter());
    }
}

void GamePlay::checkHightScore() {
    CCHttpRequest* request = new CCHttpRequest();
    request->setUrl("http://localhost:3000/users.json");
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, callfuncND_selector(GamePlay::onHttpRequestCompleted));
    CCHttpClient::getInstance()->send(request);
    request->release();
}
void GamePlay::onHttpRequestCompleted(CCNode *sender, void *data)
{
    CCSize size = CCDirector::sharedDirector()->getWinSize();
    CCHttpResponse *response = (CCHttpResponse*)data;
    
    if (!response)
    {
        return;
    }
    
    int statusCode = response->getResponseCode();
    char statusString[64] = {};
    sprintf(statusString, "HTTP Status Code: %d, tag = %s", statusCode,
            response->getHttpRequest()->getTag());
    
    if (!response->isSucceed())
    {
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
    rapidjson::Document document;
    if(data2 != NULL && !document.Parse<0>(data2).HasParseError())
    {
        string name = GameManager::sharedGameManager()->getName();
        int point = (_player1Score + 1) * (180 - (minutes * 60 + seconds)) *
           (GameManager::sharedGameManager()->getLevel() * 2000);
        for (rapidjson::SizeType  i = 0; i < document.Size(); i++)
        {
            if (document[i]["name"].GetString() == name &&
                point == document[i]["point"].GetInt() &&
                document[i]["reward"].GetInt() != 0) {
                CCDirector::sharedDirector()->replaceScene(InputEmail::scene());
                break;
            }
        }
    }
    else
    {
        CCLog(document.GetParseError());
    }
    d = -1;
    delete []data2;
}

void GamePlay::defenseLeft() {
    float px = _player2->getPositionX();
    float py = _player2->getPositionY();
    float br = _ball->getContentSize().width / 2;
    
    _player2Body->ApplyLinearImpulse(this->ptm2(7 * (w * 3 / 8 - px),
                                                7 * (h - br - 10 - py)),
                                     _player2Body->GetWorldCenter());
}

void GamePlay::defenseRight() {
    float px = _player2->getPositionX();
    float py = _player2->getPositionY();
    float br = _ball->getContentSize().width / 2;
    
    _player2Body->ApplyLinearImpulse(this->ptm2(7 * (w * 5 / 8 - px),
                                                7 * (h - br - 10  - py)),
                                     _player2Body->GetWorldCenter());
}

void GamePlay::defenseCenter() {
    float px = _player2->getPositionX();
    float py = _player2->getPositionY();
    float br = _ball->getContentSize().width / 2;
    
    _player2Body->ApplyLinearImpulse(this->ptm2(17 * (w / 2 - px),
                                                17 * (h - br - 10 - py)),
                                     _player2Body->GetWorldCenter());
}

void GamePlay::attack() {
    float x = _ball->getPositionX();
    float y = _ball->getPositionY();
    float px = _player2->getPositionX();
    float py = _player2->getPositionY();
    float vx = _ballBody->GetLinearVelocity().x;
    float vy = _ballBody->GetLinearVelocity().y;
    float br = _ball->getContentSize().width / 2;
    
    float cx = (h - 10 - br - y) * vx / vy + x;
    
    if ((cx > w / 4 && cx < w * 3 / 4) || (vx < 5 && vy < 5))
    _player2Body->ApplyLinearImpulse(b2Vec2(17*(x - px), 17*(10 + y - py)),
                                    _player2Body->GetWorldCenter());
    else _player2Body->SetLinearVelocity(b2Vec2((x/2 + w/4 - px) / 5, 0));
}

#pragma mark Touches Handle
void GamePlay::ccTouchesBegan(CCSet* touches, CCEvent* event) {
    if (_mouseJoint != NULL) return;
    CCTouch *touch = (CCTouch*)touches->anyObject() ;
    CCPoint tap = touch->getLocation();
    b2Vec2 locationWorld = this->ptm(tap);
    
    CCPoint location = tap;
    
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

void GamePlay::ccTouchesMoved(CCSet* touches, CCEvent* event) {
    if (_mouseJoint == NULL) return;
    CCTouch  *myTouch = (CCTouch *)touches->anyObject();
    CCPoint location = myTouch->getLocation();
    b2Vec2 locationWorld = this->ptm(location);
    _mouseJoint->SetTarget(locationWorld);

}

void GamePlay::ccTouchesEnded(CCSet* touches, CCEvent* event) {
    if (_mouseJoint != NULL) {
        world->DestroyJoint(_mouseJoint);
        _mouseJoint = NULL;
    }
}

#pragma mark Score Counter

void GamePlay::playerScore(int player) {
    if (GameManager::sharedGameManager()->getBgm()) {
        SimpleAudioEngine::sharedEngine()->playEffect("score.wav");
    }
    char scoreBuff[10];
    
    if (player == 1) {
        _player1Score++;
        _sc1Tens = _player1Score  % 10;
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
        _ballBody->SetTransform(
            this->ptm2(w/2, h/2 - _ball->getContentSize().height), 0);
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
        _ballBody->SetTransform(
            this->ptm2(w/2, h/2 - _ball->getContentSize().width), 0);
    }
    
}

#pragma mark Reset Game

void GamePlay::gameReset()
{
    this->playing = true;
    _player1Body->SetLinearVelocity(b2Vec2(0, 0));
    _player1Body->SetTransform(GamePlay::ptm2(s.width/2,
                    _player1->getContentSize().width), 0);
    
    _player2Body->SetLinearVelocity(b2Vec2(0, 0));

    _player2Body->SetTransform(GamePlay::ptm2(s.width/2,
                    s.height -_player2->getContentSize().width), 0);

    if (_mouseJoint != NULL) {
        world->DestroyJoint(_mouseJoint);
        _mouseJoint = NULL;
    }
}

#pragma mark RayCast
//new function for FooTest class
void GamePlay::drawReflectedRay(b2Vec2 p1, b2Vec2 p2)
{
    CCLabelTTF *inp     = CCLabelTTF::create("I", "Arial", 32);
    CCLabelTTF *outp    = CCLabelTTF::create("O", "Arial", 32);
    inp->setPosition(ccp(p1.x * PTM_RATIO, p1.y * PTM_RATIO));
    outp->setPosition(ccp(p2.x * PTM_RATIO, p2.y * PTM_RATIO));
    this->addChild(inp);
    this->addChild(outp);

    CCLOG("p1 & p2: (%f %f) (%f %f)",
       p1.x * PTM_RATIO,
       p1.y * PTM_RATIO,
       p2.x * PTM_RATIO,
       p2.y * PTM_RATIO);
    //set up input
    b2RayCastInput input;
    input.p1 = p1;
    input.p2 = p2;
    input.maxFraction = 5;

    float closestFraction = 5;
    b2Vec2 intersectionNormal(0,0);
    
    for (b2Fixture* f = _groundBody->GetFixtureList(); f;  f = f->GetNext()) {
        b2RayCastOutput output;
        if ( !f->RayCast( &output, input, 0) )
            continue;
        if ( output.fraction < closestFraction ) {
            closestFraction = output.fraction;
            CCLOG("Closest Fraction: %f", closestFraction);
            intersectionNormal = output.normal;
            CCLOG("Nomal Vector: %f %f", output.normal.x,
                  output.normal.y);
            b2Vec2 i = p1 + closestFraction * (p2 - p1);
            CCLOG("collision: (%f %f)",
                  i.x * PTM_RATIO,
                  i.y * PTM_RATIO
                  );
        }
    }
    
    b2Vec2 intersectionPoint = p1 + closestFraction * (p2 - p1);
    
    if ( closestFraction == 10 )
        return;
    if ( closestFraction == 0 )
        return;
    
    //still some ray left to reflect
    b2Vec2 remainingRay = (p1 + 5 * (p2 - p1) - intersectionPoint);
    b2Vec2 projectedOntoNormal = b2Dot(remainingRay, intersectionNormal) * intersectionNormal;
    b2Vec2 nextp2 = p2 - 2 * projectedOntoNormal;
//    CCLOG("Next: (%f %f)",
//          intersectionPoint.x * PTM_RATIO,
//          intersectionPoint.y * PTM_RATIO
//          );
    //recurse
    this->drawReflectedRay(intersectionPoint, nextp2);
}

void GamePlay::updateTime(float dt) {
    if(playing == true && minutes >= 0 && pauseGame == 1) {
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
}

void GamePlay::addBgWin() {
    CCSize s = CCDirector::sharedDirector()->getWinSize() ;
    bgWin = CCSprite::create("Default.png");
    bgWin->setPosition(ccp(s.width * 2 * (3.0f / 4) + 30, s.height / 2)) ;
    bgWin->setScaleY(1.1);
    bgWin->setScaleX(1.15);
    this->addChild(bgWin,10);
    
    CCLabelTTF *lbPoint = CCLabelTTF::create(GameManager::sharedGameManager()->getName().c_str(),
                                             "Times New Roman", 54);
    lbPoint->setPosition(ccp(bgWin->getContentSize().width / 2,
                             bgWin->getContentSize().height - 100)) ;
    bgWin->addChild(lbPoint) ;
    
    char str_point[20] = {0};
    int p = (_player1Score + 1) * (180 - (minutes * 60 + seconds)) *
       (GameManager::sharedGameManager()->getLevel() * 2000);
    sprintf(str_point, "%i", p);
    lb_point = CCLabelTTF::create(str_point, "Times New Roman", 54);
    lb_point->setPosition(ccp(s.width * 2 * (3.0f/4), s.height - 200)) ;
    this->addChild(lb_point,11) ;
    
    replayWin = CCMenuItemImage::create("btn_Continue.png", "btn_Continue.png",
                                        this, menu_selector(GamePlay::menuReplay));
    replayWin->setPosition(ccp(s.width * 5.0f / 4, s.height / 5)) ;
    replayWin->setScale(3);
    
    menuWin = CCMenuItemImage::create("btn_menu.png", "btn_menu.png", this,
                                      menu_selector(GamePlay::menuMenu));
    menuWin->setPosition(ccp(s.width + s.width * 0.75, s.height / 5)) ;
    menuWin->setScale(3);}

void GamePlay::addBgLose() {
    CCSize s = CCDirector::sharedDirector()->getWinSize() ;
    bgLose = CCSprite::create("Default.png");
    bgLose->setPosition(ccp(s.width * 2 * (3.0f / 4) + 30, s.height / 2)) ;
    bgLose->setScaleY(1.1);
    bgLose->setScaleX(1.15);
    this->addChild(bgLose,10);
    
    replayLose = CCMenuItemImage::create("btn_Continue.png", "btn_Continue.png",
                                         this, menu_selector(GamePlay::menuReplay));
    replayLose->setPosition(ccp(s.width * 5.0f / 4, s.height / 5)) ;
    replayLose->setScale(3);
    
    
    menuLose = CCMenuItemImage::create("btn_menu.png", "btn_menu.png", this,
                                       menu_selector(GamePlay::menuMenu));
    menuLose->setPosition(ccp(s.width + s.width * 3.0f / 4, s.height / 5)) ;
    menuLose->setScale(3);
}
void GamePlay::addBgPause() {
    CCSize s = CCDirector::sharedDirector()->getWinSize() ;
    bgPause = CCSprite::create("Default.png");
    bgPause->setPosition(ccp(s.width * 2 * (3.0f / 4) + 30, s.height / 2)) ;
    bgPause->setScaleY(0.5);
    bgPause->setScaleX(0.5);
    this->addChild(bgPause,10);
    
    replayPause = CCMenuItemImage::create("btn_Continue.png", "btn_Continue.png",
                                         this, menu_selector(GamePlay::menuReplay));
    replayPause->setPosition(ccp(s.width * 5.0f / 4, s.height / 3)) ;
    replayPause->setScale(2);
    
    
    menuMnPause = CCMenuItemImage::create("btn_menu.png", "btn_menu.png", this,
                                       menu_selector(GamePlay::menuMenu));
    menuMnPause->setPosition(ccp(s.width + s.width * 3.0f / 4, s.height / 3)) ;
    menuMnPause->setScale(2);
    
    menuItemImagePause = CCMenuItemImage::create("CloseNormal.png", "CloseNormal.png", this,
                                          menu_selector(GamePlay::menuPause));
    menuItemImagePause->setPosition(ccp(s.width/2, s.height/2)) ;
    menuItemImagePause->setScale(2);
}
void GamePlay::moveBgWin(int i) {
    CCSize s = CCDirector::sharedDirector()->getWinSize() ;
    if (i == 1) {
        CCMoveTo * bgWinMove = CCMoveTo::create(1, ccp(s.width / 2, s.height/2)) ;
        CCMoveTo * lbPointMove = CCMoveTo::create(1, ccp(s.width / 2, s.height - 200)) ;
        CCMoveTo * menuMove = CCMoveTo::create(1, ccp(s.width * 3.0f / 4, s.height / 5)) ;
        CCMoveTo * replayMove = CCMoveTo::create(1, ccp(s.width / 4, s.height / 5)) ;
        
        char str_point[20] = {0};
        int p = (_player1Score + 1) * (180 - (minutes * 60 + seconds)) *
           (GameManager::sharedGameManager()->getLevel() * 2000);
        sprintf(str_point, "%i", p);
        lb_point->setString(str_point);
        
        bgWin->runAction(bgWinMove);
        menuWin->runAction(menuMove);
        replayWin->runAction(replayMove);
        lb_point->runAction(lbPointMove);
    }else {
        CCMoveTo * bgWinMove = CCMoveTo::create(1, ccp(s.width * 2 * (3.0f / 4),
                                                       s.height / 2)) ;
        CCMoveTo * lbPointMove = CCMoveTo::create(1, ccp(s.width * 2 * (3.0f / 4),
                                                         s.height - 200)) ;
        CCMoveTo * menuMove = CCMoveTo::create(1, ccp(s.width + s.width * 3.0f / 4,
                                                      s.height / 5)) ;
        CCMoveTo * replayMove = CCMoveTo::create(1, ccp(s.width * 5 / 4, s.height / 5)) ;
        
        bgWin->runAction(bgWinMove);
        menuWin->runAction(menuMove);
        replayWin->runAction(replayMove);
        lb_point->runAction(lbPointMove);
    }
}

void GamePlay::moveBgLose(int i) {
    CCSize s = CCDirector::sharedDirector()->getWinSize() ;
    if (i == 1) {
        CCMoveTo * bgLoseMove = CCMoveTo::create(1, ccp(s.width /2, s.height/2)) ;
        CCMoveTo * menuMove = CCMoveTo::create(1, ccp(s.width * 3.0f / 4, s.height / 5)) ;
        CCMoveTo * replayMove = CCMoveTo::create(1, ccp(s.width / 4, s.height / 5)) ;
        if (_player2Score > _player1Score) {
           CCLabelTTF *lbLose = CCLabelTTF::create("Your Lose", "Times New Roman", 54);
           lbLose->setPosition(ccp(bgWin->getContentSize().width / 2,
                                   bgWin->getContentSize().height - 300)) ;
            lbLose->setColor(ccc3(100, 255, 255));
           bgLose->addChild(lbLose) ;
        }else if (_player2Score == _player1Score){
            CCLabelTTF *lbLose = CCLabelTTF::create("HOA", "Times New Roman", 54);
            lbLose->setPosition(ccp(bgWin->getContentSize().width / 2,
                                    bgWin->getContentSize().height - 300)) ;
            lbLose->setColor(ccc3(100, 255, 255));
            bgLose->addChild(lbLose) ;
        }
        bgLose->runAction(bgLoseMove);
        menuLose->runAction(menuMove);
        replayLose->runAction(replayMove);
    }else {
        CCMoveTo * bgLoseMove = CCMoveTo::create(1, ccp(s.width * 2 * (3.0f / 4),
                                                        s.height / 2)) ;
        CCMoveTo * menuMove = CCMoveTo::create(1, ccp(s.width + s.width * 3.0f / 4,
                                                      s.height / 5)) ;
        CCMoveTo * replayMove = CCMoveTo::create(1, ccp(s.width * 5.0f / 4,
                                                        s.height / 5)) ;
        bgLose->removeAllChildrenWithCleanup(true);
        bgLose->runAction(bgLoseMove);
        menuLose->runAction(menuMove);
        replayLose->runAction(replayMove);
    }
}
void GamePlay::moveBgPause(int i)
{
    CCSize s = CCDirector::sharedDirector()->getWinSize() ;
    if (i == 1) {
        CCMoveTo * bgLoseMove = CCMoveTo::create(0, ccp(s.width /2, s.height/2)) ;
        CCMoveTo * menuMove = CCMoveTo::create(0, ccp(s.width * 2.0f / 3, s.height / 3)) ;
        CCMoveTo * replayMove = CCMoveTo::create(0, ccp(s.width / 3, s.height / 3)) ;
        bgPause->runAction(bgLoseMove);
        menuMnPause->runAction(menuMove);
        replayPause->runAction(replayMove);
    }else {
        CCMoveTo * bgLoseMove = CCMoveTo::create(0, ccp(s.width * 2 * (3.0f / 4),
                                                        s.height / 2)) ;
        CCMoveTo * menuMove = CCMoveTo::create(0, ccp(s.width + s.width * 3.0f / 4,
                                                      s.height / 3)) ;
        CCMoveTo * replayMove = CCMoveTo::create(0, ccp(s.width * 5.0f / 4,
                                                        s.height / 3)) ;
        bgPause->runAction(bgLoseMove);
        menuMnPause->runAction(menuMove);
        replayPause->runAction(replayMove);
    }
}
void GamePlay::rePlay() {
    playing = true;
    minutes = 2;
    seconds = 60;
    _player1Score = 0;
    _player2Score = 0;
    
    char scoreBuff[10];
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
    
    
    _sc2Tens = _player2Score % 10;
    _sc2SingleDigit = _player2Score / 10;
    sprintf(scoreBuff, "%d.png", _sc2Tens);
    CCSprite *tens2 = CCSprite::createWithSpriteFrameName(scoreBuff);
    _player2ScoreLabel1->setTextureRect(tens2->getTextureRect());
    _player2ScoreLabel1->setTexture(tens2->getTexture());
    sprintf(scoreBuff, "%d.png", _sc2SingleDigit);
    CCSprite *singleDigit2 = CCSprite::createWithSpriteFrameName(scoreBuff);
    _player2ScoreLabel2->setTextureRect(singleDigit2->getTextureRect());
    _player2ScoreLabel2->setTexture(singleDigit2->getTexture());

    scheduleUpdate();
    schedule(schedule_selector(GamePlay::updateTime), 1);
    schedule(schedule_selector(GamePlay::handleProcess), 0.025);
}
void GamePlay::menuMenu(cocos2d::CCObject *psceene) {
    if (win == true) {
        win = false;
    }else if (lose == true) {
        lose = false;
    }
    CCScene *helloScene = Menu::scene();
    CCScene *pScene = CCTransitionFadeTR::create(2, helloScene);
    CCDirector::sharedDirector()->replaceScene(pScene);
}
void GamePlay::menuReplay(cocos2d::CCObject *psceene) {
    if (win == true) {
        this->moveBgWin(0);
        win = false;
        this->gameReset();
        this->rePlay();
    }else if (lose == true) {
        lose = false;
        this->moveBgLose(0);
        this->gameReset();
        this->rePlay();
    }else if(pauseGame == -1) {
        this->moveBgPause(0);
        pauseGame = 1;
    }
}
void GamePlay::menuPause(cocos2d::CCObject *psceene) {
    if (playing == true) {
        this->pauseGame = this->pauseGame * (-1);
        this->moveBgPause(1);
    }
}
#pragma mark GamePlay Scene

CCScene* GamePlay::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // add layer as a child to scene
    CCLayer* layer = new GamePlay();
    scene->addChild(layer);
    layer->release();
    
    return scene;
}
