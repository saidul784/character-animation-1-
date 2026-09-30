/*
 *  Level02.hpp  --  Honda Rush: Career Rider, LEVEL 02
 *
 *  A fixed 10-screen stunt route. Everything in it - ramps, obstacles,
 *  coins, fuel - is placed at a hard coded world position when the level is
 *  built, so the level plays identically every single run. Nothing spawns,
 *  nothing is random.
 *
 *  THE TWO IDEAS THAT MAKE IT WORK
 *
 *  1. The backgrounds are ten panels of one long painted wall laid end to
 *     end, and the camera slides along it. Panel i is drawn at
 *     (i * screen width - slide), so the order 1..10 is a property of the
 *     layout rather than something a timer decides.
 *
 *  2. A ramp is not a picture with a triangle bolted on. When the artwork
 *     loads, the height of its top edge is measured column by column out of
 *     the alpha channel, giving a surface profile that matches the drawing
 *     exactly. The wheels are then placed on that profile, so the Honda
 *     rides up the shape you can actually see.
 *
 *  Entry points used by iMain.cpp:
 *      level02Init()    load the artwork once, at start up
 *      level02Reset()   build and start the route
 *      updateLevel02()  one fixed step
 *      drawLevel02()    render
 */

#ifndef LEVEL02_HPP
#define LEVEL02_HPP

/* ==================== ARTWORK ====================
 *  Level2Art holds transparent copies of the Level 2 pictures: the supplied
 *  files have the chequerboard painted into their pixels, which cannot be
 *  drawn over a background. Same artwork, background removed - the originals
 *  in the "level 2" folder are untouched.
 */
#define L2_ART "Level2Art/"

#define L2_BG_COUNT 10

static const char *L2_BG_FILE[L2_BG_COUNT] =
{
	"../level 2/level02_bg_1.png",
	"../level 2/level02_bg_2.png",
	"../level 2/level02_bg_3.png",
	"../level 2/level02_bg_4.png",
	"../level 2/level02_bg_5.png",
	"../level 2/level02_bg_6.png",
	"../level 2/level02_bg_7.jpeg",
	"../level 2/level02_bg_8.png",
	"../level 2/level02_bg_9.png",
	"../level 2/level02_bg_10.png"
};

/*  The Honda poses come straight out of the level 2 folder: those files now
    carry real transparency, so nothing is processed and nothing is
    substituted - this is the supplied artwork, complete bike and rider.
    All three share one crop, so the tyres sit on the same line in
    every pose.                                                          */
#define L2_BIKE_DIR       "../level 2/"

#define L2_POSE_STANDING  0
#define L2_POSE_NORMAL    1
#define L2_POSE_FAST      2
#define L2_POSE_COUNT     3

static const char *L2_BIKE_FILE[L2_POSE_COUNT] =
{
	L2_BIKE_DIR "Standing_bike_1.png",
	L2_BIKE_DIR "Normal_speed_bike.png",
	L2_BIKE_DIR "High_speed_bike.png"
};

#define L2_CRASH_ART      L2_BIKE_DIR "Crash_image.png"
#define L2_FUEL_ART       L2_ART "fuel_icon.png"

/* ==================== TUNING ==================== */

#define L2_GROUND_Y          150.0   /* the road surface                  */
#define L2_BG_Y_OFFSET       -45     /* hides each photo's own road       */
#define L2_BG_DRAW_HEIGHT   (SCREEN_HEIGHT)
#define L2_BG_PARALLAX       0.45

#define L2_BIKE_SCREEN_X     210.0
#define L2_BIKE_WIDTH        215     /* height follows the artwork        */

/*  Where the tyres actually touch, as a fraction of the sprite width. The
    Honda is grounded on these two points, never on the picture's corners. */
/*  Measured off the supplied PNGs: the rear tyre touches down at 20.5% of
    the sprite width and the front at 84.2%, and the artwork's lowest opaque
    row IS the contact line, so the sprite's bottom edge sits on the road.  */
#define L2_WHEEL_REAR        0.205
#define L2_WHEEL_FRONT       0.842

/* riding */
#define L2_ACCEL             0.34
#define L2_BRAKE             0.34
#define L2_COAST             0.020
#define L2_MAX_SPEED        18.0
#define L2_FAST_SPRITE_AT    9.0

/* jumping */
#define L2_GRAVITY           0.70
#define L2_JUMP_VELOCITY    17.0
#define L2_FAST_FALL         0.95
/*  How hard a crest throws the Honda. The climb rate already carries the
    speed (slope x speed), and L2_LAUNCH_SPEED_GAIN scales it again, so
    arriving fast launches high and crawling over barely leaves the ramp.
    Nothing here is random and nothing is a fixed height.                 */
#define L2_RAMP_LAUNCH       0.90
#define L2_LAUNCH_MIN_GAIN   0.30    /* multiplier at a standstill */
#define L2_LAUNCH_SPEED_GAIN 1.40    /* extra multiplier at full speed */

/*  The Honda must never leave the picture. This is the highest its wheels
    are allowed to reach, chosen so the whole bike stays below the HUD bar:
        ceiling + bike height  <  SCREEN_HEIGHT - HUD
    It is enforced as a limit on the launch itself rather than as a lid the
    bike bumps into, so the arc still looks like a jump: whatever velocity
    would overshoot the ceiling is trimmed to the velocity that just
    reaches it.                                                          */
