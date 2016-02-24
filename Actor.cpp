#include "Actor.h"
#include "StudentWorld.h"


//int max(int x, int y);
// Actor functions



//int Actor::doSomething()
//{
//	if (isDead())
//		return -1;
//}

int Actor::doSomething()
{
	if (isDead())
		return DEAD;

	attemptToInteractWithActors(); //where all the interacting takes place

	if (doITick())
	{
		countDownATick();

		if (getTickNumber() == 0)
		{
			//die();
			performTickAction();
			return DEAD;
		}
	}

	return 42; //not useful at the moment

}


void Actor::performTickAction()
{
	die();
}


void Actor::attemptToInteractWithActors()
{
	getWorld()->attemptToInteractWithNearbyActors(this);
}

void Actor::interactWithActor(const Actor* other, double distanceOfInteraction)
{
	switch (other->whatGroupAmI())
	{
	case player:
		respondToPlayer(distanceOfInteraction);
		break;
	case enemies:
		respondToEnemy(distanceOfInteraction);
		break;
	case boulders:
		respondToBoulder(distanceOfInteraction);
		break;
	case goodies:
		respondToGoodie(distanceOfInteraction);
		break;
	case bribes:
		respondToBribe(distanceOfInteraction);
	default:
		break;
	}




}




bool Actor::isThereDirtNextToMeInDirection(Direction dir) const
{
	return getWorld()->isThereDirtInDirectionOfActor(this, dir);
}

bool Actor::isThereDirtNextToMeInCurrentDirection() const
{
	return getWorld()->isThereDirtInDirectionOfActor(this, getDirection());

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
	setGroupAs(squirts);
	setTickNumber(4);
}

int Squirt::doSomething()
{
	Actor::doSomething();

	//check to see if I died after interacting with other Actors
	if (isDead())
		return DEAD;


	countDownATick();
	if (getTickNumber() == 0)
	{
		die();
		return DEAD;
	}

	//otherwise, keep on trucking
	bool couldMove = DynamicObject::attemptMove(getDirection());
	if (!couldMove)
	{
		die();
		return DEAD; //dies if it hits a wall, dirt, or boulder
	}
	//otherwise, it somehow made it through the gauntlet of checks
	return MOVED;

}


void Squirt::respondToEnemy(double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
		die(); //enemy class will deal with damage and sound effect
}
void Squirt::respondToBoulder(double distanceOfInteraction)
{
	respondToEnemy(distanceOfInteraction); //does the same thing in this case!
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
	if (isThereDirtNextToMeInCurrentDirection()) //have already checked that movement matches direction in FrackMan::attemptMove
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

// have to deal with several ticks -- cannot rely on Actor::doSomething() to handle the tick (can handle asking the StudentWorld to interact though)
int Protester::doSomething()
{
	if (isDead())
		return DEAD;

	if (didIDie()) //maybe from Boulder or somethings
		m_currentRestTick = 0;


	if (amIAnnoyed())
	{
		if (m_annoyedRestTick < max(50, 100 - getWorld()->getLevel() * 10))
		{
			m_annoyedRestTick++;
			return STATIONARY;
		}
		else
		{
			m_annoyedRestTick = 0;
			setAnnoyed(false);
		}
	}

	Direction dir = none;

	//determine if Protester will rest this turn, or get closer to resting
	if (m_currentRestTick % m_restTicks == (m_restTicks - 1))
	{
		m_currentRestTick = 0;
		return STATIONARY;
	}
	else m_currentRestTick++;

	// Gold always acts on Protester, even as he's leaving





	if (getProtesterState() != resting)
	{
		m_currentNonrestTick++;
		//m_currentRestTick++;
	}

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
		if (didIDie())
			setProtesterState(leaving);
		else
			setProtesterState(OK);
		return STATIONARY;
		//break;
	case coolingDown:
		m_currentCoolDownTick++;
		if (m_currentCoolDownTick % m_coolDownPeriod == 0)
		{
			setProtesterState(OK);
			m_currentCoolDownTick = 0;
		}
		return STATIONARY;
		break;
	case OK:

		//attempt to yell at FrackMan

		//conveniently, DISCOVERED == 4 == shouting distance of Protester
		if (getWorld()->isActorAffectedByGroup(this, player, DISCOVERED) && getWorld()->amIFacingFrackMan(this))
		{
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->getPlayer()->changeHealthBy(-2);
			setProtesterState(coolingDown);
			return STATIONARY;
		}

		//attempt to walk toward FrackMan intelligently (Direction and ability to see FrackMan being determined by the tryToGetToFrackMan() function)
		if (!getWorld()->isActorAffectedByGroup(this, player, DISCOVERED))
		{
			Direction dir = tryToGetToFrackMan();

			if (dir != none)
			{
				m_numTimesCurrentDir = 0;
				setDir(dir);
				bool result = DynamicObject::attemptMove(dir);
				if (result)
					return MOVED;
				else exit(5); //there's a problem with a function here if result != true ...
			}
		}

		//otherwise can't see FrackMan...

		//walk in a Direction
		//first determine the Direction
		
		if (getDirTimes() == 0)
		{
			Direction r_dir = generateRandomDirection();
			CoordType x = getX(), y = getY();
			int i = 0;
			while (!getWorld()->tryToMoveFromLocation(x, y, r_dir) && i < 20) //shouldn't take more than 20 tries...
			{
				r_dir = generateRandomDirection();
				i++;
			}
			setDir(r_dir);
			rollNumberOfTimesToMoveInCurrentDirection();
		}
		else
		{
			if (m_currentNonrestTick > 200)
			{
				Direction d = getWorld()->canITurnAndMove(this);
				if (d != none)
				{
					setDir(d);
					rollNumberOfTimesToMoveInCurrentDirection();
					m_currentNonrestTick = 0;
				}
			}
		}


		//then move
		bool result = DynamicObject::attemptMove(getDirection());

		if (!result)
		{
			m_numTimesCurrentDir = 0; //will make it re-roll for a new Direction next tick
			return STATIONARY;
		}
		else return MOVED;


		//do some stuff

		break;
	}




}

