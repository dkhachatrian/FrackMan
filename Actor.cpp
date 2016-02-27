#include "Actor.h"
#include "StudentWorld.h"


#include <map>
std::map<double, int> DISTANCE_ACTION_MAP;

void initializeDistanceActionMap()
{
	DISTANCE_ACTION_MAP[DISTANCE_INTERACT] = INTERACTED;
	DISTANCE_ACTION_MAP[DISTANCE_DISCOVER] = DISCOVERED;
	//DISTANCE_ACTION_MAP[DISTANCE_YELL] = CAN_YELL;
	//DISTANCE_ACTION_MAP[DISTANCE_PLACEMENT] = PLACED;
	DISTANCE_ACTION_MAP[DISTANCE_USE_SONAR] = DISCOVERED; //DISTANCE_USE_SONAR only passed in with StudentWorld::letPlayerUseSonar()
}




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






bool Actor::isThereDirtNextToMeInDirection(Direction dir) const
{
	return getWorld()->isThereDirtInDirectionOfActor(this, dir);
}

bool Actor::isThereDirtNextToMeInCurrentDirection() const
{
	return getWorld()->isThereDirtInDirectionOfActor(this, getDirection());

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
	bool result = attemptMove(getDirection());

	if (!result)
	{
		die();
		return 41;
	}
	return 42;
	
}


void Squirt::respondToBoulder(Boulder* boulder, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
		die(); //enemy class will deal with damage and sound effect
}
void Squirt::respondToEnemy(Protester* enemy, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		//enemy->getHurt(DAMAGE_SQUIRT);
		//die();
	}
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

void FrackMan::getHurt(int damage)
{
	changeHealthBy(damage);
	if (didIDie())
	{
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		die();
	}
}

void FrackMan::respondToEnemy(Protester* enemy, double distanceOfInteraction)
{
	//getHurt(DAMAGE_YELL);
	//enemy->respondToPlayer(this, distanceOfInteraction); //called here so that
												// enemy can still call Actor::doSomething() and respond to other Actors
												// (e.g., Squirts)
}



// Protester functions

// have to deal with several ticks -- cannot rely on Actor::doSomething() to handle the tick (can handle asking the StudentWorld to interact though)


int Protester::doSomething()
{
	//first check to see if I'm dead
	if (isDead())
		return DEAD;

	if (getProtesterState() != leaving)
		Actor::doSomething();

	//then we'll see if it should be resting due to mandatory rest ticking

	//then count up a rest tick
	countUpARestTick();

	if (getCurrentRestTick() != getMaxRestTick()) //if can't move
	{
		//setResting(false); //let it have a chance to move later on
		//setCurrentRestTick(0); // reset tick counter
		return STATIONARY; //immediately return
	}
	else
	{
		setCurrentRestTick(0); //reset rest tick counter
	}



	//see if it's resting
	if (amIResting()) //if it is, determine why
	{
		if (amIAnnoyed()) // count up annoyed ticks, and don't do anything if annoyed
		{
			countUpAnAnnoyedTick();
			if (getCurrentAnnoyedTick() == getMaxAnnoyedTick()) //once it reaches here, cooldown time is finished
			{
				setAnnoyedTick(0);
				setAnnoyed(false); //stop being annoyed
				setResting(false); //and don't be considerd resting
			}
			return STATIONARY; //will not do anything while it's annoyed
		}
	}


	if (didIDie()) //maybe from Boulder or something
	{
		setCurrentRestTick(0);
		setProtesterState(leaving);
		//performGiveUpAction();
	}

	

	Direction dir = none;

	switch (m_pState)
	{
	case leaving: //ignore other places, and instead try moving
	{
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
	}
	case resting: //no longer used...
	{
		if (didIDie())
			setProtesterState(leaving);
		else
			setProtesterState(OK);
		return STATIONARY;
		//break;
	}
	case coolingDown: //no longer used...
	{
		m_currentCoolDownTick++;
		if (m_currentCoolDownTick % m_coolDownPeriod == 0)
		{
			setProtesterState(OK);
			m_currentCoolDownTick = 0;
		}
		return STATIONARY;
		break;
	}
	case OK:

		//attempt to yell at FrackMan

		//conveniently, DISCOVERED == 4 == shouting distance of Protester
		if (getWorld()->isActorAffectedByGroup(this, player, DISCOVERED) && getWorld()->amIFacingFrackMan(this))
		{
			//getWorld()->playSound(SOUND_PROTESTER_YELL);
			//getWorld()->getPlayer()->changeHealthBy(-2);
			//setProtesterState(coolingDown);
			return STATIONARY;
		}


		Direction dir = chooseDirection();
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

GraphObject::Direction Protester::chooseDirection()
{
	//attempt to walk toward FrackMan intelligently (Direction and ability to see FrackMan being determined by the tryToGetToFrackMan() function)
	if (!getWorld()->isActorAffectedByGroup(this, player, DISCOVERED))
	{
		Direction dir = tryToGetToFrackMan();

		if (dir != none)
		{
			m_numTimesCurrentDir = 0;
			setDir(dir);
			return dir;
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
		while (getWorld()->isThereDirtInDirectionOfActor(this, r_dir) || !getWorld()->tryToMoveFromLocation(x, y, r_dir)) //shouldn't take more than 20 tries...
		{
			r_dir = generateRandomDirection();
		}
		setDir(r_dir);
		rollNumberOfTimesToMoveInCurrentDirection();
		return r_dir;
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
				return d;
			}
		}
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



void Protester::respondToPlayer(FrackMan* player, double distanceOfInteraction)
{
	if (!amIAnnoyed() && distanceOfInteraction <= DISTANCE_YELL && getWorld()->amIFacingFrackMan(this))
	{
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		player->getHurt(DAMAGE_YELL);
		setAnnoyed(true);
		setResting(true);
		setAnnoyedTick(0);
		setMaxAnnoyedTickAs(15);
		//setMaxAnnoyedRestTickAs(15);
	}
}

void Protester::respondToSquirt(Squirt* squirt, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		squirt->die();
		getHurt(DAMAGE_SQUIRT);
		if (didIDie())
		{
			performGiveUpAction();
			getWorld()->increaseScore(100); //score for killing with squirt
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			setAnnoyed(true);
			setResting(true);
			setMaxAnnoyedTickAs(max(50, 100 - getWorld()->getLevel() * 10));
			setAnnoyedTick(0); //reset it to 0
			//performAnnoyedAction();
		}
	}
}

void Protester::respondToBoulder(Boulder* boulder, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT && getProtesterState() != leaving)
	{
		getHurt(DAMAGE_BOULDER);

		if (didIDie())
		{
			performGiveUpAction();
			getWorld()->increaseScore(500); //score for bonking with boulder (SAME AS FOR HARDCORE PROTESTER)s
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			setAnnoyed(true);
			setResting(true);
			setMaxAnnoyedTickAs(max(50, 100 - getWorld()->getLevel() * 10));
			setAnnoyedTick(0); //reset it to 0
							   //performAnnoyedAction();
		}
	}
}

void Protester::respondToBribe(Gold* bribe, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		bribe->die();
		startToLeave();
		getWorld()->increaseScore(25); //points for bribing
	}
}

