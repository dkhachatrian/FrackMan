#include "StudentWorld.h"
#include <string>
using namespace std;

#include "Actor.h"
#include "GameWorld.h"

#include "GraphObject.h" //so it knows what Direction's are

//helper functions
int min(int x, int y);
int max(int x, int y);
double distance(int x1, int y1, int x2, int y2);
std::string prependCharToStringToSize(std::string s, char c, int size);




GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}
/*
bool StudentWorld::canItMoveInDirection(const DynamicObject* a, const GraphObject::Direction dir) const
{
	int x_t = x;
	int y_t = y;

	switch (dir)
	{
	case GraphObject::up:
		//x = getX();
		y_t = y + UP_DIR;
		break;
	case GraphObject::down:
		//x = getX();
		y_t = y + DOWN_DIR;
		break;
	case GraphObject::left:
		x_t = x + LEFT_DIR;
		//y = getY();
		break;
	case GraphObject::right:
		x_t = x + RIGHT_DIR;
		//y = getY();
		break;
	}

	if (isInvalidLocation(x_t, y_t)) //if trying to go outside possible gridspace, bad
		return false;



}
*/


StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
	//std::vector<Actor*> m_actors;
	//m_player = new FrackMan(this);
	//m_dirts = nullptr;
	//m_barrelsLeft = -1;
	//m_goldLeft = -1;
	//m_bouldersLeft = -1;

	//initDirt();

}


// for debugging
void StudentWorld::setAllActorsAsVisible()
{
	for (int i = 0; i < m_actors.size(); i++)
		m_actors[i]->setVisibility(true);
}


int StudentWorld::init()
{
	initDirt();
	setUpDirt();

	// reset FrackMan's inventory after every level
	m_player = new FrackMan(this, PLAYER_START_X, PLAYER_START_Y);

	CoordType x, y;
	
	/*
	//check to see if the FrackMan is where he should be and facing where he should be facing
	m_player->sendLocation(x, y);
	if(!(x == PLAYER_START_X && y == PLAYER_START_Y))
		m_player->moveTo(PLAYER_START_X, PLAYER_START_Y);
	if (m_player->getDirection() != GraphObject::right)
		m_player->setDir(GraphObject::right);
		*/

	//parameters for the level
	m_bouldersLeft = min(getLevel() / 2 + 2, 6);
	m_goldLeft = max(5 - getLevel() / 2, 2);
	m_barrelsLeft = min(2 + getLevel(), 20);
	m_goodieDenominator = (getLevel() * 25) + 300;
	m_targetEnemyNumber = max(25, (200 - getLevel()));
	m_numberOfTicksToWaitBetweenEnemySpawns = min(15, 2 + getLevel() * 1.5);
	m_ticksBetweenEnemySpawns = 0;
	m_probabilityHardcoreSpawn = min(90, getLevel() * 10 + 30); //in percent

	//place boulders
	int i = 0;
	while (i < m_bouldersLeft)
	{
		bool foundSpot = generateAppropriatePossibleLocation(x, y, IID_BOULDER);

		if(foundSpot)
		{ 
			Boulder* p = new Boulder(x, y, this, IID_BOULDER, DEPTH_BOULDER);
			p->moveTo(x, y);
			removeDirtForBoulder(p);
			m_actors.push_back(p); //keep track of the Actors
			i++;
		}
		else exit(3); //bad...
		//otherwise it tries again
	}

	//then gold
	i = 0;
	while (i < m_goldLeft)
	{
		bool foundSpot = generateAppropriatePossibleLocation(x, y, IID_GOLD);

		if (foundSpot)
		{
			Gold* p = new Gold(x, y, this, SCORE_GOLD_FRACKMAN, IID_GOLD);
			m_actors.push_back(p); //keep track of the Actors
			i++;
		}
		else exit(3); //bad...
					  //otherwise it tries again
	}
	//then barrels
	i = 0;
	while (i < m_barrelsLeft)
	{
		bool foundSpot = generateAppropriatePossibleLocation(x, y, IID_BARREL);

		if (foundSpot)
		{
			Barrel* p = new Barrel(x, y, this, SCORE_BARREL, IID_BARREL);
			m_actors.push_back(p); //keep track of the Actors
			i++;
		}
		else exit(3); //bad...
					  //otherwise it tries again
	}
	
	//setUpDirt();

	//for debugging
	setAllActorsAsVisible();


	return 1; //change later
}

