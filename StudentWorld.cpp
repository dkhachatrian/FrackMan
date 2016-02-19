#include "StudentWorld.h"
#include <string>
using namespace std;

#include "Actor.h"
#include "GameWorld.h"

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}


int StudentWorld::init()
{
	setUpDirt();
	return 0; //change later
}


int StudentWorld::move()
{

	m_player->doSomething();
	return GWSTATUS_CONTINUE_GAME;

	/*
	// Update the Game Status Line
	updateDisplayText(); // update the score/lives/level text at screen top
						 // The term “Actors” refers to all Protesters, the player, Goodies,
						 // Boulders, Barrels of oil, Holes, Squirts, the Exit, etc.
						 // Give each Actor a chance to do something
	for each of the actors in the game world
	{
		if (actor[i] is still active / alive)
		{
			// ask each actor to do something (e.g. move)
			tellThisActorToDoSomething(actor[i]);
			if (theplayerDiedDuringThisTick() == true)
				return GWSTATUS_PLAYER_DIED;
			if (theplayerCompletedTheCurrentLevel() == true)
			{
				return GWSTATUS_FINISHED_LEVEL;
			}
		}
	}
		// Remove newly-dead actors after each tick
	removeDeadGameObjects(); // delete dead game objects
							 // return the proper result
	if (theplayerDiedDuringThisTick() == true)
		return GWSTATUS_PLAYER_DIED;
	// If the player has collected all of the Barrels on the level, then
	// return the result that the player finished the level
	if (theplayerCompletedTheCurrentLevel() == true)
	{
		playFinishedLevelSound();
		return GWSTATUS_FINISHED_LEVEL;
	}
	// the player hasn’t completed the current level and hasn’t died
	// let them continue playing the current level
	return GWSTATUS_CONTINUE_GAME;
	*/

	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//return GWSTATUS_CONTINUE_GAME;


	//decLives();
	//return GWSTATUS_PLAYER_DIED;
}

void StudentWorld::cleanUp()
{

}


StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	std::vector<Actor*> m_actors;
	m_player = new FrackMan(this);
	m_dirts = nullptr;

	initDirt(); //init's m_dirts

				//start with empty vector
				//std::vector<std::vector<Dirt*> > m_dirts;

				/*
				for (int i = 0; i < VIEW_WIDTH; i++)
				{
				std::vector<Dirt*>* vp = new std::vector<Dirt*>;

				for (int j = 0; j < VIEW_HEIGHT; j++)
				{
				vp->push_back(nullptr);
				}
				m_dirts.push_back(vp);
				}
				*/

				/*
				//create the array for m_dirts
				for (int i = 0; i < VIEW_WIDTH; i++)
				{
				m_dirts[i] = new Dirt*[VIEW_HEIGHT];
				}
				//make all the pointers inside the dirts point to nullptr
				*/
				//build up the 64x64 "array" of Dirt objects
				//initDirt();

}

bool StudentWorld::hasPlayerWon() const
{
	return (m_barrels == 0);
}






// Dirt is added BEFORE putting in Boulders
void StudentWorld::setUpDirt()
{
	//make extra sure the grid is clean

	//cleanUpDirt();

	//fill in the dirt
	for (int i = 0; i < VIEW_WIDTH; i++) //horizontal direction
	{
		for (int j = 0; j < VIEW_HEIGHT; j++) //vertical direction (STARTING FROM TOP)
		{

			if (j >= Y_UPPER_BOUND) //don't fill in top rows, to give space for initial Actors
			{
				m_dirts[i][j] = nullptr; //aka no dirt
			}
			else if (i >= (VIEW_HEIGHT/2 - SPRITE_WIDTH_R) //30
				&& i < (VIEW_HEIGHT/2 + SPRITE_WIDTH_R) //34
				&& j > (SPRITE_HEIGHT)) //4
													 //don't fill in the "mine shaft" (30<=x<34) 0<=y<60 according to array locations)
			{
				m_dirts[i][j] = nullptr; //aka no dirt
			}
			//if I got here, it's safe to fill in some dirt!
			//create new Dirt object and put it in the appropriate spot in the m_dirts[][]
			else
			{
				m_dirts[i][j] = new Dirt(i, j, this);
			}
			
		}


	}
}

//if m_dirts != nullptr, deletes all remaining Dirt objects, and sets the Dirt*'s to nullptr
void StudentWorld::cleanUpDirt()
{
	if (m_dirts == nullptr)
		return;

	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
			Dirt* p = m_dirts[i][j];
			if (p != nullptr)
			{
				delete p; //removes dynamically allocated Dirt objects
				p = nullptr;
			}

		}
		delete m_dirts[i]; //removes dynamically allocated Dirt** objects
	}
	delete m_dirts; //removes dynamically allocated Dirt*** object
}

//PRECONDITION: memory has been allocated to the cells in the 2D Dirt* array, m_dirts
//POSTCONDITION: all cells in m_dirts are initialized to nullptr (==> no Dirt object at that location)
void StudentWorld::initDirt()
{
	//allocates memory for 64 Dirt**'s
	m_dirts = new Dirt**[VIEW_HEIGHT];

	for (int i = 0; i < VIEW_WIDTH; i++)
	{
		m_dirts[i] = new Dirt*[VIEW_HEIGHT]; //allocates memory for 64 Dirt*'s
		for (int j = 0; j < VIEW_HEIGHT; j++)
		{
			m_dirts[i][j] = nullptr; //initializes everything to nullptr
		}

	}

}


bool StudentWorld::cleanUpActorsAndFrackMan()
{
	for (int i = 0; i < m_actors.size(); i++)
	{
		delete m_actors[i];
	}
	delete m_player;
	/*
	std::vector<Actor*>::iterator it = m_actors.begin();

	while (it != m_actors.end())
	{
		delete 
	}
	*/

	return true; //everything worked (?)
}


// Helper functions

double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}




// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

