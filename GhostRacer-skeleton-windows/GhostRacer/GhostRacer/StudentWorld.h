#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <vector>
#include <iostream>
#include "GameWorld.h"
#include <sstream>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class GhostRacer;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init(); //initializes game level
    virtual int move(); //moves actors
    virtual void cleanUp(); //cleans up after level
    virtual GhostRacer* getPlayer() { return player; }; //return's GhostRacer
    void addSpray(double x, double y, int dir); //adds a holy water spray to the screen
    Actor* checkToPurify(Actor* s); //checks for potential purification actors
    void enemySpawnedGoodie(int type, Actor* a = nullptr); //spawns a goodie based on enemy death
    Actor* checkCabLane(double x, double y, int dir); //checks for collision worthy actors in lane of given location
    ~StudentWorld();
private:
    void spawnActors(); //potentially spawns actors every tick
    std::string updateHUD(); //updates the score info at the top of the screen
    int getBonus() { return bonus; }; //returns the current bonus amount
    int getLane(double x); //returns the lane based on the given x coordinate
    std::vector<Actor*> actors; //vector of actors
    int bonus = 5000;
    double topLine; //y-coordinate of the highest white BorderLine
    GhostRacer* player = nullptr;
};

#endif // STUDENTWORLD_H_
