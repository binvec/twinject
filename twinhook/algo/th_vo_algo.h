#pragma once
#include "control/th_player.h"

/* Visualization Constants */
static const float VEC_FIELD_MIN_RESOLUTION = 8.f;
static const float MAX_FRAMES_TILL_COLLISION = 200.f;	// used for coloring vector field

/* Algorithmic Constants */
static const float SQRT_2 = sqrt(2.f);

/*
* Unit velocity as a result of moving in specified direction
* [0]: Hold, [1]: Up, [2]: Down, [3]: Left, [4]: Right,
* [5]: Top-left, [6]: Top-right, [7]: Bottom-left, [8]: Bottom-right,
* [9]: F Up, [10]: F Down, [11]: F Left, [12]: F Right, [13]: F Top-left,
* [14]: F top-right, [15]: F bottom-left, [16]: F bottom-right
*/
static const vec2 DIRECTION_VEL[] =
{
	vec2(0,0), vec2(0,-1), vec2(0,1), vec2(-1,0), vec2(1,0),
	vec2(-SQRT_2, -SQRT_2), vec2(SQRT_2, -SQRT_2),
	vec2(-SQRT_2, SQRT_2), vec2(SQRT_2, SQRT_2),
	vec2(0,-1), vec2(0,1), vec2(-1,0), vec2(1,0),
	vec2(-SQRT_2, -SQRT_2), vec2(SQRT_2, -SQRT_2),
	vec2(-SQRT_2, SQRT_2), vec2(SQRT_2, SQRT_2)
};

static const int NUM_DIRS = sizeof DIRECTION_VEL / sizeof vec2;

static const bool FOCUSED_DIR[] = {
	false,false,false,false,false,false,false,false,false,
	true,true,true,true,true,true,true,true
};

static const BYTE DIR_KEYS[][3] = {
	{ DIK_LSHIFT,	NULL,			NULL },	// focus by default
	{ DIK_UP,		NULL,			NULL },
	{ DIK_DOWN,		NULL,			NULL },
	{ DIK_LEFT,		NULL,			NULL },
	{ DIK_RIGHT,	NULL,			NULL },
	{ DIK_UP,		DIK_LEFT,		NULL },
	{ DIK_UP,		DIK_RIGHT,		NULL },
	{ DIK_DOWN,		DIK_LEFT,		NULL },
	{ DIK_DOWN,		DIK_RIGHT,		NULL },
	{ DIK_UP,		NULL,			DIK_LSHIFT },
	{ DIK_DOWN,		NULL,			DIK_LSHIFT },
	{ DIK_LEFT,		NULL,			DIK_LSHIFT },
	{ DIK_RIGHT,	NULL,			DIK_LSHIFT },
	{ DIK_UP,		DIK_LEFT,		DIK_LSHIFT },
	{ DIK_UP,		DIK_RIGHT,		DIK_LSHIFT },
	{ DIK_DOWN,		DIK_LEFT,		DIK_LSHIFT },
	{ DIK_DOWN,		DIK_RIGHT,		DIK_LSHIFT }
};

static const BYTE CONTROL_KEYS[] = { DIK_UP, DIK_DOWN, DIK_LEFT, DIK_RIGHT, DIK_LSHIFT, DIK_X };

/**
 * \brief Implementation of velocity obstacle based algorithm
 *
 * Overview:
 * The player has 5 possible velocity states excluding combination states,
 * UP, DOWN, LEFT, RIGHT, HOLD
 *
 * First we must calibrate the algorithm by determining the player velocity, by
 * frame division.
 *
 * Each velocity state then projected for collisions with obstacles. The state that
 * results in a collision being the furthest away (greedy) is the desired action.
 */
class th_vo_algo : public th_algorithm
{
	/* Adaptibility Parameters */
	// Should we use hitcircles instead of hitboxes
	bool hitCircle = false;

	/* Calibration Parameters */
	bool isCalibrated = false;
	// Number of frames spent calibrating so far
	int calibFrames = 0;
	// Starting x position when beginning calibration
	float calibStartX = -1;
	float playerVel = 0;
	float playerFocVel = 0;

	void calibInit();

	/**
	* \brief Do one tick of calibration
	* \return Whether calibration is complete or not
	*/
	bool calibTick();

	/* Visualization Parameters*/

	/**
	 * \brief Find the minimum collision tick of a static AABB
	 * \param bullets The bullets to check collision against
	 * \param p Top-left corner of AABB to check
	 * \param s Size of AABB to check
	 * \param collided All bullets which collide with the AABB are added to this vector
	 * \return The minimum collison tick
	 */
	float th_vo_algo::minStaticCollideTick(
		const std::vector<entity> &bullets,
		const vec2 &p, const vec2 &s,
		std::vector<entity> &collided) const;
	/**
	 * \brief Draw collision potentials at a specified resolution
	 * \param bullets The bullets to check collision against
	 * \param p Position of AABB containing visualization boundary
	 * \param s Size of AABB containing visualization boundary
	 * \param minRes Minimum allowable resolution for visualization
	 */
	void th_vo_algo::vizPotentialQuadtree(
		const std::vector<entity> &bullets,
		vec2 p, vec2 s,
		float minRes) const;

public:
	th_vo_algo(th_player *player) : th_algorithm(player) {}
	th_vo_algo(th_player *player, bool hit_circle) : th_algorithm(player), hitCircle(hit_circle) {}

	~th_vo_algo() = default;

	void onBegin() override;
	void onTick() override;
	void visualize(IDirect3DDevice9 *d3dDev) override;
};