GraphObject::Direction Protester::tryToGetToFrackMan() const
{
	return getWorld()->directLineToFrackMan(this);
}

void Protester::bribeMe()
{
	getWorld()->increaseScore(25);
	setProtesterState(leaving);
}


void Protester::respondToPlayer(double distanceOfInteraction)
{
	if (distanceOfInteraction == DISTANCE_YELL && getWorld()->amIFacingFrackMan())
	{
		getWorld()->getPlayer()->getHurt(DAMAGE_YELL);
		getWorld()->playSound(SOUND_PROTESTER_YELL);
	}
}

void Protester::respondToSquirt(double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		getHurt(DAMAGE_SQUIRT);
	}
}

void Protester::respondToBoulder(double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		getHurt(DAMAGE_BOULDER);
	}
}


void Protester::getHurt(int damage)
{
	changeHealthBy(damage);
	if (didIDie())
	{
		performGiveUpAction();
	}
	else
	{
		performAnnoyedAction();
	}
}


void Protester::performGiveUpAction()
{
	setProtesterState(leaving);
	setAnnoyed(false); //have him started moving
	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
}

void Protester::performAnnoyedAction()
{
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	setAnnoyed(true);
	setAnnoyedTickCount(max(50, 100 - getWorld()->getLevel() * 10));
}


//thankfully, the rest tick for being bribed as a Hardcore Protester
//is the same as being annoyed as a regular Protester
//so can recycle everything in Protester::doSomething() by having bribeMe() do something different, namely



void HardcoreProtester::bribeMe()
{
	getWorld()->increaseScore(50);
	setAnnoyedTickCount(max(50, 100 - getWorld()->getLevel() * 10));
	setAnnoyed(true);
}
// doesn't play the annoyed sound ('gold' sound handled by Gold object)
//but gets him "annoyed" / stunned for appropriate amount of time


//so now I don't need to do anything fancy
int HardcoreProtester::doSomething()
{
	return Protester::doSomething();
}

void HardcoreProtester::setDetectionRange()
{
	m_detectionRange = 16 + getWorld()->getLevel() * 2;
}