#define L2_CEILING_Y       470.0

/* fuel */
#define L2_FUEL_MAX        100.0
#define L2_FUEL_START      100.0
#define L2_FUEL_DRAIN        0.040
#define L2_FUEL_PICKUP      35.0

/*  One background panel is this many world pixels wide, and the Honda
    enters panel i at L2_PANEL(i). Content is placed against these so the
    "ramp 1 lives on background 2" table is readable in the code.         */
#define L2_PANEL_SPAN       (SCREEN_WIDTH / L2_BG_PARALLAX)
#define L2_PANEL(i)         ((i) * L2_PANEL_SPAN + L2_BIKE_SCREEN_X)

/*  The route ends with panel 10 filling the screen. */
const double L2_ROUTE_END = (L2_BG_COUNT - 1) * (double)SCREEN_WIDTH / L2_BG_PARALLAX;

/* ==================== WORLD OBJECTS ==================== */

#define L2_RAMP_COUNT     5
#define L2_OBSTACLE_COUNT 5
#define L2_FUEL_COUNT     3
#define L2_MAX_COINS    260
#define L2_PROFILE_N     72     /* surface samples across one ramp */

struct L2Ramp
{
	unsigned int tex;
	double x;                      /* world x of the left edge */
	double w, h;                   /* drawn size               */
	double prof[L2_PROFILE_N];     /* top edge, above the road */
};

struct L2Obstacle
{
	unsigned int tex;
	double x, w, h;
	double insetX, insetY;         /* fairer than the full picture */
	bool   cleared;                /* the Honda got past it        */
	bool   touching;               /* already paid for this hit    */
};

/*  Clipping the first two barriers costs fuel and the ride goes on.
    From obstacle 3 onwards a hit ends the run.                          */
#define L2_OBS_FUEL_PENALTY  28.0
static const int L2_OBS_FATAL[L2_OBSTACLE_COUNT] = { 0, 0, 1, 1, 1 };

struct L2Coin
{
	double x, y;
	int    value;
	int    art;                    /* 0..4 -> 5,10,30,50,100 */
	double size;
	bool   taken;
};

struct L2Fuel
{
	double x, y;
	bool   taken;
};

/* ==================== LEVEL STATE ==================== */

#define LEVEL02_PLAYING   0
#define LEVEL02_WIN       1
#define LEVEL02_GAMEOVER  2

#define L2_LOSE_NONE      0
#define L2_LOSE_FUEL      1
#define L2_LOSE_CRASH     2
#define L2_LOSE_FALL      3

#define L2_HIT_FLASH_TICKS 40

int l2State;
int l2LoseReason;

unsigned int l2BgTex[L2_BG_COUNT] = { 0 };
unsigned int l2CoinTex[5] = { 0 };
unsigned int l2FuelTex = 0;

SpriteSheet l2Bike;          /* standing / normal / fast, one crop */
SpriteSheet l2Crash;
int         l2HitFlash;      /* white-out after a survivable hit */

L2Ramp     l2Ramps[L2_RAMP_COUNT];
L2Obstacle l2Obstacles[L2_OBSTACLE_COUNT];
L2Coin     l2Coins[L2_MAX_COINS];
L2Fuel     l2Fuels[L2_FUEL_COUNT];
int        l2CoinCount;

double l2CameraX;
double l2Speed;
double l2BikeY, l2BikeVY;
double l2BikeAngle;              /* degrees, matches the surface slope */
double l2PrevGround, l2LastClimb;
int    l2BikeW, l2BikeH;
bool   l2OnGround;
bool   l2JumpRequested;

int    l2Score;
int    l2CoinsTaken;
double l2Fuel;
int    l2Ticks;

static const int L2_COIN_VALUE[5] = { 5, 10, 30, 50, 100 };
static const char *L2_COIN_ART[5] =
{
	L2_ART "coin_5.png",  L2_ART "coin_10.png", L2_ART "coin_30.png",
	L2_ART "coin_50.png", L2_ART "coin_100.png"
};

/* ==================== MUSIC ==================== */
void playLevel02Music()
{
	musicPlay(MUSIC_LEVEL02);
}

void stopLevel02Music()
{
	musicStop();
}

/* ==================== RAMP ARTWORK + SURFACE ====================
 *
 *  Loads one ramp and measures its top edge out of the alpha channel, so
 *  the surface the wheels ride is the shape that is drawn.
 */
