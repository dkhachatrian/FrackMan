#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

#include <vector>

//#include "GameWorld.h" //need to access getKey() in FrackMan class

class StudentWorld;

typedef int CoordType; //for coordinates of Actors
typedef double LengthType; //for sprite sizes (in X and Y) of Actors

const int X_UPPER_BOUND = VIEW_WIDTH - SPRITE_WIDTH; //cannot reach upper bound. ==60
const int Y_UPPER_BOUND = VIEW_HEIGHT - SPRITE_HEIGHT; //== 60

const int PLAYER_START_HEALTH = 10;
const int PLAYER_START_SQUIRTS = 5;

//for the moveTo functions
const int UP_DIR = 1;
const int DOWN_DIR = -1;
const int LEFT_DIR = -1;
const int RIGHT_DIR = 1;

//for putAtSpriteCorner
enum Corner {bottom_left, bottom_right, top_right, top_left, NA};
//for StudentWorld::overlap()
const std::vector<Corner> corners = { bottom_left, bottom_right, top_right, top_left };
const Corner DEFAULT_CORNER = bottom_left;

const int TOUCHING = 0;
const int OVERLAPPING = -1;
const int EXISTS_GAP = 1;


const int INVALID_IID = -1;


const double NORMAL_IMAGE_SIZE = 1;
const double DIRT_IMAGE_SIZE = 0.25;

const int PLAYER_START_X = 30;
const int PLAYER_START_Y = 60;


const unsigned int DEPTH_INVALID = -1; //a huge number actually
const unsigned int DEPTH_PLAYER = 0;
const unsigned int DEPTH_BOULDER = 1;
const unsigned int DEPTH_GOODIE = 2;
const unsigned int DEPTH_DIRT = 3;

//const string DIRT_ID = "dirt";



//doSomething() return values
const int DEAD = -1;
const int MOVED = 0;
const int COLLECTED = 1;
const int STATIONARY = 2;
const int ANNOYED = 3;
const int DISCOVERED = 4;


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
	Actor( int IID,  unsigned int depth, StudentWorld* sw, double imageSize, CoordType x = -1, CoordType y = -1, bool solidity = false):
		GraphObject(IID, x, y, right, imageSize, depth)
	{
		m_sw = sw;
		setVisible(true); //by default
		m_is_solid = solidity; //everything except Boulders is not solid
		m_width = imageSize * SPRITE_WIDTH;
		m_height = imageSize * SPRITE_HEIGHT;
		m_visible = true; //by default

	}


	virtual ~Actor() { setVisible(false); }

	//doSomething()
	virtual int doSomething() = 0;
	//virtual int doSomething();
	
	//gives bottom-left corner (i.e. what's returned by getX/Y)
	void sendLocation(CoordType& x, CoordType& y) const
	{
		x = getX();
		y = getY();
	}

	StudentWorld* getWorld() const { return m_sw; }
	bool isSolid() const{ return m_is_solid; }
	int getIdentity() const{ return m_id; }
	double getWidth() const { return m_width; }
	double getHeight() const { return m_height; }
	bool isDead() const { return m_isDead; }
	bool isVisible()  const { return m_visible; }

	bool isThereDirtBelowMe() const;

	//to be used for overlap function
	double getMaxLength() const;

	//Gives "effective" (x,y) based on sprite size and current direction
	//Returns true if the input parameters x or y were changed. Otherwise returns false.
	bool sendEffectiveLocation(CoordType& x, CoordType& y, const Direction dir) const;

	// Reverses the actions of sendToEffectiveLocation on the parameters x and y
	void reverseTransform(CoordType& x, CoordType& y, const Direction dir) const;

	void putAtSpriteCorner(Corner c, CoordType& x, CoordType& y) const;

	bool isInsideMySprite(const CoordType& x, const CoordType& y) const;

	Corner relativeLocationTo(const Actor* other) const;

protected:
	void setIdentityAs(int id) {m_id = id;}

	void die() { m_isDead = true; }

	//helper functions

	void setVisibleFlag(bool x) { m_visible = x; }
	void setSolidityAs(bool state) { m_is_solid = state; }

private:
	StudentWorld* m_sw;
	bool m_is_solid;
	int m_id;
	CoordType m_width;
	CoordType m_height;
	bool m_isDead;
	bool m_visible;
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

	Dirt(StudentWorld* sw) :Actor(IID_DIRT, DEPTH_DIRT, sw, DIRT_IMAGE_SIZE)
	{
		setDirection(right);
		setVisible(true);
		setIdentityAs(IID_DIRT);
	};

	virtual ~Dirt()
	{
		setVisible(false);
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
	DynamicObject(int IID, unsigned int depth, StudentWorld* sw, double imageSize = NORMAL_IMAGE_SIZE) :Actor(IID, depth, sw, imageSize)
	{};

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

};

//whether the movable object is annoyed
//virtual bool isAnnoyed() = 0;
//virtual int attemptAnnoyedAction() = 0;


class AnnoyableActor :public DynamicObject
{
public:
	AnnoyableActor(int IID, unsigned int depth, StudentWorld* sw, double imageSize = NORMAL_IMAGE_SIZE) :
		DynamicObject(IID, depth, sw, imageSize)
	{
		m_annoyed = false;
	}


	virtual bool isAnnoyed() const { return m_annoyed; }
	virtual bool shouldIBeAnnoyed() const = 0;
	//virtual int attemptAnnoyedAction() = 0;

protected:
	virtual void setAnnoyed(bool x) { m_annoyed = x; };
private:
	bool m_annoyed;
};




