#include "StudentWorld.h"
#include <string>
using namespace std;

#include "Actor.h"
#include "GameWorld.h"

#include "GraphObject.h" //so it knows what Direction's are

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}




StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	std::vector<Actor*> m_actors;
	m_player = new FrackMan(this);
	m_dirts = nullptr;

	initDirt();

}



int StudentWorld::init()
{
	setUpDirt();
	m_player->moveTo(PLAYER_START_X, PLAYER_START_Y); //just in case ...
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
	cleanUpDirt();
	cleanUpActorsAndFrackMan();
}

StudentWorld::~StudentWorld()
{
	cleanUpDirt();
	cleanUpActorsAndFrackMan();
}



bool StudentWorld::hasPlayerWon() const
{
	return (m_barrels == 0);
}


//x and y start at the topmost or leftmost square of dirt to be removed depending on the direction of movement:
// "up or down" --> x is leftmost
// "left or right" --> y is topmost
bool StudentWorld::removeDirt(const GraphObject::Direction dir, const int& x, const int& y)
{
	bool result = false;

	int x_eff = x;
	int y_eff = y;

	//currently glitching at some points...

	switch (dir)
	{
	case GraphObject::up:
		//int x_eff = x + SPRITE_WIDTH; //the coordinates are mapped to bottom-left corner of sprite
	case GraphObject::down:
		for (int i = 0; i < SPRITE_WIDTH; i++) //make room for sprite's width
		{
			Dirt* p = m_dirts[x_eff + i][y_eff];
			if (p != nullptr) //if there's Dirt in the way
			{
				result = true; //report that to FrackMan call
				delete p; //delete old Object
				m_dirts[x_eff + i][y_eff] = nullptr; //show that it's empty (CANNOT USE P ANYMORE)
			}
		}
		break;
	case GraphObject::left:
		//int y_eff = y + SPRITE_HEIGHT; //the coordinates are mapped to bottom-left corner of sprite
	case GraphObject::right:
		for (int j = 0; j < SPRITE_HEIGHT; j++) //make room for sprite's height
		{
			//Dirt*** d = getWorld()->getDirts();
			Dirt* p = m_dirts[x_eff][y_eff + j];
			if (p != nullptr) //if there's Dirt in the way
			{
				result = true; //report that to FrackMan call
				delete p; //delete old Object
				m_dirts[x_eff][y_eff + j] = nullptr; //show that it's empty (CANNOT USE P ANYMORE)
			}
		}
		break;
	}

	return result;

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


//PRECONDITON:

bool StudentWorld::canMoveInDirection(const GraphObject::Direction moveDir, Actor* caller)
{

	int x_t = caller->getX();
	int y_t = caller->getY();



	bool transformed = caller->getEffectiveLocation(x_t, y_t, moveDir);

	switch (moveDir)
	{
	case GraphObject::up:
		//x = getX();
		y_t += UP_DIR;
		break;
	case GraphObject::down:
		//x = getX();
		y_t += DOWN_DIR;
		break;
	case GraphObject::left:
		x_t += LEFT_DIR;
		//y = getY();
		break;
	case GraphObject::right:
		x_t += RIGHT_DIR;
		//y = getY();
		break;
	}

	if (isInvalidLocation(x_t, y_t)) //if trying to go outside possible gridspace, bad
		return false;

	std::vector<Actor*>::iterator it = m_actors.begin();

	//check over all Actors
	while (it != m_actors.end())
	{
		if (overlap(caller, (*it)) == TOUCHING) //see if there's one where the Actor is trying to go
		{
			if ((*it)->isSolid()) //if the thing there is solid (i.e. Boulder), bad
				return false;
		}
		it++;
	}

	//otherwise, we should be good to go!
	//update (x,y) and get out

	if(transformed)
		caller->reverseTransform(x_t, y_t, moveDir); //fixes the possible translation done by getEffectiveLocation()
	//x_t or y_t has also already been moved by one square according to moveDir
	//and by this point we know it's a valid move, so we're good to update the caller's location!

	caller->moveTo(x_t, y_t);

	return true;
}


// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

//a is always a dynamicObject 


//overlap compares the bottom-left corner of each sprite to determine overlap (should always be a valid comparison)
int overlap(Actor* a, Actor* b)
{
	int xa = a->getX();
	int xb = b->getX();
	int ya = a->getY();
	int yb = b->getY();

	//GraphObject::Direction da = a->getDirection();



	if (((xa - xb) == SPRITE_WIDTH) ||
		((ya - xb) == SPRITE_HEIGHT))
		return TOUCHING;
	else if (((xa - xb) < SPRITE_WIDTH) ||
		((ya - yb) < SPRITE_HEIGHT))
		return OVERLAPPING;
	else return EXISTS_GAP;
}