static bool loadRampArt(L2Ramp *r, const char *path, double drawW)
{
	int w, h, bpp, i, y, col, top;
	unsigned char *data;

	r->tex = 0;
	r->w = drawW;
	r->h = drawW;

	data = stbi_load(path, &w, &h, &bpp, 4);
	if (data == NULL)
	{
		printf("[level02] FAILED to load ramp: %s\n", path);
		for (i = 0; i < L2_PROFILE_N; i++) r->prof[i] = 0;
		return false;
	}

	r->h = drawW * h / (double)w;      /* never distorted */

	for (i = 0; i < L2_PROFILE_N; i++)
	{
		col = (int)((i + 0.5) * w / L2_PROFILE_N);
		if (col < 0)  col = 0;
		if (col >= w) col = w - 1;

		top = -1;
		for (y = 0; y < h; y++)
			if (data[(y * w + col) * 4 + 3] > 60) { top = y; break; }

		/* stb gives row 0 as the top of the picture */
		r->prof[i] = (top < 0) ? 0.0 : (h - top) * r->h / (double)h;
	}

	glGenTextures(1, &r->tex);
	glBindTexture(GL_TEXTURE_2D, r->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
	return true;
}

/*  Height of whatever the Honda would stand on at this world position:
    the road, or a ramp if one covers it.                                 */
double l2SurfaceAt(double worldX)
{
	double best = L2_GROUND_Y;
	int i;

	for (i = 0; i < L2_RAMP_COUNT; i++)
	{
		L2Ramp *r = &l2Ramps[i];
		double t, frac, a, b, hgt;
		int k;

		if (r->tex == 0) continue;
		if (worldX < r->x || worldX > r->x + r->w) continue;

		t = (worldX - r->x) / r->w * (L2_PROFILE_N - 1);
		k = (int)t;
		if (k < 0) k = 0;
		if (k > L2_PROFILE_N - 2) k = L2_PROFILE_N - 2;
		frac = t - k;

		a = r->prof[k];
		b = r->prof[k + 1];
		hgt = L2_GROUND_Y + a + (b - a) * frac;    /* smooth between samples */

		if (hgt > best) best = hgt;
	}
	return best;
}

/* ==================== SET UP ==================== */

void level02Init()
{
	int i;

	for (i = 0; i < L2_BG_COUNT; i++)
		l2BgTex[i] = loadImage(L2_BG_FILE[i]);

	for (i = 0; i < 5; i++)
		l2CoinTex[i] = loadImage(L2_COIN_ART[i]);

	l2FuelTex = loadImage(L2_FUEL_ART);

	loadImageList(&l2Bike, L2_BIKE_FILE, L2_POSE_COUNT);
	loadSpriteSheet(&l2Crash, L2_CRASH_ART, 1, 1);

	l2BikeW = L2_BIKE_WIDTH;
	if (l2Bike.frameW > 0)
		l2BikeH = (int)((double)L2_BIKE_WIDTH * l2Bike.frameH / l2Bike.frameW + 0.5);
	else
		l2BikeH = L2_BIKE_WIDTH;

	/*  The five ramps, in order. Widths are chosen so each one reads as a
	    proper stunt structure next to a 215px Honda.                     */
	loadRampArt(&l2Ramps[0], L2_ART "Wooden_high_inclined_ramp_1.png", 470);
	loadRampArt(&l2Ramps[1], L2_ART "Metal_stunt_ramp_2.png",          470);
	loadRampArt(&l2Ramps[2], L2_ART "Broken_wooden_ramp_3.png",        470);
	loadRampArt(&l2Ramps[3], L2_ART "Double_step_stant_ramp_4.png",    490);
	loadRampArt(&l2Ramps[4], L2_ART "Large_launch_ramp_5.png",         520);

	l2Obstacles[0].tex = loadImage(L2_ART "Fire_obstacle(1).png");
	l2Obstacles[1].tex = loadImage(L2_ART "Stone_obstacle(2).png");
	l2Obstacles[2].tex = loadImage(L2_ART "Wooden_obstacle(3).png");
	l2Obstacles[3].tex = loadImage(L2_ART "Fire_obstacle(4).png");
	l2Obstacles[4].tex = loadImage(L2_ART "Stripe_circle_obstacle(5).png");

	/* drawn size of each obstacle, kept low enough to be jumpable */
	l2Obstacles[0].w = 215; l2Obstacles[0].h = 117;
	l2Obstacles[1].w = 250; l2Obstacles[1].h =  89;
	l2Obstacles[2].w = 240; l2Obstacles[2].h = 107;
	l2Obstacles[3].w = 218; l2Obstacles[3].h = 120;
	l2Obstacles[4].w = 205; l2Obstacles[4].h = 117;

	for (i = 0; i < L2_OBSTACLE_COUNT; i++)
	{
		l2Obstacles[i].insetX = l2Obstacles[i].w * 0.16;
		l2Obstacles[i].insetY = l2Obstacles[i].h * 0.14;
	}
}

/* ---- track building helpers ---- */

static void l2AddCoin(double x, double y, int artIndex)
{
	if (l2CoinCount >= L2_MAX_COINS) return;
	if (artIndex < 0) artIndex = 0;
	if (artIndex > 4) artIndex = 4;

	l2Coins[l2CoinCount].x     = x;
	l2Coins[l2CoinCount].y     = y;
	l2Coins[l2CoinCount].art   = artIndex;
	l2Coins[l2CoinCount].value = L2_COIN_VALUE[artIndex];
	l2Coins[l2CoinCount].size  = 34.0 + artIndex * 5.0;   /* 34..54 */
	l2Coins[l2CoinCount].taken = false;
	l2CoinCount++;
}

static void l2AddCoinRow(double x, double gap, int n, int artIndex, double y)
{
	int i;
	for (i = 0; i < n; i++)
		l2AddCoin(x + i * gap, y, artIndex);
}

/*  An arc of coins over a ramp and its obstacle - the reward for a clean
    jump rather than a crawl.                                             */
static void l2AddCoinArc(double centreX, double spread, int artIndex, double peak)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		double t = (i - 2) / 2.0;                       /* -1 .. +1 */
		l2AddCoin(centreX + t * spread, L2_GROUND_Y + peak - t * t * (peak * 0.45),
		          artIndex);
	}
}