GraphObject::Direction HardcoreProtester::tryToGetToFrackMan() const
{
	if (getWorld()->distanceBetweenActors(this, getWorld()->getPlayer()) >= m_detectionRange) //no reason to use taxing method
		Protester::tryToGetToFrackMan();

	CoordType x_a, x_p, y_a, y_p;
	sendLocation(x_a, y_a);
	getWorld()->getPlayer()->sendLocation(x_p, y_p);

	if (getWorld()->numberOfStepsFromLocationToGoal(x_a, y_a, x_p, y_p) <= m_detectionRange)
	{
		Direction d = getWorld()->tellMeHowToGetToMyGoal(this, x_p, y_p);
		return d;
	}
	else return Protester::tryToGetToFrackMan();


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
	Actor::doSomething();


	if (isDead())
		return DEAD;



	switch (m_state)
	{
		//progression of states is only stable -> waiting -> falling (-> death)
	case stable:
		if (!isThereDirtNextToMeInCurrentDirection())
		{
			setBoulderState(waiting);
			setTickStatus(true);
			setTickNumber(WAIT_TIME_BOULDER); //now Actor::doSomething() will count down the tick until it Boulder::performTickAction()'s
		}
		return STATIONARY;
		break; //not necessary
	case waiting:
		//handled in Actor::doSomething()
		break; //not necessary
	case falling:
		//now it's moving
		bool couldMove = DynamicObject::attemptMove(down);
		if (!couldMove) //hit the bottom of the stage or dirt (or another boulder(?))
		{
			die();
			return DEAD;
		}
	}

	return 42;
}

void Boulder::performTickAction()
{
	setBoulderState(falling);
	getWorld()->playSound(SOUND_FALLING_ROCK);

}





// Goodie functions


// NOTE: DOES NOT PLAY A SOUND
int Goodie::doSomething()
{
	Actor::doSomething();

	if (isDead())
	{
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(giveScore());
		return DEAD;
	}

	return 42;

}


int Goodie::respondToPlayer(double distanceOfInteraction)
{
	int interaction = DISTANCE_ACTION_MAP[distanceOfInteraction];

	switch (interaction)
	{
	case INTERACTED:
		interactWithPlayer();
		break;
	case DISCOVERED:
		becomeDiscoveredByPlayer();
		break;
	}
}

void Goodie::interactWithPlayer()
{

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
		getWorld()->increaseScore(giveScore());
		break;
	}

	return result;
}

void Sonar::respondToPlayer(double distanceOfInteraction)
{


	switch (distanceOfInteraction)
	{
		getWorld()->getPlayer()->changeSonarBy(1);
		die();
	}
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
	Actor::doSomething(); //does most of the stuff,
										//including making a sound or killing the Goodie or becoming visible if appropriate

	if (isDead())
	{
		getWorld()->playSound(SOUND_FOUND_OIL); //different sound played for oil compared to other goodies
		getWorld()->getPlayer()->changeGoldBy(1);
		getWorld()->increaseScore(SCORE_GOLD_FRACKMAN);
		return DEAD;
	}

	return 42;
}



// Gold functions

Gold::Gold(CoordType x, CoordType y, StudentWorld * sw, int score = SCORE_GOLD_FRACKMAN, int IID = IID_GOLD) :
	Goodie(IID, sw, x, y)
{
	moveTo(x, y);
	setDir(right);
	//setGroupAs(gold); //lets Protesters know it's Gold

	//determine visibility and type of gold
	Actor* f = getWorld()->getPlayer();
	if (f->getX() == x && f->getY() == y) //if FrackMan is initially where the gold is constructed, he dropped it
	{
		m_frack = true;
		setTickStatus(true);
		setTickNumber(100);
		setVisibility(true);
		//setPickUpGroup(enemies);
		setGroupAs(bribes);
		setScore(SCORE_GOLD_PROTESTER);
	}
	else
	{
		m_frack = false;
		setTickStatus(false);
		setVisibility(false);
		setGroupAs(goodies);
		//setPickUpGroup(player);
		setScore(SCORE_GOLD_FRACKMAN);
	}

}




// NOT FINISHED. Return after implementing/designing Protester classes
int Gold::doSomething()
{

	Actor::doSomething(); //does most of the stuff,
						  //including making a sound or killing the Goodie or becoming visible if appropriate

	if (isDead())
	{
		if (whatGroupAmI() == goodies)
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->getPlayer()->changeGoldBy(1);
			getWorld()->increaseScore(SCORE_GOLD_FRACKMAN);
			return DEAD;
		}
		else
			return DEAD; //let enemy handle the fact that it got bribed
	}

	return 42;


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

	if(isDead())
	{
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->changeSquirtsBy(5);
		getWorld()->increaseScore(giveScore());
		break;
	}

	return result;

}



GraphObject::Direction generateRandomDirection()
{
	int r = rand() % 4;
	switch (r) 
	{
	case 0:
		return GraphObject::up;
	case 1:
		return GraphObject::down;
	case 2:
		return GraphObject::left;
	case 3:
		return GraphObject::right;
	}
}
