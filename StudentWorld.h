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

	bool hasPlayerWon() const { return (m_barrels == 0); }

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


	//the bread and butter
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	bool cleanUpActorsAndFrackMan();

	bool removeDirt(const GraphObject::Direction dir, const CoordType& x, const CoordType& y);

	std::vector<Actor*>* getActors() { return &(m_actors); }


	//bool StudentWorld::attemptMove(DynamicObject* caller, const GraphObject::Direction dir);
	bool StudentWorld::tryToMoveMe(DynamicObject* caller, const GraphObject::Direction moveDir);



	Dirt*** getDirts() { return m_dirts; }


	bool isThereDirtAt(int x, int y) { return (m_dirts[x][y] != nullptr); }


	bool isInvalidLocation(int x, int y) const { return (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT); }

	int overlap(const Actor* a, const Actor* b) const;


private:

	//std::string prependCharToStringToSize(std::string s, char c, int size);


	void setUpDirt();
	void cleanUpDirt();
	void initDirt();


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

//int overlap(Actor* a, Actor* b);


#endif // STUDENTWORLD_H_