void Protester::startToLeave()
{
	setAnnoyed(false); //have him start moving
	setProtesterState(leaving);
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
		//getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		//setAnnoyed(true);
	}
}


void Protester::performGiveUpAction()
{

	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	setAnnoyed(false);
	setProtesterState(leaving);
}


void Protester::setRestTick()
{
	m_restTicks = max(0, 3 - (getWorld()->getLevel() / 4));
}






void HardcoreProtester::respondToBribe(Gold* bribe, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		setAnnoyed(true);
		setResting(true);
		setMaxAnnoyedTickAs(max(50, 100 - getWorld()->getLevel() * 10));
		setAnnoyedTick(0); // reset tick time
		getWorld()->increaseScore(50); //points for bribe (different from regular Protester)
		bribe->die();
	}
}


//need to make this naturally come from Protester:respondToSquirt (only difference is points...)
void HardcoreProtester::respondToSquirt(Squirt* squirt, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		squirt->die();
		getHurt(DAMAGE_SQUIRT);
		if (didIDie())
		{
			performGiveUpAction();
			getWorld()->increaseScore(100); //score for killing with squirt
		}
		else
		{
			getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
			setAnnoyed(true);
			setResting(true);
			setMaxAnnoyedTickAs(max(50, 100 - getWorld()->getLevel() * 10));
			setAnnoyedTick(0); //reset it to 0
							   //performAnnoyedAction();
		}
	}
}

// will stop counting how far away it is after going past getDetectionRange() depth
int HardcoreProtester::howFarAwayAmIFromFrackMan() const
{
	CoordType x, y, a, b;
	int numberOfSteps;
	sendLocation(x, y);
	getWorld()->getPlayer()->sendLocation(a, b);
	getWorld()->howToGetFromLocationToGoal(x, y, a, b, numberOfSteps, getDetectionRange());
	return numberOfSteps;
}

/*
GraphObject::Direction HardcoreProtester::tryToGetToFrackMan() const
{
	if (howFarAwayAmIFromFrackMan() > getDetectionRange())
		return Protester::tryToGetToFrackMan();

	else return getWorld()->tellMeHowToGetToMyGoal(this, getWorld()->getPlayer()->getX(), getWorld()->getPlayer()->getY());


}
*/



//thankfully, the rest tick for being bribed as a Hardcore Protester
//is the same as being annoyed as a regular Protester
//so can recycle everything in Protester::doSomething() by having bribeMe() do something different, namely



void HardcoreProtester::bribeMe()
{
	getWorld()->increaseScore(50);
	setMaxAnnoyedTickAs(max(50, 100 - getWorld()->getLevel() * 10));
	setAnnoyed(true);
}
// doesn't play the annoyed sound ('gold' sound handled by Gold object)
//but gets him "annoyed" / stunned for appropriate amount of time


