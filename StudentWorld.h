#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

#include <vector>
#include "Actor.h"

class Actor;
class Dirt;
class FrackMan;

const int SPRITE_HEIGHT_R = SPRITE_HEIGHT / 2; //the "radius" of the y-coordinate of box
											   //to use the center of sprite for location
const int SPRITE_WIDTH_R = SPRITE_WIDTH / 2;


class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);

	virtual ~StudentWorld()
	{
		cleanUpDirt();
		cleanUpActorsAndFrackMan();
	}

	bool hasPlayerWon() const;

	//to be completed after Part 1
	void StudentWorld::setDisplayText()
	{
		int score = GameWorld::getScore();
		int level = GameWorld::getLevel();
		int lives = GameWorld::getLives();
		int health = m_player->getHealth();
		int squirts = m_player->getSquirts();
		int gold = m_player->getGold();
		int sonar = m_player->getSonar();
		/*
		int barrelsLeft = getNumberOfBarrelsRemainingToBePickedUp();
		// Next, create a string from your statistics, of the form:
		// “Scr: 0321000 Lvl: 52 Lives: 3 Hlth: 80% Water: 20 Gld: 3 Sonar: 1 Oil Left: 2”
		std::string s = someFunctionYouUseToFormatThingsNicely(score, level, lives,
			health, squirts, gold, sonar, barrelsLeft);
		// Finally, update the display text at the top of the screen with your
		// newly created stats
		setGameStatText(s); // calls our provided GameWorld::setGameStatText
		*/
	}


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



	virtual int init();

	virtual int move();

	virtual void cleanUp();

	bool cleanUpActorsAndFrackMan();

	std::vector<Actor*>* getActors()
	{
		return &(m_actors);
	}


	Dirt*** getDirts()
	{
		return m_dirts;
	}


	bool isThereDirtAt(int x, int y)
	{
		return (m_dirts[x][y] != nullptr);
	}


private:
	void setUpDirt();
	void cleanUpDirt();
	void initDirt();


	//keep track of how many of these items are left in the level
	int m_barrels;
	int m_gold;

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




#endif // STUDENTWORLD_H_