/*  Lays out the whole route. Ramp N is always paired with obstacle N, on
    the background the brief asks for:
        bg2 r1+o1, bg4 r2+o2, bg6 r3+o3, bg8 r4+o4, bg9 r5+o5.           */
static void buildLevel02Track()
{
	static const int rampPanel[L2_RAMP_COUNT] = { 1, 3, 5, 7, 8 };

	/*  Each obstacle sits immediately off the end of its ramp, inside the
	    jump path, so the launch flows straight into the leap. Verified by
	    simulating the arc off every ramp across the whole speed range.   */
	static const double obsGap[L2_RAMP_COUNT] = { 60.0, 60.0, 90.0, 70.0, 60.0 };
	int i;

	l2CoinCount = 0;

	for (i = 0; i < L2_RAMP_COUNT; i++)
	{
		double rampX = L2_PANEL(rampPanel[i]) + ((i == 4) ? 700.0 : 620.0);
		double obsX  = rampX + l2Ramps[i].w + obsGap[i];

		l2Ramps[i].x = rampX;

		l2Obstacles[i].x        = obsX;
		l2Obstacles[i].cleared  = false;
		l2Obstacles[i].touching = false;

		/* coins riding the jump arc, worth more as the route goes on */
		l2AddCoinArc(obsX + l2Obstacles[i].w * 0.5, 190.0, (i < 2) ? 2 : 3,
		             150.0 + i * 12.0);
	}

	/* ---- coins along the road ----
	   early: fives and tens; middle: tens and thirties; late: the big ones */
	l2AddCoinRow(L2_PANEL(0) +  620, 70,  6, 0, L2_GROUND_Y + 55);
	l2AddCoinRow(L2_PANEL(0) + 1500, 70,  6, 0, L2_GROUND_Y + 105);
	l2AddCoinRow(L2_PANEL(0) + 2200, 70,  5, 1, L2_GROUND_Y + 55);

	l2AddCoinRow(L2_PANEL(2) +  400, 70,  6, 0, L2_GROUND_Y + 55);
	l2AddCoinRow(L2_PANEL(2) + 1200, 70,  6, 1, L2_GROUND_Y + 100);
	l2AddCoinRow(L2_PANEL(2) + 2000, 70,  5, 1, L2_GROUND_Y + 55);

	l2AddCoinRow(L2_PANEL(4) +  500, 75,  5, 1, L2_GROUND_Y + 55);
	l2AddCoinRow(L2_PANEL(4) + 1400, 75,  5, 2, L2_GROUND_Y + 105);
	l2AddCoinRow(L2_PANEL(4) + 2150, 75,  4, 2, L2_GROUND_Y + 55);

	l2AddCoinRow(L2_PANEL(6) +  500, 80,  5, 2, L2_GROUND_Y + 60);
	l2AddCoinRow(L2_PANEL(6) + 1350, 80,  4, 3, L2_GROUND_Y + 110);
	l2AddCoinRow(L2_PANEL(6) + 2100, 80,  4, 2, L2_GROUND_Y + 60);

	l2AddCoinRow(L2_PANEL(8) +  300, 85,  4, 3, L2_GROUND_Y + 60);
	l2AddCoinRow(L2_PANEL(8) + 2050, 90,  3, 4, L2_GROUND_Y + 115);

	/* ---- the final stretch: mixed rows, 10 / 30 / 5 / 50 ---- */
	{
		static const int mix1[4] = { 1, 2, 0, 3 };
		static const int mix2[4] = { 3, 1, 4, 2 };
		static const int mix3[5] = { 0, 2, 3, 1, 4 };

		for (i = 0; i < 4; i++)
			l2AddCoin(L2_PANEL(9) + 320 + i * 85, L2_GROUND_Y + 60, mix1[i]);
		for (i = 0; i < 4; i++)
			l2AddCoin(L2_PANEL(9) + 900 + i * 85, L2_GROUND_Y + 110, mix2[i]);
		for (i = 0; i < 5; i++)
			l2AddCoin(L2_PANEL(9) + 1500 + i * 85, L2_GROUND_Y + 65, mix3[i]);

		l2AddCoin(L2_PANEL(9) + 2150, L2_GROUND_Y + 90, 4);
		l2AddCoin(L2_PANEL(9) + 2260, L2_GROUND_Y + 90, 4);
	}

	/* ---- the three fuel cans, exactly where the brief puts them ---- */
	l2Fuels[0].x = l2Obstacles[0].x + 430;     /* after obstacle 1 */
	l2Fuels[1].x = l2Obstacles[2].x + 430;     /* after obstacle 3 */
	l2Fuels[2].x = l2Ramps[4].x    - 520;      /* before obstacle 5 */

	for (i = 0; i < L2_FUEL_COUNT; i++)
	{
		l2Fuels[i].y     = L2_GROUND_Y + 30;
		l2Fuels[i].taken = false;
	}
}

