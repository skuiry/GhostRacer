#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(int h, bool collision, int id, double x, double y, int dir, double size, int layer, StudentWorld* world, bool p) :GraphObject(id, x, y, dir, size, layer) {
	health = h;
	alive = true;
	collisionWorthy = collision;
	currentWorld = world;
	purifiable = p;
}

bool Actor::overlap(Actor* a, Actor* b) {
	//uses given overlap algorithm
	double delta_x = abs(a->getX() - b->getX());
	double delta_y = abs(a->getY() - b->getY());
	double radiusSum = a->getRadius() + b->getRadius();
	if (delta_x < radiusSum * 0.25 && delta_y < radiusSum * 0.6) {
		return true;
	}
	return false;
}

void Actor::move() {
	//uses movement algorithm
	double vert = getVertSpeed() - getWorld()->getPlayer()->getVertSpeed();
	double horiz = getHorizSpeed();
	double new_y = getY() + vert;
	double new_x = getX() + horiz;
	moveTo(new_x, new_y);
	if (new_x < 0 || new_y < 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT) {
		setStatus(false);
		return;
	}
}

GhostRacer::GhostRacer(StudentWorld* world) :Actor(100, true, IID_GHOST_RACER, 128, 32, up, 4.0, 0, world) {
}

void GhostRacer::doSomething() {
	int key;
	if (getHealth() <= 0)
		return;
	//gone off the left edge of the road
	if (getX() <= (ROAD_CENTER - ROAD_WIDTH / 2)) {
		if (getDirection() > 90) {
			setHealth(getHealth() - 10);
		}
		if (getHealth() <= 0) {
			setStatus(false);
			getWorld()->playSound(SOUND_PLAYER_DIE);
		}
		else {
			getWorld()->playSound(SOUND_VEHICLE_HURT);
		}
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	//gone off the right edge of the road
	else if (getX() >= (ROAD_CENTER + ROAD_WIDTH / 2)) {
		if (getDirection() < 90) {
			setHealth(getHealth() - 10);
		}
		if (getHealth() <= 0) {
			setStatus(false);
			getWorld()->playSound(SOUND_PLAYER_DIE);
		}
		else {
			getWorld()->playSound(SOUND_VEHICLE_HURT);
		}
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	//otherwise, read in input
	else if (getWorld()->getKey(key)) {
		if (key == KEY_PRESS_SPACE && holyWater > 0) {
			double delx = SPRITE_HEIGHT * cos((getDirection()) * radiansPerDegree) + getHorizSpeed();
			double dely = SPRITE_HEIGHT * sin((getDirection()) * radiansPerDegree) + getVertSpeed();
			getWorld()->addSpray(getX() + delx, SPRITE_HEIGHT + getY() + dely, getDirection());
			getWorld()->playSound(SOUND_PLAYER_SPRAY);
			holyWater--;
		}
		else if (key == KEY_PRESS_LEFT && getDirection() < 114) {
			setDirection(getDirection() + 8);
		}
		else if (key == KEY_PRESS_RIGHT && getDirection() > 66) {
			setDirection(getDirection() - 8);
		}
		else if (key == KEY_PRESS_UP && getVertSpeed() < 5) {
			setVertSpeed(getVertSpeed() + 1);
		}
		else if (key == KEY_PRESS_DOWN && getVertSpeed() > -1) {
			setVertSpeed(getVertSpeed() - 1);
		}
	}
	//move according to the given algorithm
	double maxShift = 4.0;
	int dir = getDirection();
	double delx = cos(dir * radiansPerDegree) * maxShift;
	moveTo(getX() + delx, getY());
}

void GhostRacer::spinAround() {
	int spin = randInt(5, 20);
	int rotation = randInt(0, 1);
	switch (rotation) {
	case 0: //counterclockwise if it can
		if (getDirection() + spin <= 120) {
			setDirection(getDirection() + spin);
		}
		else {
			setDirection(getDirection() - spin);
		}
		break;
	case 1: //clockwise if it can
		if (getDirection() - spin >= 60) {
			setDirection(getDirection() - spin);
		}
		else {
			setDirection(getDirection() + spin);
		}
		break;
	}
}

Pedestrian::Pedestrian(int h, double x, double y, int id, int dir, double size, double mvment, StudentWorld* w) : Actor(h, true, id, x, y, dir, size, 0, w, true) {
	movementPlanDistance = mvment;
	setVertSpeed(-4);
}

void Pedestrian::doSomething() {
}

void Pedestrian::move() {
	//follows movement plan algorithm
	do {
		setHorizSpeed(randInt(-3, 3));
	} while (getHorizSpeed() == 0);
	movementPlanDistance = randInt(4, 32);
	if (getHorizSpeed() < 0) {
		setDirection(180);
	}
	else {
		setDirection(0);
	}
}

ZombiePed::ZombiePed(double x, double y, StudentWorld* w) : Pedestrian(2, x, y, IID_ZOMBIE_PED, 0, 3.0, 0, w) {}

void ZombiePed::doSomething() {
	if (!getStatus())
		return;
	//if overlap, damage GhostRacer and itself
	if (overlap(this, getWorld()->getPlayer())) {
		getWorld()->getPlayer()->setHealth(getWorld()->getPlayer()->getHealth() - 5);
		damaged(2);
		return;
	}
	//Check x difference
	double xdiff = getX() - getWorld()->getPlayer()->getX();
	if (abs(xdiff) <= 30 && getY() > getWorld()->getPlayer()->getY()) {
		setDirection(270);
		//check which way x and y go
		if (xdiff > 0)
			setHorizSpeed(-1);
		else if (xdiff < 0)
			setHorizSpeed(1);
		else
			setHorizSpeed(0);
		gruntTicks--;
		if (gruntTicks <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			gruntTicks = 20;
		}
	}
	//Move
	Actor::move();
	if (getMovementPlan() > 0) {
		setMovementPlan(getMovementPlan() - 1);
		return;
	}
	Pedestrian::move();
}

void ZombiePed::damaged(int h) {
	Pedestrian::damaged(h);
	if (!getStatus()) {
		getWorld()->playSound(SOUND_PED_DIE);
		//potentially drop a healing goodie
		if (!overlap(this, getWorld()->getPlayer())) {
			if (randInt(1, 5) == 1) {
				//add new healing goodie
				getWorld()->enemySpawnedGoodie(1, this);
			}
		}
		//increase score by 150
		getWorld()->increaseScore(150);
	}
	else {
		getWorld()->playSound(SOUND_PED_HURT);
	}
}

HumanPed::HumanPed(double x, double y, StudentWorld* w) : Pedestrian(2, x, y, IID_HUMAN_PED, 0, 2.0, 0, w) {}

void HumanPed::doSomething() {
	if (!getStatus())
		return;
	//if overlaps with GhostRacer, end the level by telling the GhostRacer it died
	if (overlap(this, getWorld()->getPlayer())) {
		getWorld()->getPlayer()->setStatus(false);
		return;
	}
	//otherwise, move
	Actor::move();
	setMovementPlan(getMovementPlan() - 1);
	if (getMovementPlan() > 0) {
		return;
	}
	Pedestrian::move();
}

void HumanPed::damaged(int h) {
	//switch direction and play hurt sound
	setHorizSpeed(-1 * getHorizSpeed());
	setDirection(abs(getDirection() - 180));
	getWorld()->playSound(SOUND_PED_HURT);
}

ZombieCab::ZombieCab(double x, double y, StudentWorld* w) :Pedestrian(3, x, y, IID_ZOMBIE_CAB, 90, 4, 0, w) {}

void ZombieCab::doSomething() {
	if (!getStatus())
		return;
	//if overlaps and hasn't damaged GhostRacer, does the specified commands
	if (overlap(this, getWorld()->getPlayer())) {
		if (!damage) {
			getWorld()->playSound(SOUND_VEHICLE_CRASH);
			getWorld()->getPlayer()->setHealth(getWorld()->getPlayer()->getHealth() - 20);
			if (getX() <= getWorld()->getPlayer()->getX()) {
				setHorizSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else {
				setHorizSpeed(5);
				setDirection(60 - randInt(0, 19));
			}
			damage = true;
		}
	}
	Actor::move();
	if (!getStatus()) {
		return;
	}
	//checks for collision worthy actors in front of it
	Actor* obs = getWorld()->checkCabLane(getX(), getY(), 0);
	if (getVertSpeed() > getWorld()->getPlayer()->getVertSpeed() && obs != nullptr) {
		if (obs->getY() > getY()) {
			if (abs(obs->getY() - getY()) < 96) {
				setVertSpeed(getVertSpeed() - 0.5);
				return;
			}
		}
	}
	//checks for collision worthy actors behind it
	obs = getWorld()->checkCabLane(getX(), getY(), 1);
	if (getVertSpeed() <= getWorld()->getPlayer()->getVertSpeed() && obs != nullptr) {
		if (obs->getY() < getY() && obs != getWorld()->getPlayer()) {
			if (abs(obs->getY() - getY()) < 96) {
				setVertSpeed(getVertSpeed() + 0.5);
				return;
			}
		}
	}
	setMovementPlan(getMovementPlan() - 1);
	if (getMovementPlan() > 0) {
		return;
	}
	else {
		setMovementPlan(randInt(4, 32));
		setVertSpeed(getVertSpeed() + randInt(-2, 2));
	}
}

void ZombieCab::damaged(int h) {
	Pedestrian::damaged(h);
	if (!getStatus()) {
		getWorld()->playSound(SOUND_VEHICLE_DIE);
		if (!overlap(this, getWorld()->getPlayer())) {
			if (randInt(1, 5) == 1) {
				//add new oil slick
				getWorld()->enemySpawnedGoodie(2, this);
			}
		}
		getWorld()->increaseScore(200);
		return;
	}
	else {
		getWorld()->playSound(SOUND_VEHICLE_HURT);
	}
}

EffectItems::EffectItems(double x, double y, int id, int dir, double size, StudentWorld* w, bool p) :Actor(0, false, id, x, y, dir, size, 2, w, p) {
	setVertSpeed(-4);
}

void EffectItems::doSomething() {
	if (overlap(this, getWorld()->getPlayer())) {
		applyEffect();
	}
	if (getStatus()) {
		Actor::move();
	}
}

HolyWaterRefill::HolyWaterRefill(double x, double y, StudentWorld* w) : EffectItems(x, y, IID_HOLY_WATER_GOODIE, 90, 2.0, w, true) {}

void HolyWaterRefill::applyEffect() {
	//adds 10 charges of holy water to GhostRacer
	getWorld()->getPlayer()->increaseHolyWater(10);
	setStatus(false);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(50);
}

void HolyWaterRefill::damaged(int h) {
	setStatus(false);
}

LostSoul::LostSoul(double x, double y, StudentWorld* w) : EffectItems(x, y, IID_SOUL_GOODIE, 0, 4.0, w) {}

void LostSoul::doSomething() {
	EffectItems::doSomething();
	//spins 10 degrees clockwise every tick
	setDirection(getDirection() - 10);
}

void LostSoul::applyEffect() {
	//increases the souls freed by 1
	getWorld()->getPlayer()->increaseSoulsFreed(1);
	setStatus(false);
	getWorld()->playSound(SOUND_GOT_SOUL);
	getWorld()->increaseScore(100);
}

HealthPack::HealthPack(double x, double y, StudentWorld* w) : EffectItems(x, y, IID_HEAL_GOODIE, 0, 1, w, true) {}

void HealthPack::applyEffect() {
	//heals player for 10 (up to 100 hit points)
	getWorld()->getPlayer()->setHealth(getWorld()->getPlayer()->getHealth() + 10);
	setStatus(false);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(250);
}

void HealthPack::damaged(int h) {
	setStatus(false);
}

OilSlick::OilSlick(double x, double y, StudentWorld* w) : EffectItems(x, y, IID_OIL_SLICK, 0, randInt(2, 5), w) {}

void OilSlick::applyEffect() {
	//spins player around
	getWorld()->playSound(SOUND_OIL_SLICK);
	getWorld()->getPlayer()->spinAround();
}

BorderLine::BorderLine(double x, double y, int id, StudentWorld* world) : Actor(0, false, id, x, y, 0, 2.0, 2, world) {
	setVertSpeed(-4);
}

void BorderLine::doSomething() {
	Actor::move();
}

Projectile::Projectile(double x, double y, int id, int dir, StudentWorld* w) : Actor(0, false, id, x, y, dir, 1.0, 1, w) {

}

void Projectile::Projectile::doSomething() {
	if (!getStatus()) {
		return;
	}
	//checks if it hit anyone
	Actor* hit = getWorld()->checkToPurify(this);
	if (hit != nullptr) {
		//damages the actor it hit if it can
		hit->damaged(1);
		setStatus(false);
		return;
	}
	//moves forward the maximum number of pixels it can
	else if (distanceLeft - SPRITE_HEIGHT > 0) {
		moveForward(SPRITE_HEIGHT);
		distanceLeft -= SPRITE_HEIGHT;
	}
	else if (distanceLeft - SPRITE_HEIGHT <= 0) {
		moveForward(distanceLeft);
		distanceLeft = 0;
		setStatus(false);
	}
	//if gone off the screen, it's dead
	if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT) {
		setStatus(false);
		return;
	}
}
