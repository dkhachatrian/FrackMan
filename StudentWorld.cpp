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
	
	//place boulders
	int i = 0;
	while (i < m_bouldersLeft)
	{
		bool foundSpot = generateAppropriatePossibleLocation(x, y, IID_BOULDER);

		if(foundSpot)
		{ 
			Boulder* p = new Boulder(x, y, this, IID_BOULDER, DEPTH_BOULDER);
			p->moveTo(x, y);
			removeDirtForActor(p);
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
	//setAllActorsAsVisible();


	return 1; //change later
}

int StudentWorld::move()
{
	// Update the Game Status Line
	setDisplayText();// update the score/lives/level text at screen top
										   // The term �Actors� refers to all Protesters, the player, Goodies,
										   // Boulders, Barrels of oil, Holes, Squirts, the Exit, etc.
										   // Give each Actor a chance to do something
	m_player->doSomething();
	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		if(!p->isDead())
		{
			// ask each actor to do something (e.g. move)
			p->doSomething();
		}
	}

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

	// the player hasn�t completed the current level and hasn�t died
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

	bool canPlace = generateAppropriatePossibleLocation(x, y, a->getID());

	if (!canPlace)
		return false; //can't place the item! Which might be worrisome...

	//interested in placing (upon init)


	//first will deal with Boulders
	//bool canPlace = true;

	//remove dirt if it's a boulder
	if (a->getID() == IID_BOULDER)
	{

	}

	//place Actor in location
	a->moveTo(x, y);

	//update vector of Actors
	m_actors.push_back(a);

	//and get out
	return true;
}



void StudentWorld::removeDirtForActor(const Actor* a)
{
	for (int i = 0; i < a->getWidth(); i++)
		for (int j = 0; j < a->getHeight(); j++)
			removeDirtFromLocation(a->getX() + i, a->getY() + j);
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


	//if (y == 0)
	//	return false; //there can't be dirt under it if it's at the bottom!

	switch (caller->getDirection())
	{
	case GraphObject::down:
		if (y == 0)
			return false; //nothing below the bottom!
		for (int k = 0; k < caller->getWidth(); k++)
			if (m_dirts[x + k][y - 1] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
	case GraphObject::up:
		if (y == Y_UPPER_BOUND)
			return false; //nothing above the top!
		for (int k = 0; k < caller->getWidth(); k++)
			if (m_dirts[x + k][y + caller->getHeight()] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
	case GraphObject::left:
		if (x == 0)
			return false; //nothing left of the left edge!
		for (int k = 0; k < caller->getHeight(); k++)
			if (m_dirts[x - 1][y + k] != nullptr)
				return true;
		//if it made it here, all of the ones underneath are nullptr --> no dirt underneath
		return false;
	case GraphObject::right:
		if (x == X_UPPER_BOUND)
			return false; //nothing right of the right edge!
		for (int k = 0; k < caller->getHeight(); k++)
			if (m_dirts[x + caller->getWidth()][y + k] != nullptr)
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
bool StudentWorld::removeDirtForFrackMan(const GraphObject::Direction dir, const CoordType& x, const CoordType& y)
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
// �Scr: 0321000 Lvl: 52 Lives: 3 Hlth: 80% Water: 20 Gld: 3 Sonar: 1 Oil Left: 2�
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
	CoordType x_t, y_t;
	caller->sendLocation(x_t, y_t);


	//bool transformed = caller->sendEffectiveLocation(x_t, y_t, moveDir); //causes problems when approaching from the left

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
	{
		caller->moveTo(caller->getX(), caller->getY()); //make it 'move' in place for animation
		return false;
	}
		//return false;

	//if Actor's current location doesn't have it run into a boulder,
	//but moving it in the place he'd like to would change it
	if (!isActorAffectedByGroup(caller, boulders, INTERACTED) && isLocationAffectedByGroup(x_t, y_t, boulders, INTERACTED))
	{
		return false; //don't move, no animation either
	}

	//std::vector<Actor*>::iterator it = m_actors.begin();
	/*
	//check over all Actors
	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		if (overlap(caller, p) == TOUCHING)
		{
			if (p->isSolid()) //if the thing there is solid (i.e. Boulder), bad
				return false; //does NOT 'move' the caller (not supposed to animate!)
		}

	}
	*/
	//otherwise, we should be good to go!
	//update (x,y) and get out

	//not transforming anymore, so don't need this
	//if(transformed)
	//	caller->reverseTransform(x_t, y_t, moveDir); //fixes the possible translation done by sendToEffectiveLocation()
	//x_t or y_t has also already been moved by one square according to moveDir
	//and by this point we know it's a valid move, so we're good to update the caller's location!

	caller->moveTo(x_t, y_t);

	return true;
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

/*
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


	switch(g)
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
			if (!(p->getID() == IID_PROTESTER || p->getID() == IID_HARD_CORE_PROTESTER))
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
			if (!(p->getID() == IID_BOULDER))
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
*/
//NOTE: incomplete
bool StudentWorld::isLocationAffectedByGroup(const CoordType& x, const CoordType& y, Group g, const int& statusOfInterest) const
{
	double distanceOfInterest = -1;

	switch (statusOfInterest)
	{
	case DISCOVERED:
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
			if (!(p->getID() == IID_PROTESTER || p->getID() == IID_HARD_CORE_PROTESTER))
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
			if (!(p->getID() == IID_BOULDER))
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


void StudentWorld::letPlayerUseSonar()
{
	bool usedSonar = true;
	for (int i = 0; i < m_actors.size(); i++)
	{
		Actor* p = m_actors[i];
		if (p->getDepth() == DEPTH_GOODIE && isActorAffectedByGroup(p, player, DISCOVERED, usedSonar))
		{
			p->setVisibility(true);
		}
	}
	playSound(SOUND_SONAR);
	m_player->changeSonarBy(-1);
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