void level02Reset()
{
	l2CameraX    = 0;
	l2Speed      = 0;
	l2BikeY      = L2_GROUND_Y;
	l2BikeVY     = 0;
	l2BikeAngle  = 0;
	l2OnGround   = true;
	l2JumpRequested = false;
	l2PrevGround = L2_GROUND_Y;
	l2LastClimb  = 0;

	l2HitFlash   = 0;
	l2Score      = 0;
	l2CoinsTaken = 0;
	l2Fuel       = L2_FUEL_START;
	l2Ticks      = 0;

	l2State      = LEVEL02_PLAYING;
	l2LoseReason = L2_LOSE_NONE;

	buildLevel02Track();
}

/* ==================== COLLISION HELPERS ==================== */

/*  The Honda's collision box, narrower than the picture so a stray pixel of
    backpack or mirror never counts as a crash.                           */
static void l2BikeBox(double *bx, double *by, double *bw, double *bh)
{
	double worldX = l2CameraX + L2_BIKE_SCREEN_X;
	*bx = worldX + l2BikeW * 0.15;
	*by = l2BikeY + 6;
	*bw = l2BikeW * 0.70;
	*bh = l2BikeH * 0.62;
}

static void enterLevel02Win()
{
	l2State = LEVEL02_WIN;
	stopLevel02Music();
	sfxPlay(SFX_WIN);
}

static void enterLevel02GameOver(int reason)
{
	l2State      = LEVEL02_GAMEOVER;
	l2LoseReason = reason;
	stopLevel02Music();
	sfxPlay(SFX_LOSE);
}

/*  Trims a launch so its arc tops out at the ceiling instead of leaving
    the screen. Ballistics: a jump of speed v rises v*v/(2g), so the fastest
    launch allowed from here is sqrt(2 * g * headroom). Speed still decides
    the height - this only removes the overshoot.                        */
static double l2LimitLaunch(double vy)
{
	double room = L2_CEILING_Y - l2BikeY;
	double vmax;

	if (room <= 0.0) return 0.0;

	vmax = sqrt(2.0 * L2_GRAVITY * room);
	return (vy > vmax) ? vmax : vy;
}

/* ==================== UPDATE STEPS ==================== */

/*  Throttle, brake, the ramp surface, jumping and landing. */
static void updateLevel02Player()
{
	double worldX, rearX, frontX, gRear, gFront, ground, climb;

	/* ---- throttle and brake ---- */
	if (keyHeld('d') || keyHeld('D'))
		l2Speed += L2_ACCEL;
	else if (keyHeld('a') || keyHeld('A'))
		l2Speed -= L2_BRAKE;
	else if (l2Speed > 0)
		l2Speed -= L2_COAST;

	if (l2Speed > L2_MAX_SPEED) l2Speed = L2_MAX_SPEED;
	if (l2Speed < 0)            l2Speed = 0;

	l2CameraX += l2Speed;
	if (l2CameraX < 0) l2CameraX = 0;

	/* ---- where the two tyres are standing ---- */
	worldX = l2CameraX + L2_BIKE_SCREEN_X;
	rearX  = worldX + l2BikeW * L2_WHEEL_REAR;
	frontX = worldX + l2BikeW * L2_WHEEL_FRONT;

	gRear  = l2SurfaceAt(rearX);
	gFront = l2SurfaceAt(frontX);
	ground = (gRear > gFront) ? gRear : gFront;   /* rest on the higher wheel */

	climb = ground - l2PrevGround;                /* how fast it is rising   */
	l2PrevGround = ground;

	/* ---- jump ---- */
	if (l2JumpRequested && l2OnGround)
	{
		l2BikeVY   = l2LimitLaunch(L2_JUMP_VELOCITY);
		l2OnGround = false;
	}
	l2JumpRequested = false;

	if (l2OnGround)
	{
		/*  Glued to the surface: the wheels sit exactly on it, which is what
		    keeps the Honda from bouncing or floating on flat road.        */
		l2BikeY  = ground;
		l2BikeVY = 0;

		if (climb > 0.4)
			l2LastClimb = climb;                  /* remember the climb rate */

		/*  Past the crest the surface drops away. Carry the speed it was
		    climbing at into the air - that is the launch.                 */
		if (climb < -1.0 && l2Speed > 1.0 && l2LastClimb > 0.4)
		{
			/*  Speed decides the height. The climb rate is already
			    slope x speed, and this gain scales it once more, so the
			    same ramp gives a hop at walking pace and a big launch at
			    full throttle - never random, never a fixed height.      */
			double gain = L2_LAUNCH_MIN_GAIN +
			              L2_LAUNCH_SPEED_GAIN * (l2Speed / L2_MAX_SPEED);

			l2BikeVY    = l2LimitLaunch(l2LastClimb * L2_RAMP_LAUNCH * gain);
			l2OnGround  = false;
			l2LastClimb = 0;
		}
		else if (climb >= -1.0 && climb <= 0.4)
		{
			l2LastClimb *= 0.90;                  /* fades on flat ground */
		}
	}
	else
	{
		l2BikeVY -= L2_GRAVITY;

		if (keyHeld('s') || keyHeld('S'))
			l2BikeVY -= L2_FAST_FALL;             /* S drops it faster */

		l2BikeY += l2BikeVY;

		if (l2BikeY > L2_CEILING_Y)               /* never off the top */
		{
			l2BikeY  = L2_CEILING_Y;
			if (l2BikeVY > 0) l2BikeVY = 0;
		}

		if (l2BikeY <= ground)                    /* landed */
		{
			l2BikeY     = ground;
			l2BikeVY    = 0;
			l2OnGround  = true;
			l2LastClimb = 0;
		}
	}

	/* ---- lie along the slope, so both tyres touch on a ramp ---- */
	{
		double span   = l2BikeW * (L2_WHEEL_FRONT - L2_WHEEL_REAR);
		double target = 0;

		if (l2OnGround && span > 1.0)
		{
			target = atan2(gFront - gRear, span) * 180.0 / 3.14159265;
			if (target >  32.0) target =  32.0;
			if (target < -32.0) target = -32.0;
		}
		/* in the air it levels out again - never a random flip */
		l2BikeAngle += (target - l2BikeAngle) * (l2OnGround ? 0.35 : 0.08);
	}

	if (l2HitFlash > 0) l2HitFlash--;

	/* ---- safety net: the road is solid, but never let it fall through ---- */
	if (l2BikeY < L2_GROUND_Y - 220.0)
		enterLevel02GameOver(L2_LOSE_FALL);
}

