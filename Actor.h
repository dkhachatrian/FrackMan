#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

#include <vector>

//#include "GameWorld.h" //need to access getKey() in FrackMan class

class StudentWorld;
class Protester;
class HardcoreProtester;
class Goodie;
class Gold;
class FrackMan;
class Squirt;
class Boulder;
class Water;
class Sonar;





typedef int CoordType; //for coordinates of Actors
typedef double LengthType; //for sprite sizes (in X and Y) of Actors

const int X_UPPER_BOUND = VIEW_WIDTH - SPRITE_WIDTH; //cannot reach upper bound. ==60
const int Y_UPPER_BOUND = VIEW_HEIGHT - SPRITE_HEIGHT; //== 60

const int PLAYER_START_HEALTH = 10;
const int PROTESTER_START_HEALTH = 5;
const int HARDCORE_PROTESTER_START_HEALTH = 20;
const int PLAYER_START_SQUIRTS = 5;

//for the moveTo functions
const int UP_DIR = 1;
const int DOWN_DIR = -1;
const int LEFT_DIR = -1;
const int RIGHT_DIR = 1;


const int TOUCHING = 0;
const int OVERLAPPING = -1;
const int EXISTS_GAP = 1;


const int INVALID_IID = -1;

enum Group { player, enemies, boulders, goodies, squirts, gold, bribes, anyone, na };



struct Coord
{
	Coord()
	{
		m_x = -1;
		m_y = -1;
	}

	Coord(CoordType x, CoordType y)
	{
		m_x = x;
		m_y = y;
	}
	CoordType x() const { return m_x; }
	CoordType y() const { return m_y; }

	bool operator==(const Coord other) const { return (x() == other.x() && y() == other.y()); }

	bool operator<(const Coord other) const { return (x() < other.x() && y() < other.y()); }


	CoordType m_x, m_y;
};


const int DAMAGE_BOULDER = -100;
const int DAMAGE_SQUIRT = -2;
const int DAMAGE_YELL = -2;


const double NORMAL_IMAGE_SIZE = 1;
const double DIRT_IMAGE_SIZE = 0.25;

const int PLAYER_START_X = 30;
const int PLAYER_START_Y = 60;


const unsigned int DEPTH_INVALID = -1; //a huge number actually
const unsigned int DEPTH_PLAYER = 0;
const unsigned int DEPTH_PROTESTER = 0;
const unsigned int DEPTH_BOULDER = 1;
const unsigned int DEPTH_SQUIRT = 1;
const unsigned int DEPTH_GOODIE = 2;
const unsigned int DEPTH_DIRT = 3;

//const string DIRT_ID = "dirt";


// ticks values

const int TICK_SQUIRT = 4;




//doSomething() return values
const int DEAD = -1;
const int MOVED = 0;
const int INTERACTED = 1;
const int STATIONARY = 2;
const int ANNOYED = 3;
const int DISCOVERED = 4;
const int PLACED = 5;
const int CAN_YELL = 6;



class Actor :public GraphObject
{
public:
	/*
	Actor(int IID, unsigned int depth, StudentWorld* sw, double imageSize):GraphObject(IID, -1, -1, right, imageSize, depth)
	{
		m_sw = sw;
		setVisible(true); //by default
		m_is_solid = false; //everything except Boulders is not solid
		m_width = imageSize * SPRITE_WIDTH;
		m_height = imageSize * SPRITE_HEIGHT;
	}
	*/
	Actor( int IID,  unsigned int depth, StudentWorld* sw, double imageSize, CoordType x = -1, CoordType y = -1):
		GraphObject(IID, x, y, right, imageSize, depth)
	{
		//moveTo(x, y);
		m_depth = depth;
		m_sw = sw;
		m_doITick = false; //most things don't tick
		m_ticksLeft = -1;
		m_width = imageSize * SPRITE_WIDTH;
		m_height = imageSize * SPRITE_HEIGHT;
		setVisibility(true);
		m_isDead = false;
		m_dir = right;
		m_group = na;
		m_id = IID;

		//m_visible = true; //by default
		//setVisible(true); //by default
	}


