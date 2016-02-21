#include "Actor.h"
#include "StudentWorld.h"


//int max(int x, int y);
// Actor functions



//int Actor::doSomething()
//{
//	if (isDead())
//		return -1;
//}

bool Actor::isThereDirtBelowMe() const
{
	return getWorld()->isThereDirtBelowActor(this);
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
	CoordType x_n, y_n;

	sendLocation(x_n, y_n);


	if (!moveMatchesDir(dir))
	{
		GraphObject::setDirection(dir);
		moveTo(getX(), getY()); //"move" with no change in coordinates to have animation play
		return false; //didn't move
	}
	//otherwise, have the world move the DynamicObject if it can

	return (getWorld()->tryToMoveMe(this, dir));
}

// FrackMan functions

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

bool FrackMan::attemptMove(const Direction dir)
{
	bool result = DynamicObject::attemptMove(dir);
	doSpecializedAction();


	return result;

}

//FrackMan's specialized action are many, depending on what key is pressed.
//	but at the moment, only thing implemented is for movement (i.e. removing dirt in front of it)
bool FrackMan::doSpecializedAction()
{
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
		m_ticksLeft--;

		if (m_ticksLeft == 0)
		{
			die();
			return DEAD;
		}
	}

	if (getWorld()->isGoodieCollected(this, whoCanPickMeUp()))
	{
		//getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(giveScore());
		die();
		return COLLECTED;
	}

	if (!isVisible() && (getWorld()->distanceBetweenActors(getWorld()->getPlayer(), this) <= DISTANCE_DISCOVER))
	{
		setVisible(true);
		setVisibleFlag(true);
		return DISCOVERED;
	}

	//if nothing happened, stays stationary

	return STATIONARY;
}










// Sonar Functions

Sonar::Sonar(CoordType x, CoordType y, StudentWorld * sw, int score = SCORE_SONAR, int IID = IID_SONAR, Group canPickMeUp = player):
	Goodie(IID, score, sw)
{
	moveTo(x, y);
	setDirection(right);
	setVisible(true);
	setVisibleFlag(true);
	//setPickUpGroup(player);

	m_ticksLeft = max(100, 10 * getWorld()->getLevel()); //it doesn't see the function in StudentWorld.h?
}


int Sonar::doSomething()
{
	int result = Goodie::doSomething(); //does most of the stuff,
	//including making a sound or killing the Goodie or becoming visible if appropriate

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case COLLECTED:
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->changeSonarBy(1);
		break;
	}

	return result;
}







// Barrel functions

Barrel::Barrel(CoordType x, CoordType y, StudentWorld * sw, int score = SCORE_BARREL, int IID = IID_BARREL) :
	Goodie(IID, score, sw, x, y)
{
	setVisible(false);
	setDirection(right);
}

int Barrel::doSomething()
{
	int result = Goodie::doSomething(); //does most of the stuff,
										//including making a sound or killing the Goodie or becoming visible if appropriate

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case COLLECTED:
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->changeBarrelsLeftBy(-1);
		break;
	}

	return result;
}



// Gold functions

Gold::Gold(CoordType x, CoordType y, StudentWorld * sw, int score = SCORE_GOLD, int IID = IID_GOLD) :
	Goodie(IID, score, sw, x, y)
{
	setDirection(right);

	//determine visibility and type of gold
	Actor* f = getWorld()->getPlayer();
	if (f->getX() == x && f->getY() == y) //if FrackMan is initially where the gold is constructed, he dropped it
	{
		m_frack = true;
		setTickStatus(true);
		setVisible(true);
		setVisibleFlag(true);
		setPickUpGroup(enemies);
	}
	else
	{
		m_frack = false;
		setTickStatus(false);
		setVisible(false);
		setVisibleFlag(false);
		setPickUpGroup(player);
	}


}

// NOT FINISHED. Need to implement/design Protester classes
int Gold::doSomething()
{
	int result = Goodie::doSomething();

	if (result == DEAD)
		return DEAD;

	switch (result)
	{
	case COLLECTED:
		if (!wasDroppedByFrackMan())
		{
			getWorld()->playSound(SOUND_GOT_GOODIE);
			getWorld()->getPlayer()->changeGoldBy(1);
		}
		else
		{
			//tell the protestor he got bribed
		}
		break;
	}
}