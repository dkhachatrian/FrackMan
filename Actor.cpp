#include "Actor.h"
#include "StudentWorld.h"


//int max(int x, int y);
// Actor functions



//int Actor::doSomething()
//{
//	if (isDead())
//		return -1;
//}

bool Actor::isThereDirtNextToMe() const
{
	return getWorld()->isThereDirtInDirectionOfActor(this);
}


//Gives "effective" (x,y) based on sprite size and current direction
//Returns true if the input parameters x or y are different from the values given by getX/Y.
bool Actor::sendEffectiveLocation(CoordType& x, CoordType& y, const Direction dir) const
{
	//sendLocation(x, y);

	switch (dir)
	{
	case up:
		putAtSpriteCorner(top_left, x, y);
		break;
		//y += (getHeight() - 1); //looking for overlap of sprites so need to fix the offset between "location" and sprite
								  // (SPRITE_DIMENSION - 1) puts the coordinate on the other edge of the sprite
		//return true;
	case right:
		putAtSpriteCorner(bottom_right, x, y);
		break;
		//return true;
	default:
		putAtSpriteCorner(DEFAULT_CORNER, x, y);
		return false;
	}

	return (x != getX() || y != getY()); //aka if 'up' or 'right' were selected
}

// Reverses the actions of sendToEffectiveLocation on the parameters x and y
void Actor::reverseTransform(CoordType& x, CoordType& y, const Direction dir) const
{
	switch (dir)
	{
	case up:
		y -= (getHeight() - 1); //looking for overlap of sprites so need to fix the offset between "location" and sprite
								  // (SPRITE_DIMENSION - 1) puts the coordinate on the other edge of the sprite
		return;
	case right:
		x -= (getWidth() - 1);
		return;
	}
}

// NOTE: does *not* move the actual location of the Actor using moveTo()!
//	Given a corner, places x and y to the corresponding coordinates.
void Actor::putAtSpriteCorner(Corner c, CoordType& x, CoordType& y) const
{
	int x_n = this->getX();
	int y_n = this->getY();

	switch (c)
	{
	case bottom_left:
		break;
		//looking for overlap of sprites so need to fix the offset between "location" and sprite
		// (SPRITE_DIMENSION - 1) puts the coordinate on the other edge of the sprite
	case bottom_right:
		x_n += (getWidth() - 1);
		break;
	case top_left:
		y_n += (getHeight() - 1);
		break;
	case top_right:
		x_n += (getWidth() - 1);
		y_n += (getHeight() - 1);
		break;
	}

	x = x_n;
	y = y_n;

	return;
}

// Returns whether a coordinate is in an Actor's "hitbox"
bool Actor::isInsideMySprite(const CoordType& x, const CoordType& y) const
{
	int m_x = getX();
	int m_y = getY();

	for (int i = 0; i < getWidth(); i++)
		for (int j = 0; j < getHeight(); j++)
		{
			if (x == (m_x + i) && y == (m_y + j))
				return true;
		}

	return false;
}



// Since all Actors have square sprites, super simple
double Actor::getMaxLength() const
{
	return (getWidth() * sqrt(2));
}

//if compares the lower-left corners of two Actors
//if exactly the same coordinates, returns 'none'
//else, returns relative location of 'this' compared to 'other', with bias toward bottom and left
//	(e.g. this(2,7) and other(3,1) returns bottom_right
// (e.g. this(2,7) and other(2,1) returns top_left

Corner Actor::relativeLocationTo(const Actor* other) const
{
	CoordType x1, x2, y1, y2;

	this->putAtSpriteCorner(DEFAULT_CORNER, x1, y1);
	other->putAtSpriteCorner(DEFAULT_CORNER, x2, y2);

	if (x1 == x2 && y1 == y2)
		return NA;

	if (x1 > x2)
	{
		if (y1 > y2)
			return top_right;
		else return bottom_right;
	}
	else
	{
		if (y1 > y2)
			return top_left;
		else return bottom_left;
	}

}






// DynamicObject functions


bool DynamicObject::attemptMove(const Direction dir)
{
	//CoordType x_n, y_n;

	//sendLocation(x_n, y_n);


	if (!moveMatchesDir(dir))
	{
		setDir(dir);
		moveTo(getX(), getY()); //"move" with no change in coordinates to have animation play
		return false; //didn't move
	}
	//otherwise, have the world move the DynamicObject if it can

	return (getWorld()->tryToMoveMe(this, dir));
}


// Squirt functions

Squirt::Squirt(CoordType x, CoordType y, StudentWorld* sw, Direction dir) :DynamicObject(IID_WATER_SPURT, DEPTH_SQUIRT, sw, x, y)
{
	setVisibility(true);
	setDir(dir);
	setTickStatus(true);
	setTickNumber(4);
}