/*  One dangerous collision ends the run. */
static void checkLevel02Obstacles()
{
	double bx, by, bw, bh;
	int i;

	l2BikeBox(&bx, &by, &bw, &bh);

	for (i = 0; i < L2_OBSTACLE_COUNT; i++)
	{
		L2Obstacle *o = &l2Obstacles[i];
		double ox = o->x + o->insetX;
		double oy = L2_GROUND_Y + o->insetY;
		double ow = o->w - 2 * o->insetX;
		double oh = o->h - o->insetY;

		if (rectsOverlap(bx, by, bw, bh, ox, oy, ow, oh))
		{
			if (!o->touching)             /* the moment of impact only */
			{
				o->touching = true;
				sfxPlay(SFX_COLLISION);

				if (L2_OBS_FATAL[i])
				{
					enterLevel02GameOver(L2_LOSE_CRASH);
					return;
				}

				/*  The first two barriers are survivable: they cost fuel
				    and the ride carries on. One hit, one charge - the flag
				    re-arms only once the Honda is clear of the box.      */
				l2Fuel -= L2_OBS_FUEL_PENALTY;
				l2HitFlash = L2_HIT_FLASH_TICKS;

				if (l2Fuel <= 0)
				{
					l2Fuel = 0;
					enterLevel02GameOver(L2_LOSE_FUEL);
					return;
				}
			}
		}
		else
		{
			o->touching = false;          /* separated - can be hit again */
		}

		if (!o->cleared && bx > o->x + o->w)
			o->cleared = true;            /* jumped it - counts as cleared */
	}
}

static void checkLevel02Coins()
{
	double bx, by, bw, bh;
	int i;

	l2BikeBox(&bx, &by, &bw, &bh);

	for (i = 0; i < l2CoinCount; i++)
	{
		double s;
		if (l2Coins[i].taken) continue;

		s = l2Coins[i].size;
		if (rectsOverlap(bx, by, bw, bh,
		                 l2Coins[i].x - s / 2, l2Coins[i].y - s / 2, s, s))
		{
			l2Coins[i].taken = true;      /* gone for good */
			l2Score += l2Coins[i].value;
			l2CoinsTaken++;
			sfxPlay(SFX_COIN);
		}
	}
}

static void checkLevel02Fuel()
{
	double bx, by, bw, bh;
	int i;

	l2BikeBox(&bx, &by, &bw, &bh);

	for (i = 0; i < L2_FUEL_COUNT; i++)
	{
		if (l2Fuels[i].taken) continue;

		if (rectsOverlap(bx, by, bw, bh, l2Fuels[i].x, l2Fuels[i].y, 52, 56))
		{
			l2Fuels[i].taken = true;
			l2Fuel += L2_FUEL_PICKUP;
			if (l2Fuel > L2_FUEL_MAX) l2Fuel = L2_FUEL_MAX;   /* never overfills */
			sfxPlay(SFX_FUEL);
		}
	}
}

static void updateLevel02Fuel()
{
	checkLevel02Fuel();

	if (l2Speed > 0.05)
		l2Fuel -= L2_FUEL_DRAIN;          /* burns only while riding */

	if (l2Fuel <= 0)
	{
		l2Fuel = 0;
		enterLevel02GameOver(L2_LOSE_FUEL);
	}
}

/*  Only the far end counts, and only once obstacle 5 is behind us. */
static void checkLevel02Finish()
{
	if (l2CameraX >= L2_ROUTE_END)
	{
		l2CameraX = L2_ROUTE_END;
		l2Speed   = 0;

		if (l2Obstacles[L2_OBSTACLE_COUNT - 1].cleared)
			enterLevel02Win();
	}
}

void updateLevel02()
{
	if (l2State != LEVEL02_PLAYING)
		return;

	l2Ticks++;

	updateLevel02Player();
	if (l2State != LEVEL02_PLAYING) return;

	checkLevel02Obstacles();
	if (l2State != LEVEL02_PLAYING) return;

	checkLevel02Coins();

	updateLevel02Fuel();
	if (l2State != LEVEL02_PLAYING) return;

	checkLevel02Finish();
}