	virtual ~Actor() { setVisible(false); }


	//allow StudentWorld to set visibility or direction if needed
	inline void setVisibility(bool x)
	{
		setVisible(x);
		setVisibleFlag(x);
	}
	inline void setDir(Direction d)
	{
		setDirection(d);
		m_dir = d;
	}



	Group whatGroupAmI() const { return m_group; }



	virtual int doSomething();
	virtual void attemptToInteractWithActors();

	//StudentWorld tells the Actor what it's responding to
	virtual void respondToPlayer(FrackMan* player, double distanceOfInteraction) {};
	virtual void respondToEnemy(Protester* enemy, double distanceOfInteraction) {};
	virtual void respondToSquirt(Squirt* squirt, double distanceOfInteraction) {};
	virtual void respondToBoulder(Boulder* boulder, double distanceOfInteraction) {};
	virtual void respondToGoodie(Goodie* goodie, double distanceOfInteraction) {};
	virtual void respondToBribe(Gold* bribe, double distanceOfInteraction) {};





	//virtual int doSomething();
	
	//gives bottom-left corner (i.e. what's returned by getX/Y)
	//for location
	void sendLocation(CoordType& x, CoordType& y) const
	{
		x = getX();
		y = getY();
	}
	Coord getCoord() const { return Coord(getX(), getY()); }


	int getIdentity() const{ return m_id; }
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	bool isDead() const { return m_isDead; }
	bool isVisible()  const { return m_visible; }
	int getDepth() const { return m_depth; }



	//let StudentWorld kill the Actors when cleaning up
	void die() { m_isDead = true; }


protected:

	//only Actors need to see these

	void setIdentityAs(int id) {m_id = id;}
	void setGroupAs(Group g) { m_group = g; }

	//ticking
	void countDownATick() { m_ticksLeft--; }
	bool doITick() const { return m_doITick; }
	void setTickStatus(bool x) { m_doITick = x; }
	void setTickNumber(int x) { m_ticksLeft = x; }
	int getTickNumber() const { return m_ticksLeft; }

	virtual void Actor::performTickAction();
	void setVisibleFlag(bool x) { m_visible = x; }


	StudentWorld* getWorld() const { return m_sw; }


	//calls StudentWorld
	bool Actor::isThereDirtNextToMeInDirection(Direction dir) const;
	bool Actor::isThereDirtNextToMeInCurrentDirection() const;




private:
	StudentWorld* m_sw;
	int m_id;
	CoordType m_width;
	CoordType m_height;
	bool m_isDead;
	bool m_visible;
	int m_ticksLeft;
	bool m_doITick;
	int m_depth;
	Direction m_dir;
	Group m_group;
	//GraphObject m_go;

};

//only class with image size different from the rest
class Dirt :public Actor
{
public:
	//default constructor for the purposes of initializing the m_dirts array
	//Dirt() :Actor(-1, -1, IID_DIRT, right, DIRT_IMAGE_SIZE, DEPTH_DIRT, nullptr)
	//{
	//	setVisible(true);
	//};
	//Actor(CoordType x, CoordType y, int IID, Direction dir, unsigned int depth, StudentWorld* sw, double imageSize, bool solidity = false) :

	Dirt(StudentWorld* sw, CoordType x, CoordType y) : Actor(IID_DIRT, DEPTH_DIRT, sw, DIRT_IMAGE_SIZE, x, y)
	{
		setDir(right);
		setVisibility(true);
		setIdentityAs(IID_DIRT);
	};

	virtual ~Dirt()
	{
		//setVisible(false);
	};

	virtual int doSomething()
	{
		return 0; //do nothing (how boring!)
	};


private:
};

//all DynamicObjects are of the size USUAL_IMAGE_SIZE
class DynamicObject :public Actor
{
public:
	DynamicObject(int IID, unsigned int depth, StudentWorld* sw, CoordType x = -1, CoordType y = -1, double imageSize = NORMAL_IMAGE_SIZE):
		Actor(IID, depth, sw, imageSize, x, y)
	{

		m_hp = 0;
	};


