#ifndef _GameManager_H_
#define _GameManager_H_

#include "cocos2d.h"

using namespace std;

class GameManager : public cocos2d::CCObject
{
private:
    bool bgm;
    string name;
    int level;
    //Constructor
    GameManager();

    //Instance of the singleton
    static GameManager* m_mySingleton;

public:
    //Get instance of singleton
    static GameManager* sharedGameManager();
    bool getBgm();
    void setBgm(bool bgm);
    string getName();
    void setName(string name);
    int getLevel();
    void setLevel(int level);
};

#endif