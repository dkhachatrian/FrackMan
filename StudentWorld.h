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


	bool removeDirtForFrackMan();


	std::vector<Actor*>* getActors() { return &(m_actors); }
	FrackMan* getPlayer() { return m_player; }

	//bool StudentWorld::isActorAffectedByGroup(const Actor* caller, Group g, const int& statusOfInterest, bool usedSonar) const;
	//NOTE: incomplete
	bool StudentWorld::isActorAffectedByGroup(const Actor* caller, Group g, const int& statusOfInterest, bool usedSonar = false) const
	{
		double distanceOfInterest = -1;

		switch (statusOfInterest)
		{
		case DISCOVERED:
			if (usedSonar)
				distanceOfInterest = DISTANCE_USE_SONAR;
			else
				distanceOfInterest = DISTANCE_DISCOVER;
			break;
		case INTERACTED:
			distanceOfInterest = DISTANCE_INTERACT;
			break;
		case PLACED: //only on init
			distanceOfInterest = DISTANCE_PLACEMENT;
			break;
		}


		switch (g)
		{
		case player:
			if (distanceBetweenActors(caller, m_player) <= distanceOfInterest)
				return true;
			break;
		case enemies:
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				//filter condition
				if (!(p->getIdentity() == IID_PROTESTER || p->getIdentity() == IID_HARD_CORE_PROTESTER))
					continue;
				if (distanceBetweenActors(caller, p) <= distanceOfInterest)
					return true;
			}
			break;
		case boulders:
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				//filter condition
				if (!(p->getIdentity() == IID_BOULDER))
					continue;
				if (distanceBetweenActors(caller, p) <= distanceOfInterest)
					return true;
			}
			break;

		case goodies:
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				//filter condition
				if (!(p->getDepth() == DEPTH_GOODIE)) //conveniently, only Goodies are on Layer 2
					continue;
				if (distanceBetweenActors(caller, p) <= distanceOfInterest)
					return true;
			}
			break;
		case anyone:
			if (distanceBetweenActors(caller, m_player) <= distanceOfInterest)
				return true;
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				if (distanceBetweenActors(caller, p) <= distanceOfInterest)
					return true;
			}
			break; //will fill in if it ends up being necessary...
		}
		//if it didn't match with anyone, it hasn't been picked up
		return false;
	}

	bool StudentWorld::isActorAffectedByActor(const Actor* a, const Actor* b, const int& statusOfInterest, bool usedSonar = false) const
	{
		double distanceOfInterest = -1;

		switch (statusOfInterest)
		{
		case DISCOVERED:
			if (usedSonar)
				distanceOfInterest = DISTANCE_USE_SONAR;
			else
				distanceOfInterest = DISTANCE_DISCOVER;
			break;
		case INTERACTED:
			distanceOfInterest = DISTANCE_INTERACT;
			break;
		case PLACED: //only on init
			distanceOfInterest = DISTANCE_PLACEMENT;
			break;
		}

		if (distanceBetweenActors(a, b) <= distanceOfInterest)
			return true;
		else return false;

	}

	bool StudentWorld::attemptToInteractWithNearbyActors(const Actor* caller);


	void StudentWorld::letPlayerDropGold();
	void letPlayerUseSonar();
	void letPlayerFireASquirt();
	
	//NOTE: incomplete
	bool StudentWorld::isLocationAffectedByGroup(const CoordType& x, const CoordType& y, Group g, const int& statusOfInterest, bool usedSonar = false) const
	{
		double distanceOfInterest = -1;

		switch (statusOfInterest)
		{
		case DISCOVERED:
			if (usedSonar)
				distanceOfInterest = DISTANCE_USE_SONAR;
			else
				distanceOfInterest = DISTANCE_DISCOVER;
			break;
		case INTERACTED:
			distanceOfInterest = DISTANCE_INTERACT;
			break;
		case PLACED: //only on init
			distanceOfInterest = DISTANCE_PLACEMENT;
			break;
		}


		switch (g)
		{
		case player:
			if (distanceBetweenLocationAndActor(x, y, m_player) <= distanceOfInterest)
				return true;
			break;
		case enemies:
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				//filter condition
				if (!(p->getIdentity() == IID_PROTESTER || p->getIdentity() == IID_HARD_CORE_PROTESTER))
					continue;
				if (distanceBetweenLocationAndActor(x, y, p) <= distanceOfInterest)
					return true;
			}
			break;
		case boulders:
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				//filter condition
				if (!(p->getIdentity() == IID_BOULDER))
					continue;
				if (distanceBetweenLocationAndActor(x, y, p) <= distanceOfInterest)
					return true;
			}
			break;

		case goodies:
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				//filter condition
				if (!(p->getDepth() == DEPTH_GOODIE)) //conveniently, only Goodies are on Layer 2
					continue;
				if (distanceBetweenLocationAndActor(x, y, p) <= distanceOfInterest)
					return true;
			}
			break;
		case anyone:
			if (distanceBetweenLocationAndActor(x, y, m_player) <= distanceOfInterest)
				return true;
			for (int i = 0; i < m_actors.size(); i++)
			{
				Actor* p = m_actors[i];
				if (distanceBetweenLocationAndActor(x, y, p) <= distanceOfInterest)
					return true;
			}
			break; //will fill in if it ends up being necessary...
		}
		//if it didn't match with anyone, it hasn't been picked up
		return false;
	}


	GraphObject::Direction StudentWorld::tellMeHowToGetToMyGoal(const Actor* caller, CoordType x_goal, CoordType y_goal) const;

	GraphObject::Direction StudentWorld::HowToGetFromLocationToGoal(CoordType x_actor, CoordType y_actor, CoordType x_goal, CoordType y_goal) const;

	int StudentWorld::numberOfStepsFromLocationToGoal(CoordType x_actor, CoordType y_actor, CoordType x_goal, CoordType y_goal) const;

	bool StudentWorld::isThereSpaceForAnActorHere(CoordType x, CoordType y) const;


	//bool StudentWorld::attemptMove(DynamicObject* caller, const GraphObject::Direction dir);
	bool StudentWorld::tryToMoveMe(DynamicObject* caller, const GraphObject::Direction moveDir);

	double distanceBetweenActors(const Actor* a, const Actor* b) const;
	double distanceBetweenLocationAndActor(const CoordType& x, const CoordType& y, const Actor* b) const;

	Dirt*** getDirts() { return m_dirts; }

	void changeBarrelsLeftBy(int x) { m_barrelsLeft += x; }

	bool isThereDirtAt(int x, int y) const { return (m_dirts[x][y] != nullptr); }

	bool StudentWorld::placeItemIntoGrid(Actor* a);
	bool StudentWorld::generateAppropriatePossibleLocation(int& x, int& y, const int& ID);


	bool tryToMoveFromLocation(CoordType& x, CoordType& y, const GraphObject::Direction moveDir) const;

	bool StudentWorld::bribeEnemy(const Actor* caller) const;


	void StudentWorld::moveCoordsInDirection(CoordType& x, CoordType& y, GraphObject::Direction dir) const;

	bool StudentWorld::amIFacingFrackMan(const Actor* caller) const;
	GraphObject::Direction StudentWorld::directLineToFrackMan(const Actor* caller) const;
	GraphObject::Direction StudentWorld::canITurnAndMove(const Actor* caller) const;

	//for debugging
	void StudentWorld::setAllActorsAsVisible();

	bool isInvalidLocation(int x, int y) const { return (x < 0 || x > X_UPPER_BOUND || y < 0 || y > Y_UPPER_BOUND); }

	int overlap(const Actor* a, const Actor* b) const;


	bool StudentWorld::isThereDirtInDirectionOfActor(const Actor* caller, GraphObject::Direction dir) const;
	bool StudentWorld::isThereDirtInDirection(GraphObject::Direction dir, CoordType x, CoordType y, CoordType height, CoordType width) const;
	bool StudentWorld::removeDirtFromLocation(const int& x, const int& y);
	void StudentWorld::removeDirtForBoulder(const Actor* a);

private:

	//std::string prependCharToStringToSize(std::string s, char c, int size);

	void setUpDirt();
	void cleanUpDirt();
	void initDirt();
	bool cleanUpActorsAndFrackMan();
	int getNumberOfEnemies() const;


	//keep track of how many of these items are left in the level
	int m_barrelsLeft;
	int m_goldLeft;
	int m_bouldersLeft;
	int m_goodieDenominator;
	int m_ticksBetweenEnemySpawns;
	int m_numberOfTicksToWaitBetweenEnemySpawns;
	int m_targetEnemyNumber;
	int m_probabilityHardcoreSpawn;
	int m_numEnemies;
	
	//GraphObject::Direction** m_howToLeave;

	bool StudentWorld::isThereABoulderAt(CoordType x, CoordType y) const;


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
double angleBetweenTwoPoints(int x1, int y1, int x2, int y2);
int min(int x, int y);
int max(int x, int y);
//int overlap(Actor* a, Actor* b);


#endif // STUDENTWORLD_H_