	int getHealth() const { return m_hp; }

	virtual ~DynamicObject()
	{};

	virtual bool attemptMove(const Direction dir);
	virtual bool doSpecializedAction() { return false; }
	
	//bool setDir(Direction dir) {}; //covered by GraphObject::setDirection(Direction d);


	//changes x_s,y_s (copies of the coordinates of Actor who called it)
	// to where the Actor wants to go.
	// Returns true if no boundary or solid object (i.e. Boulder) is in the way, false otherwise.
	//virtual bool canMoveInDirection(const Direction dir, int& x, int& y);


	// Does not change anything
	virtual bool moveMatchesDir(const Direction moveDir) const { return (GraphObject::getDirection() == moveDir); }

protected:

	void setHealth(int hp) { m_hp = hp; }


	void changeHealthBy(int x) { m_hp += x; }

	bool didIDie() const { return m_hp <= 0; }


	virtual void getHurt(int damage) {}; //not all DynamicObjects get hurt, but both FrackMan and the Protesters do


private:

	int m_hp;

};

//whether the movable object is annoyed
//virtual bool isAnnoyed() = 0;
//virtual int attemptAnnoyedAction() = 0;


class Squirt :public DynamicObject
{
public:
	Squirt(CoordType x, CoordType y, StudentWorld* sw, Direction dir);

	virtual ~Squirt() {};

	virtual int doSomething();

	virtual void respondToEnemy(Protester* enemy, double distanceOfInteraction);
	virtual void respondToBoulder(Boulder* boulder, double distanceOfInteraction);


};







class FrackMan :public DynamicObject
{
public:
	FrackMan(StudentWorld* sw, CoordType x = PLAYER_START_X, CoordType y = PLAYER_START_Y):DynamicObject(IID_PLAYER, DEPTH_PLAYER, sw, x, y)
	{
		changeHealthBy(PLAYER_START_HEALTH);
		m_squirts = PLAYER_START_SQUIRTS;
		m_gold = 0;
		m_sonar = 1;
		setIdentityAs(IID_PLAYER);
		setDir(right);
		setGroupAs(player);
	};

	virtual int doSomething();

	virtual bool attemptMove(const Direction dir);

	//for the purposes of compiling...
	//virtual bool isAnnoyed() { return false; }
	//virtual int attemptAnnoyedAction() { return 0; }
	//virtual bool shouldIBeAnnoyed() const { return false; }

	virtual void respondToEnemy(Protester* enemy, double distanceOfInteraction);


	int getSquirts() const { return m_squirts; }
	void changeSquirtsBy(int x) { m_squirts += x; }
	bool attemptToUseWaterGun();

	int getGold() const { return m_gold; }
	void changeGoldBy(int x) { m_gold += x; }
	bool attemptToDropGold();

	int getSonar() const { return m_sonar; }
	void changeSonarBy(int x) { m_sonar += x; }
	bool attemptToUseSonar();

	virtual void getHurt(int damage);


	bool attemptToDig();
	//bool removeDirt(const Direction dir, const int& x, const int& y);

protected:
	//virtual bool setDir(int dir);
private:
	int m_squirts;
	int m_gold;
	int m_sonar;
};





enum protesterState { leaving, resting, coolingDown, OK };

const int REST_TICK_NORMAL = 4;
const int REST_TICK_YELL = 15;

class Protester :public DynamicObject
{
public:
	Protester(CoordType x, CoordType y, StudentWorld* sw, int IID = IID_PROTESTER) :DynamicObject(IID, DEPTH_PROTESTER, sw, x, y)
	{
		setVisibility(true);
		setDir(left);
		setHealth(PROTESTER_START_HEALTH);
		setGroupAs(enemies);
		setProtesterState(OK);
		//m_restTicks = REST_TICK_NORMAL;
		setRestTick();
		m_coolDownPeriod = REST_TICK_YELL;
		m_turnPeriod = 200;
		rollNumberOfTimesToMoveInCurrentDirection();
		m_currentRestTick = -1; //immediately counts up to 0 on its first doSomething() call
		m_currentNonrestTick = 0;
		m_currentNonTurnedTick = 0;
		m_currentCoolDownTick = 0;
		m_annoyedRestTick = 0;
		m_annoyed = false;
		m_resting = false;
	}
	virtual ~Protester() {};

