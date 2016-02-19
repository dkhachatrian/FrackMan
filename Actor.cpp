#include "Actor.h"
#include "StudentWorld.h"

const int TOUCHING = 0;
const int OVERLAPPING = -1;
const int EXISTS_GAP = 1;


int FrackMan::doSomething()
{
	int input = 0;
	bool result;



	if (Actor::getWorld()->getKey(input)) //hit a key in the tick!
	{
		//MOVEMENT ACTIONS
		switch (input)
		{
			// possibly move in a direction
		case KEY_PRESS_DOWN:
			result = attemptMove(down);
			break;
		case KEY_PRESS_LEFT:
			result = attemptMove(left);
			break;
		case KEY_PRESS_RIGHT:
			result = attemptMove(right);
			break;
		case KEY_PRESS_UP:
			result = attemptMove(up);
			break;
		}
	}

	return 0; //change later
}



/*
//reads keyboard input (for FrackMan only!)
//calls Actor::setDirection with appropriate parameter
// Returns true if direction was changed, false if not
bool DynamicObject::setDir(Direction dir)
{

	switch (dir)
	{
	case KEY_PRESS_LEFT:
		Actor::setDirection(left);
		break;
	case KEY_PRESS_DOWN:
		Actor::setDirection(down);
		break;
	case KEY_PRESS_RIGHT:
		Actor::setDirection(right);
		break;
	case KEY_PRESS_UP:
		Actor::setDirection(up);
		break;
	default: //shouldn't change
		return false;
	}
	
	return true;
}
*/

bool FrackMan::attemptMove(const Direction dir)
{
	int x_n = getX();
	int y_n = getY();

	
	//make "effective" x,y coordinates due to sprites
	//(i.e., shift y up SPRITE_HEIGHT squares if trying to go up
	//			or x right SPRITE_WIDTH squares if trying to go right)

	bool transformed = getEffectiveLocation(x_n, y_n, dir);

	bool a = !moveMatchesDir(dir);
	bool b = !canMoveInDirection(dir, x_n, y_n);

	if (a || b)
	{
		GraphObject::setDirection(dir);
		//fix the affected coordinates
		return false; //didn't move
	}
	//otherwise
	bool removedDirt = removeDirt(dir, x_n, y_n); //if FrackMan removes any Dirt, he will not move that tick
	
	if (removedDirt) //means dirt was removed
	{
		getWorld()->playSound(SOUND_DIG);
		//return false;
	}
	
	//fix the affected coordinates if necessary
	if (transformed)
		reverseTransform(x_n, y_n, dir);

	//alright
	moveTo(x_n, y_n);
	return true;
	


	//return false; //for now
}

//x and y start at the topmost or leftmost square of dirt to be removed depending on the direction of movement:
// "up or down" --> x is leftmost
// "left or right" --> y is topmost
bool FrackMan::removeDirt(const Direction dir, const int& x, const int& y)
{
	bool result = false;

	int x_eff = x;
	int y_eff = y;

	//currently glitching at some points...

	switch (dir)
	{
	case up:
		//int x_eff = x + SPRITE_WIDTH; //the coordinates are mapped to bottom-left corner of sprite
	case down:
		for (int i = 0; i < SPRITE_WIDTH; i++) //make room for sprite's width
		{
			Dirt* p = getWorld()->getDirts()[x_eff+i][y_eff];
			if(p != nullptr) //if there's Dirt in the way
			{
				result = true; //report that to FrackMan call
				delete p; //delete old Object
				getWorld()->getDirts()[x_eff+i][y_eff] = nullptr; //show that it's empty (CANNOT USE P ANYMORE)
			}
		}
		break;
	case left:
		//int y_eff = y + SPRITE_HEIGHT; //the coordinates are mapped to bottom-left corner of sprite
	case right:
		for (int j = 0; j < SPRITE_HEIGHT; j++) //make room for sprite's height
		{
			Dirt*** d = getWorld()->getDirts();
			Dirt* p = d[x_eff][y_eff + j];
			if (p != nullptr) //if there's Dirt in the way
			{
				result = true; //report that to FrackMan call
				delete p; //delete old Object
				getWorld()->getDirts()[x_eff][y_eff + j] = nullptr; //show that it's empty (CANNOT USE P ANYMORE)
			}
		}
		break;
	}

	return result;

}



bool DynamicObject::canMoveInDirection(const Direction moveDir, int& x, int& y)
{

	int x_t = x;
	int y_t = y;

	switch (moveDir)
	{
	case up:
		//x = getX();
		y_t = y + UP_DIR;
		break;
	case down:
		//x = getX();
		y_t = y + DOWN_DIR;
		break;
	case left:
		x_t = x + LEFT_DIR;
		//y = getY();
		break;
	case right:
		x_t = x + RIGHT_DIR;
		//y = getY();
		break;
	}

	if (isInvalidLocation(x_t, y_t)) //if trying to go outside possible gridspace, bad
		return false;
	
	std::vector<Actor*>::iterator it = getWorld()->getActors()->begin();

	//check over all Actors
	while (it != getWorld()->getActors()->end())
	{
		if (overlap(this, (*it)) == TOUCHING) //see if there's one where the Actor is trying to go
		{
			if ((*it)->isSolid()) //if the thing there is solid (i.e. Boulder), bad
				return false;
		}
		it++;
	}

	//otherwise, we should be good to go!
	//update (x,y) and get out
	x = x_t;
	y = y_t;

	return true;
}


// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp




// Helper functions

//a is always a dynamicObject 
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