int Squirt::doSomething()
{
	if (isDead())
		return DEAD;
	
	

	countDownATick();
	if (getTickNumber() == 0)
	{
		die();
		return DEAD;
	}

	bool result = getWorld()->attemptToInteractWithNearbyActors(this);

	if (result)
	{
		die();
		return DEAD;
	}

	//otherwise, keep on trucking
	result = DynamicObject::attemptMove(getDirection());
	if (!result)
	{
		die();
		return DEAD; //dies if it hits a wall, dirt, or boulder
	}
	//otherwise, it somehow made it through the gauntlet of checks
	return MOVED;

}



// FrackMan functions



int FrackMan::doSomething()
{
	int input = 0;
	bool result;

	if (didIDie())
	{
		die();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		return DEAD;
	}



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
		case KEY_PRESS_ESCAPE:
			die();
			break;
		case KEY_PRESS_SPACE:
			attemptToUseWaterGun();
			break;
		case KEY_PRESS_TAB:
			attemptToDropGold();
			break;
		case 'z':
		case 'Z':
			attemptToUseSonar();
			break;

		}
	}

	return 0; //change later
}

bool FrackMan::attemptMove(const Direction dir)
{
	//try to dig if in the same direction
	if (moveMatchesDir(dir))
		attemptToDig();

	bool result = DynamicObject::attemptMove(dir);

	return result;

}

//FrackMan's specialized action are many, depending on what key is pressed.
//	but at the moment, only thing implemented is for movement (i.e. removing dirt in front of it)
bool FrackMan::attemptToDig()
{
	bool result = false;
	if (isThereDirtNextToMe())
		result = getWorld()->removeDirtForFrackMan();

	return result;
	/*
	int x_n = getX();
	int y_n = getY();
	Direction dir = getDirection();

	sendEffectiveLocation(x_n, y_n, dir);

	bool removedDirt = getWorld()->removeDirtForFrackMan(dir, x_n, y_n);

	if (removedDirt) //means dirt was removed
	{
		getWorld()->playSound(SOUND_DIG);
		//return false;
	}
	return removedDirt;
	*/
}

bool FrackMan::attemptToUseSonar()
{
	if (m_sonar == 0)
		return false;
	//otherwise, can drop

	getWorld()->letPlayerUseSonar();
	return true;
}

bool FrackMan::attemptToDropGold()
{
	if (m_gold == 0)
		return false;

	getWorld()->letPlayerDropGold();
	return true;
}

bool FrackMan::attemptToUseWaterGun()
{
	if (m_squirts == 0)
		return false;

	getWorld()->letPlayerFireASquirt();
	return true;

}



// Protester functions

int Protester::doSomething()
{
	if (isDead())
		return DEAD;

	Direction dir = none;

	switch (m_pState)
	{
	case leaving:
		if (getX() == X_UPPER_BOUND && getY() == Y_UPPER_BOUND)
		{
			die();
			return DEAD;
		}
		//otherwise, head over there
		dir = getWorld()->tellMeHowToGetToMyGoal(this, X_UPPER_BOUND, Y_UPPER_BOUND);
		attemptMove(dir);
		return MOVED;
		break;
	case resting:
		setProtesterState(OK);
		return STATIONARY;
		//break;
	case coolingDown:
		m_currentTick++;
		if (m_currentTick % m_coolDownPeriod == 0)
		{
			setProtesterState(OK);
		}
		return STATIONARY;
		break;
	case OK:
		



		//start resting next tick
		m_currentTick++;
		if (m_currentTick % m_restTicks == (m_restTicks - 1))
		{
			m_currentTick = 0;
			setProtesterState(resting);
		}

		//do some stuff

		break;
	}

}














// Boulder functions

Boulder::Boulder(CoordType x, CoordType y, StudentWorld* sw, int IID = IID_BOULDER, unsigned int depth = DEPTH_BOULDER) :
	DynamicObject(IID, depth, sw, x, y)
{
	//moveTo(x, y);

	m_state = stable;
	m_haveWaited = false;
	setDir(down);
	setVisibility(true);
	setSolidityAs(true);
	setGroupAs(boulders);
}