	virtual int doSomething();
	virtual Direction tryToGetToFrackMan() const;

	void setResting(bool x) { m_resting = x; }


	virtual GraphObject::Direction Protester::chooseDirection();

	//void Protester::setAnnoyedRestTickMax(int x) = {m_annoyed}

	virtual void setRestTick();

	//virtual void respondToActor();
	int getDirTimes() const { return m_numTimesCurrentDir; }

	//void resetTick(int& t) { t = 0; }
	
	void setAnnoyedTick(int t) { m_annoyedRestTick = t; }
	int getCurrentAnnoyedTick() const { return m_annoyedRestTick; }
	void countUpAnAnnoyedTick() { m_annoyedRestTick++; }
	void setMaxAnnoyedTickAs(int x) { m_annoyedRestTickMax = x; }
	int getMaxAnnoyedTick() const { return m_annoyedRestTickMax; }

	void setAnnoyed(bool x) { m_annoyed = x; }
	bool amIAnnoyed() { return m_annoyed; }

	bool amIResting() const { return m_resting; }


	int getMaxRestTick() const { return m_restTicks; } //i.e., 4
	void setMaxRestTickAs(int x) { m_restTicks = x; }
	void setCurrentRestTick(int x) { m_currentRestTick = x; }
	void countUpARestTick() { m_currentRestTick++; }
	int getCurrentRestTick() const { return m_currentRestTick; }

	protesterState getProtesterState() const { return m_pState; }
	void setProtesterState(protesterState s) { m_pState = s; }
	//Graph::Direction canSeeFrackMan() const;
	void rollNumberOfTimesToMoveInCurrentDirection() { m_numTimesCurrentDir = (rand() % 53) + 8; }

	virtual void performGiveUpAction();
	//virtual void Protester::performAnnoyedAction();

	virtual void Protester::respondToPlayer(FrackMan* player, double distanceOfInteraction);
	virtual void respondToSquirt(Squirt* squirt, double distanceOfInteraction);
	virtual void Protester::respondToBoulder(Boulder* boulder, double distanceOfInteraction);
	virtual void Protester::respondToBribe(Gold* bribe, double distanceOfInteraction);
	void Protester::startToLeave();





	virtual void getHurt(int damage);


protected:


private:
	bool m_resting;
	int m_restTicks;
	int m_currentRestTick;
	int m_currentNonrestTick;
	int m_currentNonTurnedTick;
	int m_turnPeriod;
	int m_currentCoolDownTick;
	int m_coolDownPeriod;
	int m_numTimesCurrentDir;
	int m_annoyedRestTick;
	int m_annoyedRestTickMax;

	protesterState m_pState;

	bool m_annoyed;

};


class HardcoreProtester :public Protester
{
public:
	HardcoreProtester(CoordType x, CoordType y, StudentWorld* sw, int IID = IID_HARD_CORE_PROTESTER):Protester(x, y, sw, IID)
	{
		//m_bribed = false;
		setHealth(HARDCORE_PROTESTER_START_HEALTH);
		setDetectionRange();
	}
	virtual int doSomething();
	int howFarAwayAmIFromFrackMan() const;

	virtual void HardcoreProtester::respondToSquirt(Squirt* squirt, double distanceOfInteraction);
	virtual void respondToBribe(Gold* bribe, double distanceOfInteraction);
	virtual GraphObject::Direction HardcoreProtester::tryToGetToFrackMan() const;


	//virtual Direction tryToGetToFrackMan() const;

protected:
	void setDetectionRange();
	int getDetectionRange() const { return m_detectionRange; }
private:
	//bool m_bribed;
	int m_detectionRange;
};




//from spec (double-check though!)
const double DISTANCE_INTERACT = 3;
const double DISTANCE_DISCOVER = 4;
const double DISTANCE_YELL = 4;
const double DISTANCE_PLACEMENT = 6;
const double DISTANCE_USE_SONAR = 12;