//so now I don't need to do anything fancy

//need to fix
int HardcoreProtester::doSomething()
{
	return Protester::doSomething();
	//Actor::doSomething();
	//attemptMove(tryToGetToFrackMan());
	return 42;
}

void HardcoreProtester::setDetectionRange()
{
	m_detectionRange = 16 + getWorld()->getLevel() * 2;
	//m_detectionRange = 200;
}

GraphObject::Direction HardcoreProtester::tryToGetToFrackMan() const
{
	//if (getWorld()->distanceBetweenActors(this, getWorld()->getPlayer()) > getDetectionRange()) //no reason to use taxing method
	//	Protester::tryToGetToFrackMan();

	CoordType x_a, x_p, y_a, y_p;
	sendLocation(x_a, y_a);
	getWorld()->getPlayer()->sendLocation(x_p, y_p);

	if (howFarAwayAmIFromFrackMan() <= getDetectionRange())
	{
		Direction d = getWorld()->tellMeHowToGetToMyGoal(this, x_p, y_p);
		return d;
	}
	else return Protester::tryToGetToFrackMan();


}

/*
GraphObject::Direction HardcoreProtester::tryToGetToFrackMan() const
{
	CoordType x, y, a, b;
	sendLocation(x, y);
	getWorld()->getPlayer()->sendLocation(a, b);

	if (howFarAwayAmIFromFrackMan() <= getDirectionRange())
	{
		getWorld()->howToGetFromLocationToGoal(x, y, a, b);
	}
	else return getWorld()->directLineToFrackMan(this);
}

*/




// Boulder functions

Boulder::Boulder(CoordType x, CoordType y, StudentWorld* sw, int IID = IID_BOULDER, unsigned int depth = DEPTH_BOULDER) :
	DynamicObject(IID, depth, sw, x, y)
{
	//moveTo(x, y);

	m_state = stable;
	//m_haveWaited = false;
	setDir(down);
	setVisibility(true);
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

void Boulder::respondToEnemy(Protester* enemy, double distanceOfInterest)
{
	if (getBoulderState() != falling)
		return;


}

void Boulder::respondToPlayer(FrackMan* player, double distanceOfInterest)
{
	if (getBoulderState() != falling)
		return;

	else
	{
		player->getHurt(DAMAGE_BOULDER);
	}
}



// Goodie functions


// NOTE: DOES NOT PLAY A SOUND
int Goodie::doSomething()
{
	Actor::doSomething();

	return 42;
}


void Goodie::respondToPlayer(FrackMan* player, double distanceOfInteraction)
{
	int interaction = DISTANCE_ACTION_MAP[distanceOfInteraction];

	switch (interaction)
	{
	case INTERACTED:
		interactWithPlayer(player, distanceOfInteraction);
		break;
	case DISCOVERED:
		becomeDiscoveredByPlayer();
		break;
	}
}




void Goodie::becomeDiscoveredByPlayer()
{
	setVisibility(true);
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

/*
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
*/

void Sonar::interactWithPlayer(FrackMan* player, double distanceOfInteraction)
{
	player->changeSonarBy(1);
	getWorld()->playSound(SOUND_GOT_GOODIE);
	getWorld()->increaseScore(giveScore());
	die();
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
	Goodie::doSomething();
	return 42;
}

void Barrel::interactWithPlayer(FrackMan* player, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		getWorld()->changeBarrelsLeftBy(-1);
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(giveScore());
		die();
	}
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
		setTickStatus(true);
		setTickNumber(100);
		setVisibility(true);
		//setPickUpGroup(enemies);
		setGroupAs(bribes);
	}
	else
	{
		setTickStatus(false);
		setVisibility(false);
		setGroupAs(goodies);
		//setPickUpGroup(player);
	}

}




// NOT FINISHED. Return after implementing/designing Protester classes
int Gold::doSomething()
{

	Actor::doSomething(); //does most of the stuff,
						  //including making a sound or killing the Goodie or becoming visible if appropriate


	return 42;
}

void Gold::interactWithPlayer(FrackMan* player, double distanceOfInteraction)
{
	if (whatGroupAmI() == bribes)
		return;
	else
	{
		getWorld()->playSound(SOUND_GOT_GOODIE);
		player->changeGoldBy(1);
		getWorld()->increaseScore(SCORE_GOLD_FRACKMAN);
		die();
		return;
	}
}

// will just die. Will have protester respond with bribe/point increase
void Gold::respondToEnemy(Protester* enemy, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT && whatGroupAmI() == bribes)
	{
		//getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		//die();
	}
	//otherwise, it doesn't respond to enemy
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



void Water::interactWithPlayer(FrackMan* player, double distanceOfInteraction)
{
	if (distanceOfInteraction <= DISTANCE_INTERACT)
	{
		getWorld()->playSound(SOUND_GOT_GOODIE);
		player->changeSquirtsBy(5);
		getWorld()->increaseScore(giveScore());
		die();
	}
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

