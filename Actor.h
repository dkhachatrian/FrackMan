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
const unsigned int INVALID_DEPTH =-1; //a huge number actually
const double NORMAL_IMAGE_SIZE = 1;
const double DIRT_IMAGE_SIZE = 0.25;
const int DIRT_DEPTH = 3;
const int PLAYER_START_X = 30;
const int PLAYER_START_Y = 60;
const unsigned int PLAYER_DEPTH = 0;

//const string DIRT_ID = "dirt";


class Actor:public GraphObject
{
public:
	Actor(CoordType x, CoordType y, int IID, Direction dir, double imageSize, unsigned int depth, StudentWorld* sw, bool solidity = false):
		GraphObject(IID, x, y, dir, imageSize, depth)
	{
		m_sw = sw;
		setVisible(true); //by default
		m_is_solid = solidity; //everything except Boulders is not solid
		m_width = imageSize * SPRITE_WIDTH;
		m_height = imageSize * SPRITE_HEIGHT;

	}


	virtual ~Actor() {};

	//doSomething()
	virtual int doSomething() = 0;


	StudentWorld* getWorld() const { return m_sw; }
	bool isSolid() const{ return m_is_solid; }
	int getIdentity() const{ return m_identity; }
	double getWidth() const { return m_width; }
	double getHeight() const { return m_height; }

	//to be used for overlap function
	double getMaxLength() const;

	//Gives "effective" (x,y) based on sprite size and current direction
	//Returns true if the input parameters x or y were changed. Otherwise returns false.
	bool sendToEffectiveLocation(CoordType& x, CoordType& y, const Direction dir);

	// Reverses the actions of sendToEffectiveLocation on the parameters x and y
	void reverseTransform(CoordType& x, CoordType& y, const Direction dir);

	void putAtSpriteCorner(Corner c, CoordType& x, CoordType& y) const;

	bool isInsideMySprite(const CoordType& x, const CoordType& y) const;

	Corner relativeLocationTo(Actor* other) const;
	//accessor functions for GraphObject (should never change!)
	//maybe move these to protected?
	/*
	int getX() const
	{
		return m_go.getX();
	}
	int getY() const
	{
		return m_go.getY();
	}
	int getDir() const
	{
		return m_go.getDirection();
	}
	double getBrightness() const
	{
		return m_go.getBrightness();
	}
	int getSize() const
	{
		return m_go.getSize();
	}
	int getID() const
	{
		return m_go.getID();
	}
	*/

	/*
	virtual bool changeXBy(int x)
	{
		if (abs(x) > 1)
			exit(2); //only moving one subpixel at a time!
		m_x += x;
	}
	virtual bool changeYBy(int y)
	{
		if (abs(y) > 1)
			exit(2); //only moving one subpixel at a time!
		m_y += y;
	}
	*/
protected:
	void setIdentityAs(int id) {m_identity = id;}



	//helper functions


	void setSolidityAs(bool state)
	{
		m_is_solid = state;
	}

private:
	StudentWorld* m_sw;
	bool m_is_solid;
	int m_identity;
	CoordType m_width;
	CoordType m_height;
	//GraphObject m_go;

};

//only class with image size different from the rest
class Dirt :public Actor
{
public:
	//default constructor for the purposes of initializing the m_dirts array
	//Dirt() :Actor(-1, -1, IID_DIRT, right, DIRT_IMAGE_SIZE, DIRT_DEPTH, nullptr)
	//{
	//	setVisible(true);
	//};

	Dirt(int x, int y, StudentWorld* sw) :Actor(x, y, IID_DIRT, right, DIRT_IMAGE_SIZE, DIRT_DEPTH, sw)
	{
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


class DynamicObject :public Actor
{
public:
	DynamicObject(int x, int y, int IID, Direction dir, int depth, StudentWorld* sw) :Actor(x, y, IID, dir, NORMAL_IMAGE_SIZE, depth, sw)
	{};

	virtual ~DynamicObject()
	{};

	virtual bool attemptMove(const Direction dir);
	virtual bool doSpecializedAction() { return false; }
	
	//bool setDir(Direction dir) {}; //covered by GraphObject::setDirection(Direction d);

	//whether the movable object is annoyed
	virtual bool isAnnoyed() = 0;
	virtual int attemptAnnoyedAction() = 0;





	//changes x_s,y_s (copies of the coordinates of Actor who called it)
	// to where the Actor wants to go.
	// Returns true if no boundary or solid object (i.e. Boulder) is in the way, false otherwise.
	virtual bool canMoveInDirection(const Direction dir, int& x, int& y);


	// Does not change anything
	virtual bool moveMatchesDir(const Direction moveDir) const { return (GraphObject::getDirection() == moveDir); }

};

/*
class StaticObject :public Actor
{
public:
	StaticObject(int x, int y, int IID, Direction dir, int depth, StudentWorld* sw) :Actor(x, y, IID, dir, NORMAL_IMAGE_SIZE, depth, sw)
	{};

	virtual ~StaticObject()
	{};

};
*/





class FrackMan :public DynamicObject
{
public:
	FrackMan(StudentWorld* sw):DynamicObject(PLAYER_START_X, PLAYER_START_Y, IID_PLAYER, right, PLAYER_DEPTH, sw)
	{
		m_hp = PLAYER_START_HEALTH;
		m_squirts = PLAYER_START_SQUIRTS;
		m_gold = 0;
		m_sonar = 1;
		setIdentityAs(IID_PLAYER);
	};

	virtual int doSomething();

	virtual bool attemptMove(const Direction dir);

	//for the purposes of compiling...
	virtual bool isAnnoyed() { return false; }
	virtual int attemptAnnoyedAction() { return 0; }

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
	bool removeDirt(const Direction dir, const int& x, const int& y);

protected:
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



class Goodie :public Actor
{
public:

protected:

};

class Water :public Goodie
{

};

class Gold :public Goodie
{

};

class Oil :public Goodie
{

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