/* ==================== DRAWING ==================== */

int level02BackgroundIndex()
{
	int i = (int)((l2CameraX * L2_BG_PARALLAX) / SCREEN_WIDTH);
	if (i < 0) i = 0;
	if (i > L2_BG_COUNT - 1) i = L2_BG_COUNT - 1;
	return i;
}

static void drawLevel02Backgrounds()
{
	double slide = l2CameraX * L2_BG_PARALLAX;
	int i;

	iSetColor(255, 255, 255);

	for (i = 0; i < L2_BG_COUNT; i++)
	{
		double sx = i * (double)SCREEN_WIDTH - slide;
		if (sx >= SCREEN_WIDTH) break;
		if (sx + SCREEN_WIDTH <= 0) continue;
		iShowImage((int)sx, L2_BG_Y_OFFSET, SCREEN_WIDTH, L2_BG_DRAW_HEIGHT, l2BgTex[i]);
	}
}

static void drawLevel02Road()
{
	double period = 130.0, dashW = 74.0, x, off;

	iSetColor(56, 56, 60);
	iFilledRectangle(0, 0, SCREEN_WIDTH, L2_GROUND_Y);
	iSetColor(44, 44, 48);
	iFilledRectangle(0, 0, SCREEN_WIDTH, L2_GROUND_Y * 0.42);

	iSetColor(96, 96, 102);
	iFilledRectangle(0, L2_GROUND_Y - 8, SCREEN_WIDTH, 8);
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	iFilledRectangle(0, L2_GROUND_Y - 2, SCREEN_WIDTH, 2);

	iSetColor(150, 150, 148);
	iFilledRectangle(0, L2_GROUND_Y - 22, SCREEN_WIDTH, 3);

	off = l2CameraX - floor(l2CameraX / period) * period;
	for (x = -off; x < SCREEN_WIDTH; x += period)
	{
		iSetColor(205, 205, 195);
		iFilledRectangle(x, L2_GROUND_Y * 0.40, dashW, 6);
	}
}

static unsigned int level02BikeTexture()
{
	int pose;

	/* the wreck, once the run has ended in a crash */
	if (l2State == LEVEL02_GAMEOVER && l2LoseReason == L2_LOSE_CRASH &&
	    l2Crash.count > 0)
		return l2Crash.frame[0];

	if (l2Bike.count < 1) return 0;

	if (l2Speed < 0.05 && l2OnGround)   pose = L2_POSE_STANDING;
	else if (l2Speed >= L2_FAST_SPRITE_AT) pose = L2_POSE_FAST;
	else                                pose = L2_POSE_NORMAL;

	if (pose >= l2Bike.count) pose = 0;
	return l2Bike.frame[pose];
}

static void drawLevel02Hud()
{
	char buf[64];
	double barW = 210, barH = 16;
	double progress;

	iSetColor(COL_BG_R, COL_BG_G, COL_BG_B);
	iFilledRectangle(0, SCREEN_HEIGHT - 74, SCREEN_WIDTH, 74);
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	iFilledRectangle(0, SCREEN_HEIGHT - 77, SCREEN_WIDTH, 3);

	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	drawText(26, SCREEN_HEIGHT - 38, "LEVEL 02", GLUT_BITMAP_TIMES_ROMAN_24);

	/* fuel */
	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(180, SCREEN_HEIGHT - 30, "FUEL", GLUT_BITMAP_HELVETICA_12);
	iSetColor(COL_PANEL_R, COL_PANEL_G, COL_PANEL_B);
	iFilledRectangle(220, SCREEN_HEIGHT - 34, barW, barH);
	if (l2Fuel > L2_FUEL_MAX * 0.25)
		iSetColor(60, 160, 235);
	else
		iSetColor(240, 160, 40);
	iFilledRectangle(220, SCREEN_HEIGHT - 34, barW * l2Fuel / L2_FUEL_MAX, barH);
	iSetColor(120, 120, 130);
	iRectangle(220, SCREEN_HEIGHT - 34, barW, barH);
	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	sprintf_s(buf, "%d", (int)l2Fuel);
	drawText(220 + barW + 10, SCREEN_HEIGHT - 32, buf, GLUT_BITMAP_HELVETICA_12);

	/* score + coins */
	iSetColor(255, 206, 64);
	iFilledCircle(500, SCREEN_HEIGHT - 26, 11);
	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	sprintf_s(buf, "%d", l2CoinsTaken);
	drawText(518, SCREEN_HEIGHT - 32, buf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "SCORE  %d", l2Score);
	drawText(590, SCREEN_HEIGHT - 32, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	/* route */
	progress = l2CameraX / L2_ROUTE_END;
	if (progress > 1) progress = 1;

	sprintf_s(buf, "ROUTE  %d / %d", level02BackgroundIndex() + 1, L2_BG_COUNT);
	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 28, buf, GLUT_BITMAP_HELVETICA_12);
	iSetColor(COL_PANEL_R, COL_PANEL_G, COL_PANEL_B);
	iFilledRectangle(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 48, 270, 10);
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	iFilledRectangle(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 48, 270 * progress, 10);

	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(180, SCREEN_HEIGHT - 58,
	         "D  throttle    A  brake    W  jump    S  fast fall    R  restart    ESC  menu",
	         GLUT_BITMAP_HELVETICA_12);
}