int StudentWorld::move()
{
	// Update the Game Status Line
	setDisplayText();// update the score/lives/level text at screen top
										   // The term “Actors” refers to all Protesters, the player, Goodies,
										   // Boulders, Barrels of oil, Holes, Squirts, the Exit, etc.
										   // Give each Actor a chance to do something

	if(m_ticksBetweenEnemySpawns == 0 && getNumberOfEnemies() < m_targetEnemyNumber)
	{
		if ((rand() % 100) < m_probabilityHardcoreSpawn)
		{
			//spawn a Hardcore Protester at (60,60)
		}
		else
		{
			//spawn a regular Protester at (60,60)
		}
	}

	//count a tick as we wait for a chance to spawn an enemy
	// (will spawn when m_ticksBetweenEnemySpawns modulo's back down to zero)
	m_ticksBetweenEnemySpawns = (m_ticksBetweenEnemySpawns + 1) % m_numberOfTicksToWaitBetweenEnemySpawns;

	//determine whether to add a WaterPool or SonarKit
	// (will allow more than one Goodie to be at the same location
	//if (rand() % 10 == 0)
	if (rand() % m_goodieDenominator == 0) //this is a 1 in m_goodieDenominator chance
	{
		int x = 0, y = 0;
		int rn = rand() % 5;
		//one in five chance it's a Sonar. Four-in-five chance it's a Water

		//Water Pool
		if (rn < 4)
		{
			do
			{
				//generate random coordinates
				x = rand() % X_UPPER_BOUND;
				y = rand() % Y_UPPER_BOUND;
				//generateAppropriatePossibleLocation(x, y, IID_WATER_POOL);
			}
			while (!isThereSpaceForAnActorHere(x, y)); //until it can fit
			//will give us a location that works
			Water* p = new Water(x, y, this);
			m_actors.push_back(p);
		}
		//Sonar
		else if (rn < 5) //aka 1 in 5 chance
		{
			Sonar* p = new Sonar(0, 60, this);
			m_actors.push_back(p);
		}
	}

	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		if(!p->isDead())
		{
			// ask each actor to do something (e.g. move)
			p->doSomething();
		}
	}

	m_player->doSomething();

	removeDeadActors();

	if (m_player->isDead())
	{
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	// If the player has collected all of the Barrels on the level, then
	// return the result that the player finished the level
	if (m_barrelsLeft == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	// the player hasn’t completed the current level and hasn’t died
	// let them continue playing the current level
	return GWSTATUS_CONTINUE_GAME;

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

void StudentWorld::removeDeadActors()
{
	std::vector<Actor*>::iterator it = m_actors.begin();

	while (it != m_actors.end())
	{
		Actor* p = (*it);
		if (p->isDead())
		{
			delete p; //free memory
			it = m_actors.erase(it); //move to the next one
		}
		else it++; //otherwise keep checking
	}

	return;
}


bool StudentWorld::cleanUpActorsAndFrackMan()
{
	//kill all the Actors
	for (int i = 0; i < m_actors.size(); i++)
	{
		m_actors[i]->die();
	}
	//then remove them all from the vector of Actor*
	removeDeadActors();

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


bool StudentWorld::placeItemIntoGrid(Actor* a)
{
	// generate a possible location on the grid
	CoordType x, y;

	bool canPlace = generateAppropriatePossibleLocation(x, y, a->getIdentity());

	if (!canPlace)
		return false; //can't place the item! Which might be worrisome...

	//interested in placing (upon init)


	//first will deal with Boulders
	//bool canPlace = true;

	//remove dirt if it's a boulder
	if (a->getIdentity() == IID_BOULDER)
	{

	}

	//place Actor in location
	a->moveTo(x, y);

	//update vector of Actors
	m_actors.push_back(a);

	//and get out
	return true;
}

int StudentWorld::getNumberOfEnemies() const
{
	int total = 0;
	for (int i = 0; i < m_actors.size(); i++)
	{
		if (m_actors[i]->whatGroupAmI() == enemies)
			total++;
	}
	return total;
}

void StudentWorld::removeDirtForBoulder(const Actor* a)
{
	for (int i = 0; i < a->getWidth(); i++)
		for (int j = 0; j < a->getHeight(); j++)
			removeDirtFromLocation(a->getX() + i, a->getY() + j);
}

bool StudentWorld::isThereSpaceForAnActorHere(CoordType x, CoordType y) const
{
	for (int i = 0; i < SPRITE_WIDTH; i++)
		for (int j = 0; j < SPRITE_HEIGHT; j++)
			if (isThereDirtAt(x + i, y + j))
				return false;
	return true;
}

bool StudentWorld::removeDirtFromLocation(const int& x, const int& y)
{
	Dirt* p = m_dirts[x][y];
	if (p == nullptr)
		return false;
	else
	{
		delete p;
		m_dirts[x][y] = nullptr;
		return true;
	}
}


const int MAX_ATTEMPTS = 200;
//places into x and y a possible location an actor of identity ID could be placed
bool StudentWorld::generateAppropriatePossibleLocation(int& x, int& y, const int& ID)
{
	int i, j;
	int numAttempts = 0; //if it's tried to place the Goodie over a threshold and is failing, then there's no space for it
	int x_offset_bottom, y_offset_bottom, x_offset_top, y_offset_top = 0; // 0<=x<=60, 20<=y<=56
	bool checkForDirtBelow = true; //for boulders, must be at a location that would otherwise be filled with Dirt

	//bool placementGrid[VIEW_WIDTH][VIEW_HEIGHT];

	//for (int i = 0; i < VIEW_WIDTH; i++)
	//	for (int j = 0; j < VIEW_HEIGHT; j++)
	//		placementGrid[i][j] = false; //false meaning no 'collisions' with anything else

	//for (int i = 0; i < m_actors.size(); i++)


	switch (ID)
	{
		//must be placed in dirt
	case IID_BOULDER:
	case IID_BARREL:
	case IID_GOLD:
		x_offset_bottom = 0;
		x_offset_top = 4; //(64-60) == 4
		y_offset_bottom = 20;
		y_offset_top = 8;// (64-56) == 8
	//case IID_SONAR:
		//if i and j reach the end, it's trying to place a Goodie at the top-right corner --> nothing worked
		do
		{
			numAttempts++; // don't want to try this forever!

			i = rand() % (VIEW_WIDTH - x_offset_bottom - x_offset_top) + x_offset_bottom + 1; //gives 0<=x<=60
			j = rand() % (VIEW_HEIGHT - y_offset_bottom - y_offset_top) + y_offset_bottom + 1; //gives 20<=y<=56 
													//is definitely within allowed coordinates stated in spec

			bool isOK = true;

			//has to not be in the oil shaft/mining surface, i.e., where there's no dirt
			if (m_dirts[i][j] == nullptr) //if there's no dirt there
				continue; //re-roll

			//make sure there is Dirt under the sprite
			for (int k = 0; k < SPRITE_WIDTH; k++)
				if (m_dirts[i + k][j - 1] == nullptr)
					isOK = false;

			if (!isOK) //if there isn't
				continue; //re-roll

										//these must be hidden in the dirt, so



			for (int k = 0; k < m_actors.size(); k++)
			{
				double dist = distance(m_actors[k]->getX(), m_actors[k]->getY(), i, j);
				if (dist <= DISTANCE_PLACEMENT)
				{
					isOK = false;
					break; //no need to keep checking if it already overlapped with someone's space
				}
			}

			if (isOK) //if isOK is still true, then it's a possible coordinate, so we can get out finally!
			{
				x = i;
				y = j;
				break;
			}

		} while (numAttempts < MAX_ATTEMPTS);
		break;
	}


	//if going randomly didn't work, brute-force it by checking every possible tile
	//TODO: implement brute-force method in case it's necessary

	return (numAttempts != MAX_ATTEMPTS); //if it didn't reach the max number of attempts, a spot was found
}

//isThereDirtInDirectionOfActor;

bool StudentWorld::isThereDirtInDirectionOfActor(const Actor* caller) const
{
	CoordType x, y;
	caller->sendLocation(x, y);

	return isThereDirtInDirection(caller->getDirection(), x, y, caller->getWidth(), caller->getHeight());

	//if (y == 0)
	//	return false; //there can't be dirt under it if it's at the bottom!



}


bool StudentWorld::isThereDirtInDirection(GraphObject::Direction dir, CoordType x, CoordType y, CoordType height, CoordType width) const
{

	switch (dir)
	{
	case GraphObject::down:
		if (y == 0)
			return false; //nothing below the bottom!
		for (int k = 0; k < width; k++)
			if (m_dirts[x + k][y - 1] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
	case GraphObject::up:
		if (y == Y_UPPER_BOUND)
			return false; //nothing above the top!
		for (int k = 0; k < width; k++)
			if (m_dirts[x + k][y + height] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
	case GraphObject::left:
		if (x == 0)
			return false; //nothing left of the left edge!
		for (int k = 0; k < height; k++)
			if (m_dirts[x - 1][y + k] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
	case GraphObject::right:
		if (x == X_UPPER_BOUND)
			return false; //nothing right of the right edge!
		for (int k = 0; k < height; k++)
			if (m_dirts[x + width][y + k] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
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
				&& j >= (SPRITE_HEIGHT)) //4
													 //don't fill in the "mine shaft" (30<=x<34) 0<=y<60 according to array locations)
			{
				m_dirts[i][j] = nullptr; //aka no dirt
			}
			//if I got here, it's safe to fill in some dirt!
			//create new Dirt object and put it in the appropriate spot in the m_dirts[][]
			else
			{
				m_dirts[i][j] = new Dirt(this, i, j);
			}
			
		}


	}
}

// For FrackMan class.
//x and y start at the topmost or leftmost square of dirt to be removed depending on the direction of movement:
// "up or down" --> x is leftmost
// "left or right" --> y is topmost
bool StudentWorld::removeDirtForFrackMan()
{
	CoordType x, y, dx = 0, dy = 0;

	m_player->sendLocation(x, y);

	switch (m_player->getDirection())
	{
	case GraphObject::left:
		if (x == 0)
			return false;
		dx = -1;
		break;
	case GraphObject::right:
		if (x == X_UPPER_BOUND)
			return false;
		dx = m_player->getWidth();
		break;
	case GraphObject::up:
		if (y == Y_UPPER_BOUND)
			return false;
		dy = m_player->getHeight();
		break;
	case GraphObject::down:
		if (y == 0)
			return false;
		dy = -1;
		break;
	}
	bool result = false;

	switch (m_player->getDirection())
	{
	case GraphObject::up:
	case GraphObject::down:
		for (int k = 0; k < m_player->getWidth(); k++)
			if (removeDirtFromLocation(x + dx + k, y + dy))
				result = true;
		break;
	case GraphObject::left:
	case GraphObject::right:
		for (int k = 0; k < m_player->getHeight(); k++)
			if (removeDirtFromLocation(x + dx, y + dy + k))
				result = true;
		break;

	}
	if (result)
		playSound(SOUND_DIG);

	return result;

	/*
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
	*/
}


// This will undoubtedly need to change in structure once I implement the protesters...
bool StudentWorld::attemptToInteractWithNearbyActors(const Actor* caller)
{
	int damage = 0;

	switch (caller->getIdentity())
	{
	case IID_WATER_SPURT:
		damage = -2;
		break;
	case IID_BOULDER:
		damage = -100;
		break;
		//case for Protesters with player?
	}

	bool interacted = false;

	//check against player
	if (caller->getIdentity() == IID_BOULDER && isActorAffectedByActor(caller, m_player, INTERACTED))
	{
		m_player->changeHealthBy(damage);
	}

	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		Group g = p->whatGroupAmI();
		if (g == enemies && isActorAffectedByActor(caller, p, DISTANCE_INTERACT))
		{
			p->changeHealthBy(damage);
			interacted = true;
			break;
		}

	}

	return interacted;
}




void StudentWorld::setDisplayText()
{
int score = GameWorld::getScore();
int level = GameWorld::getLevel();
int lives = GameWorld::getLives();
int health = m_player->getHealth();
int squirts = m_player->getSquirts();
int gold = m_player->getGold();
int sonar = m_player->getSonar();

int barrelsLeft = m_barrelsLeft;
// Next, create a string from your statistics, of the form:
// “Scr: 0321000 Lvl: 52 Lives: 3 Hlth: 80% Water: 20 Gld: 3 Sonar: 1 Oil Left: 2”
std::string s = formatDisplayText(score, level, lives,
health, squirts, gold, sonar, barrelsLeft);
// Finally, update the display text at the top of the screen with your
// newly created stats
setGameStatText(s); // calls our provided GameWorld::setGameStatText

}

// of course, right AFTER I finish with the display text,
//the writeup on ostringstream objects is posted...
std::string StudentWorld::formatDisplayText(int score, int level, int lives,
	int health, int squirts, int gold, int sonar, int barrelsLeft) const
{
	//d = 'display'
	std::string spacing = "  ";
	std::string d_score, d_level, d_lives, d_health, d_squirts, d_gold, d_sonar, d_barrelsLeft = "";


	d_score = "Scr: " + prependCharToStringToSize(to_string(score), '0', 6);
	d_level = "Lvl: " + prependCharToStringToSize(to_string(level), ' ', 2);
	d_lives = "Lives: " + prependCharToStringToSize(to_string(lives), ' ', 1);

	int hp = health / PLAYER_START_HEALTH * 100.00; //start health is max health
	//should convert double on right to (truncated) int on left.
	d_health = "Hlth: " + prependCharToStringToSize(to_string(hp), ' ', 3) + '%';

	d_squirts = "Wtr: " + prependCharToStringToSize(to_string(squirts), ' ', 2);
	d_gold = "Gld: " + prependCharToStringToSize(to_string(gold), ' ', 2);
	d_sonar = "Sonar: " + prependCharToStringToSize(to_string(sonar), ' ', 2);
	d_barrelsLeft = "Oil Left: " + prependCharToStringToSize(to_string(barrelsLeft), ' ', 2);

	vector<std::string> v = { d_score, d_level, d_lives, d_health, d_squirts, d_gold, d_sonar, d_barrelsLeft };

	std::string result = "";

	for (int i = 0; i < v.size(); i++)
	{
		result += v[i];
		if (i == v.size() - 1)
			break;
		//else add spacing
		result += spacing;
	}


	return result;
}

//PRECONDITON: caller has already checked that it's attempted to move in the same direction as it's facing
//POSTCONDITON: Moves the caller in the appropriate direction if possible, returning true. Else, returns false.
bool StudentWorld::tryToMoveMe(DynamicObject* caller, const GraphObject::Direction moveDir)
{
	CoordType x, y;
	caller->sendLocation(x, y);

	if (isInvalidLocation(x, y)) //if trying to go outside possible gridspace, bad
	{
		caller->moveTo(caller->getX(), caller->getY()); //make it 'move' in place for animation
		return false;
	}

	//bool transformed = caller->sendEffectiveLocation(x_t, y_t, moveDir); //causes problems when approaching from the left

	bool result = tryToMoveFromLocation(x, y, caller->getDirection());
		//return false;



	if(result)
		caller->moveTo(x, y);

	return result;
}

void StudentWorld::moveCoordsInDirection(CoordType& x, CoordType& y, GraphObject::Direction dir) const
{
	switch (dir)
	{
	case GraphObject::up:
		//x = getX();
		y += UP_DIR;
		break;
	case GraphObject::down:
		//x = getX();
		y += DOWN_DIR;
		break;
	case GraphObject::left:
		x += LEFT_DIR;
		//y = getY();
		break;
	case GraphObject::right:
		x += RIGHT_DIR;
		//y = getY();
		break;
	}
}


bool StudentWorld::tryToMoveFromLocation(CoordType& x, CoordType& y, GraphObject::Direction moveDir) const
{
	CoordType x_old = x, y_old = y;

	moveCoordsInDirection(x, y, moveDir);

	//if Actor's current location doesn't have it run into a boulder,
	//but moving it in the place he'd like to would change it
	if (!isLocationAffectedByGroup(x_old, y_old, boulders, INTERACTED) && isLocationAffectedByGroup(x, y, boulders, INTERACTED))
	{
		return false; //don't move, no animation either
	}

	//by this point, FrackMan will have dug through
	// and no other Actors can move through Dirt
	return (isThereSpaceForAnActorHere(x, y));
}

GraphObject::Direction StudentWorld::canITurnAndMove(const Actor* caller) const
{
	GraphObject::Direction cDir = caller->getDirection();
	CoordType x, y;
	caller->sendLocation(x, y);
	std::vector<GraphObject::Direction> pDirs;

	switch(cDir)
	{
	case GraphObject::left:
	case GraphObject::right:
		pDirs.push_back(GraphObject::up);
		pDirs.push_back(GraphObject::down);
		break;
	case GraphObject::up:
	case GraphObject::down:
		pDirs.push_back(GraphObject::left);
		pDirs.push_back(GraphObject::right);
		break;
	}

	std::vector<GraphObject::Direction>::iterator it = pDirs.begin();

	while (it != pDirs.end())
	{
		if (!StudentWorld::tryToMoveFromLocation(x, y, (*it)))
			it = pDirs.erase(it);
		else it++;
	}

	if (pDirs.size() == 0)
		return GraphObject::none; //can't turn
	else //otherwise, choose a random way to turn (or the only way to turn if there's only one way to turn)
		return (pDirs[rand() % pDirs.size()]);


}


// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

//a is always a dynamicObject 


//overlap compares the bottom-left corner of each sprite to determine overlap (should always be a valid comparison)
int StudentWorld::overlap(const Actor* a, const Actor* b) const
{
	//if the Actors are far away...
	CoordType xa, ya, xb, yb;

	a->putAtSpriteCorner(DEFAULT_CORNER, xa, ya);
	b->putAtSpriteCorner(DEFAULT_CORNER, xb, yb);

	double dist = distance(xa, ya, xb, yb);

	if (dist > (a->getMaxLength() + b->getMaxLength()))
		return EXISTS_GAP; //there must be a gap. Skip all the extra stuff below


	Corner relLoc = a->relativeLocationTo(b);

	if (relLoc == NA)
		return OVERLAPPING; //if at the same place, definitely overlapping

	//CoordType x_low, x_high, y_low, y_high;
	CoordType x, y;

	b->putAtSpriteCorner(relLoc, x, y);

	if (a->isInsideMySprite(x, y))
		return OVERLAPPING;

	bool atSide;

	//consider boundary cases of 'touching'
	switch (relLoc) //of a compared to b
	{
	case bottom_left:
						// b is directly to the right of a		 and		b is near a vertically
		atSide = ( ((a->getX() + a->getWidth()) == b->getX()) && ((b->getY() - a->getY()) < a->getHeight() ));
		break;
	case bottom_right:
		atSide = (((a->getX() - a->getWidth()) == b->getX()) && ((b->getY() - a->getY()) < a->getHeight()));
		break;
	case top_left:
		atSide = (((a->getY() + a->getHeight()) == b->getY()) && ((b->getX() - a->getX()) < a->getWidth()));
		break;
	case top_right:
		atSide = (((a->getY() + a->getHeight()) == b->getY()) && ((b->getX() - a->getX()) < a->getWidth()));
		break;
	}

	if (atSide)
		return TOUCHING;

	//if it's not overlapping or touching, there must be a gap!

	return EXISTS_GAP;

		/*
		//cases below do the best they can to make (x_low, y_low) be more toward the specified corner than (x_high, y_high)
		//	if x_this < x_other (i.e. this is "bottom"), this->putAtSpriteCorner(top____, x_low, ...)
		//						                  else   other->putAtSpriteCorner(top____, x_low, ...)
	case bottom_left:
		a->putAtSpriteCorner(top_right, x_low, y_low);
		b->putAtSpriteCorner(bottom_left, x_high, y_high);
		break;
	case top_left:
		a->putAtSpriteCorner(bottom_right, x_low, y_high);
		b->putAtSpriteCorner(top_left, x_high, y_low);
		break;
	case bottom_right:
		a->putAtSpriteCorner(top_left, x_high, y_low);
		b->putAtSpriteCorner(bottom_right, x_low, y_high);
		break;
	case top_right:
		a->putAtSpriteCorner(bottom_left, x_high, y_high);
		b->putAtSpriteCorner(top_right, x_low, y_low);
		break;
	}

	CoordType dx = x_high - x_low;
	CoordType dy = y_high - y_low;

	// "Touching" is when (x_high - x_low) == 1
	// will subtract 1 from dx and dy so we can base our logic around 0
	dx--;
	dy--;
	
	//if either dx or dy are less

	*/
}

double StudentWorld::distanceBetweenActors(const Actor* a, const Actor* b) const
{
	CoordType x1, x2, y1, y2;
	a->sendLocation(x1, y1);
	b->sendLocation(x2, y2);

	return distance(x1, y1, x2, y2);
}


double StudentWorld::distanceBetweenLocationAndActor(const CoordType& x, const CoordType& y, const Actor* b) const
{
	CoordType x1, x2, y1, y2;
	b->sendLocation(x2, y2);

	return distance(x, y, x2, y2);
}

void StudentWorld::letPlayerUseSonar()
{
	bool usedSonar = true;
	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		if (p->getDepth() == DEPTH_GOODIE && isLocationAffectedByGroup(p->getX(), p->getY(), player, DISCOVERED, usedSonar))
		{
			p->setVisibility(true);
		}
	}
	playSound(SOUND_SONAR);
	m_player->changeSonarBy(-1);
}

void StudentWorld::letPlayerDropGold()
{
	CoordType x, y;
	m_player->sendLocation(x, y);
	Gold* p = new Gold(x, y, this, SCORE_GOLD_PROTESTER, IID_GOLD);
	m_actors.push_back(p);
	m_player->changeGoldBy(-1);

}

void StudentWorld::letPlayerFireASquirt()
{
	CoordType x, y, dx = 0, dy = 0;
	m_player->sendLocation(x, y);

	switch (m_player->getDirection())
	{
	case GraphObject::left:
		dx = -SPRITE_WIDTH;
		break;
	case GraphObject::right:
		dx = SPRITE_WIDTH;
		break;
	case GraphObject::up:
		dy = SPRITE_HEIGHT;
		break;
	case GraphObject::down:
		dy = -SPRITE_HEIGHT;
		break;
	}


	Squirt* p = new Squirt(x+dx, y+dy, this, m_player->getDirection());
	m_actors.push_back(p);
	playSound(SOUND_PLAYER_SQUIRT);
	m_player->changeSquirtsBy(-1);
	return;

}


GraphObject::Direction StudentWorld::tellMeHowToGetToMyGoal(const Actor* caller, CoordType x_goal, CoordType y_goal) const
{
	CoordType x, y;
	caller->sendLocation(x, y);

	return HowToGetFromLocationToGoal(x, y, x_goal, y_goal);
}

#include <queue>
const char WALL = 'X';
const char BREADCRUMB = '#';
const char PATH = '.';
const char GOAL = '%';

GraphObject::Direction StudentWorld::HowToGetFromLocationToGoal(CoordType x_actor, CoordType y_actor, CoordType x_goal, CoordType y_goal) const
{
	return GraphObject::right;

	std::queue<Coord> s;
	
	char a[X_UPPER_BOUND][Y_UPPER_BOUND];

	//determine all the spots the Actor cannot go
	for (int i = 0; i < X_UPPER_BOUND; i++)
		for (int j = 0; j < Y_UPPER_BOUND; j++)
		{
			if (isThereDirtAt(i, j))
				a[i][j] = WALL;
			else if (isThereABoulderAt(i, j))
			{
				
				for (int k = -DISTANCE_INTERACT; k < DISTANCE_INTERACT; k++)
					for (int l = -DISTANCE_INTERACT; l < DISTANCE_INTERACT; l++)
						if (distance(i, j, i + k, j + l) <= DISTANCE_INTERACT)
							a[i + k][j + l] = WALL;
			}
			else a[i][j] = PATH;
		}

	//to obtain Direction, will have to work backwards, from Goal to Actor
	// When they finally match, return the opposite direction of that used to reach the Actor
	Coord c(x_goal, y_goal);
	a[x_actor][y_actor] = GOAL;

	CoordType x = c.m_x, y = c.m_y;


	if (c.m_x == x_actor && c.m_y == y_actor) //check to see if we're already there
		return GraphObject::none;

	s.push(c);

	while (!s.empty())
	{

		c = s.front();
		s.pop();
		x = c.m_x, y = c.m_y;
		a[x][y] = BREADCRUMB;

		if (a[x + LEFT_DIR][y] == GOAL) //if the 'goal' Actor would have to move left to get to the 'caller' Actor
			return GraphObject::right; //the caller should move the opposite of left, i.e., right
		else if (a[x + LEFT_DIR][y] == PATH) //otherwise, if I can move there
		{
			Coord d(x + LEFT_DIR, y); //push the Coordinate to the queue
			s.push(d);
		}
		//repeat thought process for all four cardinal directions
		if (a[x][y + DOWN_DIR] == GOAL)
			return GraphObject::up;
		else if (a[x][y + DOWN_DIR] == PATH)
		{
			Coord d(x, y + DOWN_DIR);
			s.push(d);
		}

		if (a[x + RIGHT_DIR][y] == GOAL)
			return GraphObject::left;
		else if (a[x + RIGHT_DIR][y] == PATH)
		{
			Coord d(x + RIGHT_DIR, y);
			s.push(d);
		}

		if (a[x][y + UP_DIR] == GOAL)
			return GraphObject::down;
		else if (a[x][y + UP_DIR] == PATH)
		{
			Coord d(x, y + UP_DIR);
			s.push(d);
		}

	}

	//if made it here and didn't return already, can't make it back
	//(which is worrisome, since there should always be a way back...)
	return GraphObject::none;

}


bool StudentWorld::isThereABoulderAt(CoordType x, CoordType y) const
{
	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		if (p->whatGroupAmI() == boulders)
		{
			CoordType a, b;
			p->sendLocation(a, b);
			if (x == a && y == b)
				return true;
		}
	}
	return false;
}


bool StudentWorld::amIFacingFrackMan(const Actor* caller) const
{
	GraphObject::Direction c_dir = caller->getDirection();
	CoordType x_c, y_c, x_p, y_p;
	caller->sendLocation(x_c, y_c);
	m_player->sendLocation(x_p, y_p);

	int dx = 4, dy = 4; //the 'cone' of vision will be a rectangle of coordinate +- d_coord

	bool result = false;

	switch (c_dir)
	{
	case GraphObject::right:
		//return whether, it's in the above rectangle of (x_p > x_c, y_p-dy <= y_c <= y_p + dy)
		return (x_p >= x_c && (y_p - dy) <= y_c && y_c <= (y_p + dy));
	case GraphObject::left:
		return (x_p =< x_c && (y_p - dy) <= y_c && y_c <= (y_p + dy));
	case GraphObject::down:
		return ((x_p - dx) <= x_c && x_c <= (x_p + dx) && y_p <= y_c);
	case GraphObject::up:
		return ((x_p - dx) <= x_c && x_c <= (x_p + dx) && y_p >= y_c);

	case GraphObject::none:
		exit(5); //bad...


	}


}

//if in the same vertical/horizontal line and there's no dirt in the way, can see the FrackMan
//if so, return Direction from caller to FrackMan
//else return GraphObject::none
GraphObject::Direction StudentWorld::directLineToFrackMan(const Actor* caller) const
{
	CoordType x_c, y_c, x_p, y_p;
	caller->sendLocation(x_c, y_c);
	m_player->sendLocation(x_p, y_p);

	if (!(x_c == x_p || y_c == y_p)) //then no direct line of sight for sure
		return GraphObject::none;

	if (x_c == x_p) //on the same vertical line
	{
		for (int j = min(y_c, y_p); j < max(y_c, y_p); j++)
		{
			if (!isThereSpaceForAnActorHere(x_c, j) || isLocationAffectedByGroup(x_c, j, boulders, INTERACTED))
			{
				return GraphObject::none;
			}
		}
		//if made it here, could make it to FrackMan
		//determine direction
		if (y_c < y_p)
			return GraphObject::up;
		else return GraphObject::down;
	}

	if (y_c == y_p) //on the same horizontal line
	{
		for (int i = min(x_c, x_p); i < max(x_c, x_p); i++)
		{
			if (!isThereSpaceForAnActorHere(i, y_c) || isLocationAffectedByGroup(i, y_c, boulders, INTERACTED))
			{
				return GraphObject::none;
			}
		}
		//if made it here, could make it to FrackMan
		//determine direction
		if (x_c < x_p)
			return GraphObject::right;
		else return GraphObject::left;
	}


}



// Helper functions

double distance(int x1, int y1, int x2, int y2)
{
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

std::string prependCharToStringToSize(std::string s, char c, int size)
{
	while (s.size() < size)
		s = c + s;

	if (s.size() != size) //truncate to take last 'size' chars
		s = s.substr(s.size() - size); //still miss Python...

	return s;
}


double angleBetweenTwoPoints(int x1, int y1, int x2, int y2)
{
	return (tan(double(x2 - x1) / (y2 - y1)));
}



int min(int x, int y)
{
	if (x < y)
		return x;
	else return y;
}
int max(int x, int y)
{
	if (x > y)
		return x;
	else return y;
}