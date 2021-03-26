#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
//ADD CABS
class Actor : public GraphObject {
public:
	Actor(int h, bool collision, int id, double x, double y, int dir, double size, int layer, StudentWorld* world, bool p = false);
	virtual void doSomething() = 0;

	int getHealth() { return health; };
	void setHealth(int h) {
		if (h <= 100) {
			health = h;
		}
		else {
			health = 100;
		}
		if (health <= 0) {
			alive = false;
		}
	};
	bool getStatus() { return alive; };
	double getHorizSpeed() { return horizSpeed; };
	void setStatus(bool s) { alive = s; };
	void setVertSpeed(double v) { vertSpeed = v; };
	double getVertSpeed() { return vertSpeed; };
	bool isPurifiable() { return purifiable; };
	bool isCollisionWorthy() { return collisionWorthy; };
	bool overlap(Actor* a, Actor* b);
	virtual void damaged(int h) { setHealth(getHealth() - h); };
	virtual ~Actor() {};
private:
	StudentWorld* currentWorld = nullptr;
	bool purifiable = false;
	int health;
	double vertSpeed = 0;
	double horizSpeed = 0;
	bool alive = true;
	bool collisionWorthy;
protected:
	void move();
	void setHorizSpeed(double h) { horizSpeed = h; };
	StudentWorld* getWorld() { return currentWorld; };
	const double radiansPerDegree = 3.1415926535897 / 180;
};

//Peds (humans, zombies, cabs)
class Pedestrian : public Actor {
public:
	Pedestrian(int h, double x, double y, int id, int dir, double size, double mvment, StudentWorld* w);
private:
	void doSomething();
	double movementPlanDistance = 0;
protected:
	void move();
	virtual void damaged(int h) { Actor::damaged(h); };
	double getMovementPlan() { return movementPlanDistance; };
	void setMovementPlan(double d) { movementPlanDistance = d; };
};

class HumanPed : public Pedestrian {
public:
	HumanPed(double x, double y, StudentWorld* w);
private:
	void doSomething();
	void damaged(int h);
};

class ZombiePed : public Pedestrian {
public:
	ZombiePed(double x, double y, StudentWorld* w);
private:
	void doSomething();
	void damaged(int h);
	int gruntTicks = 0;
};

class ZombieCab : public Pedestrian {
public:
	ZombieCab(double x, double y, StudentWorld* w);
private:
	void doSomething();
	void damaged(int h);
	bool damage = false;
};
//EffectItems (holy water refills, healing items, lost souls, oil slicks)
class EffectItems : public Actor {
public:
	EffectItems(double x, double y, int id, int dir, double size, StudentWorld* w, bool p = false);
protected:
	void doSomething();
private:
	virtual void applyEffect() = 0;
};

class HolyWaterRefill : public EffectItems {
public:
	HolyWaterRefill(double x, double y, StudentWorld* w);
private:
	void damaged(int h);
	void applyEffect();
};
class LostSoul : public EffectItems {
public:
	LostSoul(double x, double y, StudentWorld* w);
private:
	void doSomething();
	void applyEffect();
};
class HealthPack : public EffectItems {
public:
	HealthPack(double x, double y, StudentWorld* w);
private:
	void damaged(int h);
	void applyEffect();
};
class OilSlick : public EffectItems {
public:
	OilSlick(double x, double y, StudentWorld* w);
private:
	void applyEffect();
};

//Projectiles
class Projectile : public Actor {
public:
	Projectile(double x, double y, int id, int dir, StudentWorld* world);
private:
	double getDistanceLeft() { return distanceLeft; };
	void doSomething();
	int distanceLeft = 160;
};

//GhostRacer (self explanatory)
class GhostRacer : public Actor {
public:
	GhostRacer(StudentWorld* w);
	void doSomething();
	void increaseHolyWater(int x) { holyWater += x; };
	int getHolyWater() { return holyWater; };
	int getSoulsFreed() { return soulsFreed; };
	void increaseSoulsFreed(int s) { soulsFreed += s; };
	void spinAround();
private:
	int soulsFreed = 0;
	int holyWater = 10;
};

//BorderLine (self explanatory)
class BorderLine : public Actor {
public:
	BorderLine(double x, double y, int id, StudentWorld* world);
private:
	void doSomething();

};


#endif // ACTOR_H_