static void drawLevel02Result()
{
	char buf[80];
	double pw = 620, ph = 260;
	double px = SCREEN_WIDTH / 2 - pw / 2;
	double py = SCREEN_HEIGHT / 2 - ph / 2;
	const char *title, *line;

	if (l2State == LEVEL02_WIN)
	{
		title = "LEVEL 2 COMPLETED";
		line  = "You cleared every ramp and reached the end of the route.";
	}
	else if (l2LoseReason == L2_LOSE_FUEL)
	{
		title = "GAME OVER";
		line  = "Out of fuel - grab the OCTANE cans along the road.";
	}
	else if (l2LoseReason == L2_LOSE_FALL)
	{
		title = "GAME OVER";
		line  = "The Honda left the road.";
	}
	else
	{
		title = "GAME OVER";
		line  = "Crashed into the barrier - ride the ramp fast and jump it.";
	}

	iSetColor(COL_BG_R, COL_BG_G, COL_BG_B);
	iFilledRectangle(px, py, pw, ph);
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	drawBorder(px, py, pw, ph, 3);
	iFilledRectangle(px, py + ph - 62, pw, 62);

	iSetColor(255, 255, 255);
	drawTextCentered(SCREEN_WIDTH / 2, py + ph - 42, title, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	drawTextCentered(SCREEN_WIDTH / 2, py + ph - 104, line, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "FINAL SCORE   %d", l2Score);
	drawTextCentered(SCREEN_WIDTH / 2, py + 96, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf_s(buf, "COINS  %d        FUEL LEFT  %d", l2CoinsTaken, (int)l2Fuel);
	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawTextCentered(SCREEN_WIDTH / 2, py + 66, buf, GLUT_BITMAP_HELVETICA_12);

	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	drawTextCentered(SCREEN_WIDTH / 2, py + 28,
	                 "R  restart level 2          ESC  main menu", GLUT_BITMAP_HELVETICA_18);
}

void drawLevel02()
{
	int i;
	double sx;

	drawLevel02Backgrounds();
	drawLevel02Road();

	/* ---- ramps ---- */
	for (i = 0; i < L2_RAMP_COUNT; i++)
	{
		sx = l2Ramps[i].x - l2CameraX;
		if (sx > SCREEN_WIDTH || sx + l2Ramps[i].w < 0) continue;
		iSetColor(255, 255, 255);
		iShowImage((int)sx, (int)L2_GROUND_Y, (int)l2Ramps[i].w, (int)l2Ramps[i].h,
		           l2Ramps[i].tex);
	}

	/* ---- obstacles ---- */
	for (i = 0; i < L2_OBSTACLE_COUNT; i++)
	{
		sx = l2Obstacles[i].x - l2CameraX;
		if (sx > SCREEN_WIDTH || sx + l2Obstacles[i].w < 0) continue;
		iSetColor(255, 255, 255);
		iShowImage((int)sx, (int)L2_GROUND_Y, (int)l2Obstacles[i].w,
		           (int)l2Obstacles[i].h, l2Obstacles[i].tex);
	}

	/* ---- fuel cans ---- */
	for (i = 0; i < L2_FUEL_COUNT; i++)
	{
		if (l2Fuels[i].taken) continue;
		sx = l2Fuels[i].x - l2CameraX;
		if (sx > SCREEN_WIDTH + 60 || sx < -60) continue;
		iSetColor(255, 255, 255);
		iShowImage((int)sx, (int)(l2Fuels[i].y + sin((l2Ticks + i * 20) * 0.05) * 5.0),
		           52, 56, l2FuelTex);
	}

	/* ---- coins ---- */
	for (i = 0; i < l2CoinCount; i++)
	{
		double s;
		if (l2Coins[i].taken) continue;
		sx = l2Coins[i].x - l2CameraX;
		if (sx > SCREEN_WIDTH + 60 || sx < -60) continue;
		s = l2Coins[i].size;
		iSetColor(255, 255, 255);
		iShowImage((int)(sx - s / 2),
		           (int)(l2Coins[i].y - s / 2 + sin((l2Ticks + i * 12) * 0.06) * 4.0),
		           (int)s, (int)s, l2CoinTex[l2Coins[i].art]);
	}

	/* ---- the Honda, tilted onto the surface it is standing on ---- */
	{
		double cx = L2_BIKE_SCREEN_X + l2BikeW * 0.5;

		/* blink for a moment after a survivable hit, so the fuel loss reads */
		if (l2HitFlash == 0 || (l2HitFlash / 4) % 2 == 0)
		{
			iSetColor(255, 255, 255);
			iRotate(cx, l2BikeY, l2BikeAngle);
			iShowImage((int)L2_BIKE_SCREEN_X, (int)l2BikeY, l2BikeW, l2BikeH,
			           level02BikeTexture());
			iUnRotate();
		}
	}

	drawLevel02Hud();

	if (l2State != LEVEL02_PLAYING)
		drawLevel02Result();
}

#endif