class FrackMan :public AnnoyableActor
{
public:
	FrackMan(StudentWorld* sw):AnnoyableActor(IID_PLAYER, DEPTH_PLAYER, sw)
	{
		m_hp = PLAYER_START_HEALTH;
		m_squirts = PLAYER_START_SQUIRTS;
		m_gold = 0;
		m_sonar = 1;
		setIdentityAs(IID_PLAYER);
		setDirection(right);
	};

	virtual int doSomething();

	virtual bool attemptMove(const Direction dir);

	//for the purposes of compiling...
	virtual bool isAnnoyed() { return false; }
	virtual int attemptAnnoyedAction() { return 0; }
	virtual bool shouldIBeAnnoyed() const { return false; }

	int getHealth() const { return m_hp; }
	bool isDead() const { return m_hp <= 0; }
	void changeHealthBy(int x) { m_hp += x; }

	int getSquirts() const { return m_squirts; }
	void changeSquirtsBy(int x) { m_squirts += x; }

	int getGold() const { return m_gold; }
	void changeGoldBy(int x) { m_gold += x; }

	int getSonar() const { return m_sonar; }
	void changeSonarBy(int x) { m_sonar += x; }


	virtual bool doSpecializedAction();
	//bool removeDirt(const Direction dir, const int& x, const int& y);

protected:
	virtual void setAnnoyed() {};
	//virtual bool setDir(int dir);
private:
	int m_hp;
	int m_squirts;
	int m_gold;
	int m_sonar;
};


class Protestor :public Actor
{
public:

private:

};



//entirely made to have a branch to oppose DynamicObject -- doesn't actually add much in terms of features
// (at least, that I can think of at the moment...)
class StaticObject :public Actor
{
public:
	StaticObject(int IID, unsigned int depth, StudentWorld* sw, double imageSize = NORMAL_IMAGE_SIZE, bool solidity = false) :
		Actor(IID, depth, sw, imageSize)
	{

	}
	
	virtual ~StaticObject() {};

};


enum Group {player, enemies, anyone};
//from spec (double-check though!)
const double DISTANCE_COLLECT = 3;
const double DISTANCE_DISCOVER = 4;
const double DISTANCE_PLACEMENT = 6;

const int SCORE_SONAR = 75;
const int SCORE_BARREL = 1000;
const int SCORE_GOLD = 10;

class Goodie :public StaticObject
{
public:
	Goodie(int IID, int score,  StudentWorld* sw, CoordType x, CoordType y, unsigned int depth = DEPTH_GOODIE, Group canPickMeUp = player) :
		StaticObject(IID, depth, sw)
	{
		setVisible(false); //most start off invisible
		setVisibleFlag(false);
		m_score = score;
		m_whoCanPickMeUp = player;
	}
	virtual ~Goodie() {};

	virtual int doSomething();

	//picking up Goodies gives a score...
	int giveScore() const { return m_score; }
	Group whoCanPickMeUp() const { return m_whoCanPickMeUp; }

	bool shouldITick() { return false; } //most Goodies don't tick (?)


protected:
	void setScore(int score) { m_score = score; }
	void countDownATick() { m_ticksLeft--; }
	bool doITick() const { return m_doITick; }
	void setTickStatus(bool x) { m_doITick = x; }
	void setPickUpGroup(Group g) { m_whoCanPickMeUp = g; }
private:
	int m_score;
	int m_ticksLeft;
	bool m_doITick;
	Group m_whoCanPickMeUp;
};

class Sonar : public Goodie
{
public:
	Sonar(CoordType x, CoordType y, StudentWorld* sw, int score, int IID, Group canPickMeUp);
	virtual ~Sonar() {};

	virtual int doSomething();

private:


};


class Barrel :public Goodie
{
public:
	Barrel(CoordType x, CoordType y, StudentWorld* sw, int score, int IID);
	virtual ~Barrel() {};

	virtual int doSomething();
};

class Water :public Goodie
{
public:
	Water(CoordType x, CoordType y, StudentWorld* sw, int score, int IID);


	virtual int doSomething();
};

class Gold :public Goodie
{
public:
	Gold(CoordType x, CoordType y, StudentWorld* sw, int score, int IID);

	bool wasDroppedByFrackMan() const { return m_frack; }
	virtual int doSomething();

private:
	bool m_frack;
	int m_ticksLeft;
};


enum state { stable, waiting, falling };
const int WAIT_TIME_BOULDER = 30;
class Boulder : public DynamicObject
{
public:
	Boulder(CoordType x, CoordType y, StudentWorld* sw, int IID = IID_BOULDER, unsigned int depth = DEPTH_BOULDER) :
		DynamicObject(IID, depth, sw)
	{
		moveTo(x, y);
		m_state = stable;
		setDirection(down);
		setVisible(true); //just to be safe...
		m_ticksLeft = WAIT_TIME_BOULDER;
	}
	virtual ~Boulder() {};
	virtual int doSomething() {};

	state checkState() const { return m_state; }

private:
	state m_state;
	int m_ticksLeft;
};





// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp


// Helper functions

//Returns TOUCHING if the two Actor sprites will overlap
//	if they move one square closer to each other in one of the coordinates
//Returns OVERLAPPING if the two Actor sprites are currently overlapping
//Returns EXISTS_GAP if there is one or more squares of distance between the two Actors
//	in both the X and Y axes
//int overlap(Actor* a, Actor* b);


#endif // ACTOR_H_
