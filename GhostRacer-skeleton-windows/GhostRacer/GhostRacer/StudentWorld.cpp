#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>


GameWorld* createStudentWorld(std::string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(std::string assetPath)
    : GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    //Add GhostRacer
    player = new GhostRacer(this);
    //Add BorderLines (both white and yellow ones)
    for (int N = 0; N < VIEW_HEIGHT / SPRITE_HEIGHT; N++) {
        BorderLine* l = new BorderLine(ROAD_CENTER - ROAD_WIDTH / 2, N * SPRITE_HEIGHT, IID_YELLOW_BORDER_LINE, this);
        BorderLine* r = new BorderLine(ROAD_CENTER + ROAD_WIDTH / 2, N * SPRITE_HEIGHT, IID_YELLOW_BORDER_LINE, this);
        actors.push_back(l);
        actors.push_back(r);
    }
    for (int M = 0; M < VIEW_HEIGHT / (4 * SPRITE_HEIGHT); M++) {
        BorderLine* wl = new BorderLine(ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, M * (4 * SPRITE_HEIGHT), IID_WHITE_BORDER_LINE, this);
        BorderLine* wr = new BorderLine(ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, M * (4 * SPRITE_HEIGHT), IID_WHITE_BORDER_LINE, this);
        actors.push_back(wl);
        actors.push_back(wr);
        topLine = M * 4 * SPRITE_HEIGHT;
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (bonus > 0) {
        bonus--;
    }
    //This for loop is for letting the actors that are "alive" do something.
    for (std::vector<Actor*>::iterator it = actors.begin(); it < actors.end();) {
        if ((*it)->getStatus()) {
            (*it)->doSomething();
            /*if (!(*it)->getStatus()) {
                dead.push_back(*it);
                it = actors.erase(it);
            }*/
            if (!player->getStatus()) {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (player->getSoulsFreed() == getLevel() * 2 + 5) {
                increaseScore(bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
            else {
                it++;
            }
        }
        else {
            /*dead.push_back(*it);
            it = actors.erase(it);*/
            it++;
        }
    }
    //This is for the player to do something.
    if (player->getStatus()) {
        player->doSomething();
        //In case the player dies trying to do something
        if (!player->getStatus()) {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }
    }
    //If the player isn't alive, return the respective status.
    else {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    //Get rid of dead actors
    for (std::vector<Actor*>::iterator it = actors.begin(); it != actors.end();) {
        if (!(*it)->getStatus()) {
            delete (*it);
            it = actors.erase(it);
        }
        else {
            it++;
        }
    }
    //Add new actors NEED TO IMPLEMENT
    spawnActors();
    //Add new BorderLines to the top of the screen
    topLine = topLine - 4 - player->getVertSpeed();
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - topLine;
    if (delta_y >= SPRITE_HEIGHT) {
        BorderLine* l = new BorderLine(ROAD_CENTER - ROAD_WIDTH / 2, new_border_y, IID_YELLOW_BORDER_LINE, this);
        BorderLine* r = new BorderLine(ROAD_CENTER + ROAD_WIDTH / 2, new_border_y, IID_YELLOW_BORDER_LINE, this);
        actors.push_back(l);
        actors.push_back(r);
    }
    if (delta_y >= 4 * SPRITE_HEIGHT) {
        BorderLine* wl = new BorderLine(ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3, new_border_y, IID_WHITE_BORDER_LINE, this);
        BorderLine* wr = new BorderLine(ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3, new_border_y, IID_WHITE_BORDER_LINE, this);
        actors.push_back(wl);
        actors.push_back(wr);
        topLine = new_border_y;
    }
    //Continue the game if the player hasn't died yet.
    setGameStatText(updateHUD());
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::enemySpawnedGoodie(int type, Actor* a) {
    //For goodies that can be spawned through killing an enemy
    HealthPack* s;
    OilSlick* o;
    switch (type) {
    case 1: //Healing Goodie
        s = new HealthPack(a->getX(), a->getY(), this);
        actors.push_back(s);
        break;
    case 2: //Oil Slick
        o = new OilSlick(a->getX(), a->getY(), this);
        actors.push_back(o);
        break;
    }

}

std::string StudentWorld::updateHUD() {
    std::ostringstream stream;
    int score = getScore();
    stream << "Score: " << score << "  ";
    int level = getLevel();
    stream << "Lvl: " << level << "  ";
    int soulsLeft = getLevel() * 2 + 5 - getPlayer()->getSoulsFreed();
    stream << "Souls2Save: " << soulsLeft << "  ";
    int lives = getLives();
    stream << "Lives: " << lives << "  ";
    int health = getPlayer()->getHealth();
    stream << "Health: " << health << "  ";
    int sprays = getPlayer()->getHolyWater();
    stream << "Sprays: " << sprays << "  ";
    stream << "Bonus: " << bonus;
    return stream.str();
}

void StudentWorld::spawnActors() {
    //Zombie Cabs

    int chanceVehicle = std::max(100 - getLevel() * 10, 20);
    int x = randInt(0, chanceVehicle - 1);
    if (x == 0) {
        //add new zombie cab
        Actor* closestToBottom = nullptr;
        Actor* closestToTop = nullptr;
        bool foundLane = false;
        double startx = 0;
        double starty = 0;
        double startvel = 0;
        int cur_lane = randInt(1, 3);
        for (int i = cur_lane; i < cur_lane + 3; i++) {
            //lane 1
            if (i % 3 == 1 && !foundLane) {
                //checks closest actor from the bottom up
                closestToBottom = checkCabLane(ROAD_CENTER - ROAD_WIDTH / 3, 0, 0);
                if (closestToBottom == nullptr || closestToBottom->getY() > (VIEW_HEIGHT / 3)) {
                    startx = ROAD_CENTER - ROAD_WIDTH / 3;
                    starty = SPRITE_HEIGHT / 2;
                    startvel = getPlayer()->getVertSpeed() + randInt(2, 4);
                    foundLane = true;
                    break;
                }
                //checks closest actor from top up
                closestToTop = checkCabLane(ROAD_CENTER - ROAD_WIDTH / 3, VIEW_HEIGHT, 1);
                if (closestToTop == nullptr || closestToTop->getY() < (2 * VIEW_HEIGHT / 3)) {
                    startx = ROAD_CENTER - ROAD_WIDTH / 3;
                    starty = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    startvel = getPlayer()->getVertSpeed() - randInt(2, 4);
                    foundLane = true;
                    break;
                }
            }
            //lane 2
            else if (i % 3 == 2 && !foundLane) {
                //checks closest actor from the bottom up
                closestToBottom = checkCabLane(ROAD_CENTER, 0, 0);
                if (closestToBottom == nullptr || closestToBottom->getY() > (VIEW_HEIGHT / 3)) {
                    startx = ROAD_CENTER;
                    starty = SPRITE_HEIGHT / 2;
                    startvel = getPlayer()->getVertSpeed() + randInt(2, 4);
                    foundLane = true;
                    break;
                }
                //checks closest actor from top up
                closestToTop = checkCabLane(ROAD_CENTER, VIEW_HEIGHT, 1);
                if (closestToTop == nullptr || closestToTop->getY() < (2 * VIEW_HEIGHT / 3)) {
                    startx = ROAD_CENTER;
                    starty = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    startvel = getPlayer()->getVertSpeed() - randInt(2, 4);
                    foundLane = true;
                    break;
                }
            }
            //lane 3
            else if (i % 3 == 0 && !foundLane) {
                //checks closest actor from the bottom up
                closestToBottom = checkCabLane(ROAD_CENTER + ROAD_WIDTH / 3, 0, 0);
                if (closestToBottom == nullptr || closestToBottom->getY() > (VIEW_HEIGHT / 3)) {
                    startx = ROAD_CENTER + ROAD_WIDTH / 3;
                    starty = SPRITE_HEIGHT / 2;
                    startvel = getPlayer()->getVertSpeed() + randInt(2, 4);
                    foundLane = true;
                    break;
                }
                //checks closest actor from top up
                closestToTop = checkCabLane(ROAD_CENTER + ROAD_WIDTH / 3, VIEW_HEIGHT, 1);
                if (closestToTop == nullptr || closestToTop->getY() < (2 * VIEW_HEIGHT / 3)) {
                    startx = ROAD_CENTER + ROAD_WIDTH / 3;
                    starty = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                    startvel = getPlayer()->getVertSpeed() - randInt(2, 4);
                    foundLane = true;
                    break;
                }
            }
        }
        //if a lane was found, place a zombie cab there with the appropriate vertical speed
        if (foundLane) {
            ZombieCab* s = new ZombieCab(startx, starty, this);
            s->setVertSpeed(startvel);
            actors.push_back(s);
        }
    }
    //Oil Slicks
    int chanceOilSlick = std::max(150 - getLevel() * 10, 40);
    x = randInt(0, chanceOilSlick - 1);
    if (x == 0) {
        //add oil slick
        OilSlick* s = new OilSlick(randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT, this);
        actors.push_back(s);
    }

    //Zombie Peds
    int chanceZombiePed = std::max(100 - getLevel() * 10, 20);
    x = randInt(0, chanceZombiePed - 1);
    if (x == 0) {
        //add zombie ped
        ZombiePed* s = new ZombiePed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
        actors.push_back(s);
    }

    //Human Peds
    int chanceHumanPed = std::max(200 - getLevel() * 10, 30);
    x = randInt(0, chanceHumanPed - 1);
    if (x == 0) {
        //add human ped
        HumanPed* s = new HumanPed(randInt(0, VIEW_WIDTH), VIEW_HEIGHT, this);
        actors.push_back(s);
    }

    //Holy Water Refills
    int chanceHolyWater = 100 + 10 * getLevel();
    x = randInt(0, chanceHolyWater - 1);
    if (x == 0) {
        //add holy water refill
        HolyWaterRefill* s = new HolyWaterRefill(randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT, this);
        actors.push_back(s);
    }

    //Lost Souls
    int chanceSoul = 100;
    x = randInt(0, chanceSoul);
    if (x == 0) {
        //add lost soul
        LostSoul* s = new LostSoul(randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT, this);
        actors.push_back(s);
    }
}

int StudentWorld::getLane(double x) {
    int lane = 0;
    if (x >= ROAD_CENTER - ROAD_WIDTH / 2 && x < ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3) {
        lane = 1; //left lane
    }
    else if (x >= ROAD_CENTER - ROAD_CENTER - ROAD_WIDTH / 2 + ROAD_WIDTH / 3 && x < ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3) {
        lane = 2; //middle lane
    }
    else if (x >= ROAD_CENTER + ROAD_WIDTH / 2 - ROAD_WIDTH / 3 && x < ROAD_CENTER + ROAD_WIDTH / 2) {
        lane = 3; //right lane
    }
    return lane;
}

Actor* StudentWorld::checkCabLane(double x, double y, int dir) {
    Actor* closestObstacle = nullptr;
    double smallestDistance = 100000;
    //find the lane of the given location
    int cabLane = getLane(x);
    for (std::vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        //check if any collision worthy actor is present in the location
        if (cabLane == getLane((*it)->getX()) && (*it)->isCollisionWorthy()) {
            //checks if the actor is behind or in front based on what the given actor needs
            if (dir == 0) {
                if ((*it)->getY() > y && (*it)->getY() - y < smallestDistance) {
                    smallestDistance = (*it)->getY() - y;
                    closestObstacle = *it;
                }
            }
            else if (dir == 1) {
                if ((*it)->getY() < y && y - (*it)->getY() < smallestDistance) {
                    smallestDistance = y - (*it)->getY();
                    closestObstacle = *it;
                }
            }
        }
    }
    //checks to see if the player is in the lane (behind or front)
    if (cabLane == getLane(player->getX())) {
        if (dir == 0) {
            if ((player)->getY() > y && (player)->getY() - y < smallestDistance) {
                smallestDistance = (player)->getY() - y;
                closestObstacle = player;
            }
        }
        else if (dir == 1) {
            if ((player)->getY() < y && y - (player)->getY() < smallestDistance) {
                smallestDistance = y - (player)->getY();
                closestObstacle = player;
            }
        }
    }
    return closestObstacle;
}

Actor* StudentWorld::checkToPurify(Actor* s) {
    //checks for purifiable actors and returns it, if none are purifiable, returns nullptr
    for (std::vector<Actor*>::iterator it = actors.begin(); it != actors.end(); it++) {
        if (s->overlap(s, *it) && (*it)->isPurifiable()) {
            return *it;
        }
    }
    return nullptr;
}

void StudentWorld::cleanUp()
{
    //Delete the leftover actors
    if (actors.size() != 0) {
        for (std::vector<Actor*>::iterator it = actors.begin(); it != actors.end();) {
            delete (*it);
            it = actors.erase(it);
        }
    }
    //Delete the GhostRacer
    delete player;
    player = nullptr;
}

void StudentWorld::addSpray(double x, double y, int dir) {
    //adds a new spray
    Projectile* holy = new Projectile(x, y, IID_HOLY_WATER_PROJECTILE, dir, this);
    actors.push_back(holy);
}
StudentWorld::~StudentWorld() {
    cleanUp();
}