int Boulder::doSomething()
{
	if (isDead())
		return DEAD;

	switch (m_state)
	{
		//progression of states is only stable -> waiting -> falling (-> death)
	case stable:
		if (!isThereDirtNextToMe())
		{
			m_state = waiting;
			m_haveWaited = true;
			setTickNumber(30);
		}
		return STATIONARY;
		break; //not necessary
	case waiting:
		countDownATick();
		if (getTickNumber() == 0)
		{
			m_state = falling;
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
		return STATIONARY;
		break; //not necessary
	case falling:
		if (isThereDirtNextToMe()) //before moving down through the dirt, check if there is any dirt
		{
			die();
			return DEAD;
		}
		bool couldMove = attemptMove(down);
		if (!couldMove) //hit the bottom of the stage
		{
			die();
			return DEAD;
		}
		if(getWorld()->isLocationAffectedByGroup(getX(), getY(), player, INTERACTED))
		{
			getWorld()->attemptToInteractWithNearbyActors(this);
			return MOVED; //this will finish the round
		}
		if (getWorld()->isLocationAffectedByGroup(getX(), getY(), enemies, INTERACTED))
		{
			//make them annoyed by 100 points
		}
		return MOVED;
	}


}







// Goodie functions


// NOTE: DOES NOT PLAY A SOUND
int Goodie::doSomething()
{
	if (isDead())
	{
		//die();
		return DEAD;
	}

	if (doITick())
	{
		countDownATick();

		if (getTickNumber() == 0)
		{
			die();
			return DEAD;
		}
	}

	if (getWorld()->isLocationAffectedByGroup(getX(), getY(), whoCanPickMeUp(), INTERACTED))
	{
		//getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(giveScore());
		die();
		return INTERACTED;
	}

	if (!isVisible() && (getWorld()->isLocationAffectedByGroup(getX(), getY(), whoCanPickMeUp(), DISCOVERED)))
	{
		setVisibility(true);
		return DISCOVERED;
	}

	//if nothing happened, stays stationary

	return STATIONARY;
}










// Sonar Functions

Sonar::Sonar(CoordType x, CoordType y, StudentWorld * sw):
	Goodie(IID_SONAR, sw, x, y, SCORE_SONAR)
{
	moveTo(x, y);
	setDir(right);
	setVisible(true);
	setVisibleFlag(true);
	setTickStatus(true);
	//setPickUpGroup(player);
	setTickNumber(max(100, 10 * getWorld()->getLevel())); //it doesn't see the function in StudentWorld.h?

	//for testing
	//setTickNumber(5);
}


int Sonar::doSomething()
{
	int result = Goodie::doSomething(); //does most of the stuff,
	//including making a sound or killing the Goodie or becoming visible if appropriate

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case INTERACTED:
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->changeSonarBy(1);
		break;
	}

	return result;
}







// Barrel functions

Barrel::Barrel(CoordType x, CoordType y, StudentWorld * sw, int score = SCORE_BARREL, int IID = IID_BARREL) :
	Goodie(IID, sw, x, y, score)
{
	moveTo(x, y);
	setVisibility(false);
	//setVisible(true);
	setDir(right);
}

int Barrel::doSomething()
{
	int result = Goodie::doSomething(); //does most of the stuff,
										//including making a sound or killing the Goodie or becoming visible if appropriate

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case INTERACTED:
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->changeBarrelsLeftBy(-1);
		break;
	}

	return result;
}



// Gold functions

Gold::Gold(CoordType x, CoordType y, StudentWorld * sw, int score = SCORE_GOLD_FRACKMAN, int IID = IID_GOLD) :
	Goodie(IID, sw, x, y)
{
	moveTo(x, y);
	setDir(right);

	//determine visibility and type of gold
	Actor* f = getWorld()->getPlayer();
	if (f->getX() == x && f->getY() == y) //if FrackMan is initially where the gold is constructed, he dropped it
	{
		m_frack = true;
		setTickStatus(true);
		setTickNumber(100);
		setVisibility(true);
		setPickUpGroup(enemies);
		setScore(SCORE_GOLD_PROTESTER);
	}
	else
	{
		m_frack = false;
		setTickStatus(false);
		setVisibility(false);
		setPickUpGroup(player);
		setScore(SCORE_GOLD_FRACKMAN);
	}

}

// NOT FINISHED. Return after implementing/designing Protester classes
int Gold::doSomething()
{
	int result = Goodie::doSomething();

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case INTERACTED:
		if (!wasDroppedByFrackMan())
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->getPlayer()->changeGoldBy(1);
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
			//tell the protestor he got bribed
		}
		break;
	}
}




// Water functions

Water::Water(CoordType x, CoordType y, StudentWorld* sw) :
	Goodie(IID_WATER_POOL, sw, x, y, SCORE_WATER_POOL)
{
	moveTo(x, y);
	setDir(right);
	setVisibility(true);
	setTickStatus(true);
	setTickNumber(max(100, 300 - (10 * getWorld()->getLevel())));
	//for testing
	//setTickNumber(50);
}

int Water::doSomething()
{
	int result = Goodie::doSomething(); //does most of the stuff,
										//including making a sound or killing the Goodie or becoming visible if appropriate

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case INTERACTED:
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->changeSquirtsBy(5);
		break;
	}

	return result;

}