void initializeDistanceActionMap();


const std::vector<double> DISTANCES = { DISTANCE_INTERACT, DISTANCE_DISCOVER };


const int SCORE_INVALID = -1;
const int SCORE_SONAR = 75;
const int SCORE_BARREL = 1000;
const int SCORE_GOLD_FRACKMAN = 10;
const int SCORE_GOLD_PROTESTER = 25;
const int SCORE_GOLD_HARDCORE_PROTESTER = 50;
const int SCORE_WATER_POOL = 100;

class Goodie :public Actor
{
public:
	Goodie(int IID,  StudentWorld* sw, CoordType x, CoordType y, int score = SCORE_INVALID, unsigned int depth = DEPTH_GOODIE) :
		Actor(IID, depth, sw, NORMAL_IMAGE_SIZE, x, y)
	{
		setVisibility(false);
		//setVisible(false); //most start off invisible
		//setVisibleFlag(false);
		m_score = score;
		setGroupAs(goodies);
		//m_doITick = false; //most don't tick
	}
	virtual ~Goodie() { };

	virtual int doSomething();

	virtual void respondToPlayer(FrackMan* player, double distanceOfInteraction);
	virtual void Goodie::interactWithPlayer(FrackMan* player, double distanceOfInteraction) = 0;
	virtual void Goodie::becomeDiscoveredByPlayer();
	virtual void respondToEnemy(Protester* enemy, double distanceOfInteraction) {};


	//picking up Goodies gives a score...
	int giveScore() const { return m_score; }

	//virtual bool shouldITick() const { return m_doITick; } 


protected:
	void setScore(int score) { m_score = score; }
private:
	int m_score;
};

class Sonar : public Goodie
{
public:
	Sonar(CoordType x, CoordType y, StudentWorld* sw);
	virtual ~Sonar() {};

	//virtual int doSomething();

	virtual void interactWithPlayer(FrackMan* player, double distanceOfInteraction);


private:


};


class Barrel :public Goodie
{
public:
	Barrel(CoordType x, CoordType y, StudentWorld* sw, int score, int IID);
	virtual ~Barrel() {};

	virtual int doSomething();
	virtual void Barrel::interactWithPlayer(FrackMan* player, double distanceOfInteraction);


};

class Water :public Goodie
{
public:
	Water(CoordType x, CoordType y, StudentWorld* sw);
	virtual ~Water() {};


	virtual void interactWithPlayer(FrackMan* player, double distanceOfInteraction);


};

class Gold :public Goodie
{
public:
	Gold(CoordType x, CoordType y, StudentWorld* sw, int score, int IID);

	virtual ~Gold() {};

	virtual int doSomething();

	virtual void interactWithPlayer(FrackMan* player, double distanceOfInteraction);
	virtual void respondToEnemy(Protester* enemy, double distanceOfInteraction);


private:

};


enum boulderState { stable, waiting, falling };
const int WAIT_TIME_BOULDER = 30;
class Boulder : public DynamicObject
{
public:
	Boulder(CoordType x, CoordType y, StudentWorld* sw, int IID, unsigned int depth);

	void respondToPlayer(FrackMan* player, double distanceOfInterest);


	virtual ~Boulder() {};
	virtual int doSomething();

	boulderState getBoulderState() const { return m_state; }

	virtual void performTickAction();

	void setBoulderState(boulderState x) { m_state = x; }
	virtual void Boulder::respondToEnemy(Protester* enemy, double distanceOfInterest);




private:
	boulderState m_state;
	int m_ticksLeft;
	//bool m_haveWaited;
};


GraphObject::Direction generateRandomDirection();


// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


// Helper functions

//Returns TOUCHING if the two Actor sprites will overlap
//	if they move one square closer to each other in one of the coordinates
//Returns OVERLAPPING if the two Actor sprites are currently overlapping
//Returns EXISTS_GAP if there is one or more squares of distance between the two Actors
//	in both the X and Y axes
//int overlap(Actor* a, Actor* b);


#endif // ACTOR_H_
