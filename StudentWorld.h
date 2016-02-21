#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

#include <vector>
#include "Actor.h"

//#include "GraphObject.h"


class Actor;
class Dirt;
class FrackMan;
//enum Direction;

const int SPRITE_HEIGHT_R = SPRITE_HEIGHT / 2; //the "radius" of the y-coordinate of box
											   //to use the center of sprite for location
const int SPRITE_WIDTH_R = SPRITE_WIDTH / 2;


class StudentWorld : public GameWorld
{
public:
	//construct/destruct
	StudentWorld(std::string assetDir);
	virtual ~StudentWorld();

	bool hasPlayerWon() const { return (m_barrelsLeft == 0); }

	void StudentWorld::setDisplayText();
	std::string formatDisplayText(int score, int level, int lives,
		int health, int squirts, int gold, int sonar, int barrelsLeft) const;
	/*
	std::string createGameStatText() const
	{
		//std::string text = "";
		//text = "Scr: " + (string)GameWorld::getScore() + "Lvl: " + 

		std::cout << "Scr: " << GameWorld::getScore() <<
			"Lvl: " << GameWorld::getLevel() <<
			"Lives: " << GameWorld::getLives() <<
			"Hlth: " << calculatePlayerHealth() <<

	}
	*/

	void StudentWorld::removeDeadActors();

	//the bread and butter
	virtual int init();
	virtual int move();
	virtual void cleanUp();


	bool removeDirtForFrackMan(const GraphObject::Direction dir, const CoordType& x, const CoordType& y);


	std::vector<Actor*>* getActors() { return &(m_actors); }
	FrackMan* getPlayer() { return m_player; }

	bool StudentWorld::isActorAffectedByGroup(const Actor* caller, Group g, const int& statusOfInterest) const;
	bool StudentWorld::isLocationAffectedByGroup(const CoordType& x, const CoordType& y, Group g, const int& statusOfInterest) const;


	//bool StudentWorld::attemptMove(DynamicObject* caller, const GraphObject::Direction dir);
	bool StudentWorld::tryToMoveMe(DynamicObject* caller, const GraphObject::Direction moveDir);

	double distanceBetweenActors(const Actor* a, const Actor* b) const;
	double distanceBetweenLocationAndActor(const CoordType& x, const CoordType& y, const Actor* b) const;

	Dirt*** getDirts() { return m_dirts; }

	void changeBarrelsLeftBy(int x) { m_barrelsLeft += x; }

	bool isThereDirtAt(int x, int y) { return (m_dirts[x][y] != nullptr); }

	bool StudentWorld::placeItemIntoGrid(Actor* a);
	bool StudentWorld::generateAppropriatePossibleLocation(int& x, int& y, const int& ID);


	bool isInvalidLocation(int x, int y) const { return (x < 0 || x > X_UPPER_BOUND || y < 0 || y > Y_UPPER_BOUND); }

	int overlap(const Actor* a, const Actor* b) const;
	bool StudentWorld::isThereDirtInDirectionOfActor(const Actor* caller) const;
	bool StudentWorld::removeDirtFromLocation(const int& x, const int& y);
	void StudentWorld::removeDirtForActor(const Actor* a);

private:

	//std::string prependCharToStringToSize(std::string s, char c, int size);





	void setUpDirt();
	void cleanUpDirt();
	void initDirt();
	bool cleanUpActorsAndFrackMan();


	//keep track of how many of these items are left in the level
	int m_barrelsLeft;
	int m_goldLeft;
	int m_bouldersLeft;
	//int m_squirts;

	//int nums[2]; // nums is a ints*
	std::vector<Actor*> m_actors;
	FrackMan* m_player;
	Dirt*** m_dirts;
	//std::vector<std::vector<Dirt*>*> m_dirts; //will hold dirt. m_dirts[0][0] refers to the top-left corner (63, 0)
	//Dirt*** m_dirts;// [VIEW_WIDTH][VIEW_HEIGHT]; //will hold Dirt*. m_dirts[0][0] refers to the top-left corner (63, 0)
	//a 2D array holding Dirt* pointers.
	//

};



// Helper functions

double distance(int x1, int y1, int x2, int y2);
int min(int x, int y);
int max(int x, int y);
//int overlap(Actor* a, Actor* b);


#endif // STUDENTWORLD_H_
