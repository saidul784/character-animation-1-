/*
 *  Level01.hpp  --  Honda Rush: Career Rider, LEVEL 01   (rebuilt)
 *
 *  An eight screen ride, built on the same world system as levels 02 and
 *  03: eight background panels laid end to end (BG1 -> BG8, never
 *  shuffled), a separate playable road drawn in front of them, and every
 *  ramp, obstacle, coin and fuel can placed at a fixed world position.
 *  Nothing is random; every run is the same route.
 *
 *  THE ROUTE
 *      BG1   open road - the Honda starts here, stationary
 *      BG2   Obstacle 1
 *      BG3   Obstacle 2
 *      BG4   RAMP 1, with Obstacle 3 in its landing path
 *      BG5   Obstacle 4      <- from here a hit ends the run
 *      BG6   Obstacle 2
 *      BG7   Obstacle 3
 *      BG8   RAMP 2, with Obstacle 4 in its landing path -> LEVEL COMPLETE
 *
 *  COLLISION RULE
 *      BG2, BG3, BG4  a hit only knocks the speed down - keep riding
 *      BG5 onwards    a hit is fatal
 *
 *  Everything is prefixed L1_ / l1 so it cannot collide with the level 02
 *  and level 03 code that shares this translation unit. The only names
 *  kept from the old level 1 are the four iMain.cpp already calls:
 *  lvlState, lvlPaused, lvlJumpRequested and LEVEL01_PLAYING.
 *
 *  Entry points used by iMain.cpp:
 *      level01Init()    load the artwork once, at start up
 *      level01Reset()   build and start the route
 *      level01Update()  one fixed step
 *      level01Draw()    render
 */

#ifndef LEVEL01_HPP
#define LEVEL01_HPP

/* ==================== ARTWORK ==================== */

#define L1_DIR "../Level1/"

#define L1_BG_COUNT 8

static const char *L1_BG_FILE[L1_BG_COUNT] =
{
	L1_DIR "Background_1.jpg", L1_DIR "Background_2.jpg",
	L1_DIR "Background_3.jpg", L1_DIR "Background_4.jpg",
	L1_DIR "Background_5.jpg", L1_DIR "Background_6.jpg",
	L1_DIR "Background_7.jpg", L1_DIR "Background_8.jpg"
};

static const char *L1_OBS_FILE[4] =
{
	L1_DIR "Obstacle_1.png", L1_DIR "Obstacle_2.png",
	L1_DIR "Obstacle_3.png", L1_DIR "Obstacle_4.png"
};

#define L1_BIKE_NORMAL  L1_DIR "Normal_riding.png"
#define L1_BIKE_FAST    L1_DIR "Bike fast moving (1).png"
#define L1_FUEL_ART     L1_DIR "fuel_icon.png"
#define L1_COIN_SHEET   L1_DIR "Coin.jpg"
#define L1_RAMP1_ART    L1_DIR "Ramp_1.png"
#define L1_RAMP2_ART    L1_DIR "Ramp_2.png"

/* ==================== TUNING ==================== */

#define L1_GROUND_Y          150.0   /* the road surface                  */
/*  The road covers the bottom 150 px and the HUD the top 74, so only the
    middle band of each photo is ever seen. At -45 that band was the top
    4%% to 73%% of the picture - almost all sky, which is why BG7 read as a
    white screen. Lifting the picture shows its lower two thirds instead,
    where the gates and buildings actually are.                          */
#define L1_BG_Y_OFFSET       110
#define L1_BG_PARALLAX       0.45

#define L1_BIKE_SCREEN_X     210.0   /* the Honda's column on screen      */
#define L1_BIKE_WIDTH        215     /* height follows each pose's aspect */

/*  Measured off the two supplied riding pictures - both poses put their
    wheels in the same place, so one pair of numbers fits both.          */
#define L1_WHEEL_REAR        0.21
#define L1_WHEEL_FRONT       0.845

/* riding */
#define L1_ACCEL             0.30
#define L1_BRAKE             0.45
#define L1_COAST             0.020
#define L1_MAX_SPEED        15.0
#define L1_REVERSE_MAX       3.5     /* A keeps rolling it back slowly    */
#define L1_FAST_SPRITE_AT    8.0     /* the fast picture takes over here  */

/* jumping - the same feel as level 02 */
#define L1_GRAVITY           0.70
#define L1_JUMP_VELOCITY    17.0
#define L1_FAST_FALL         0.95
#define L1_RAMP_LAUNCH       0.90
#define L1_LAUNCH_MIN_GAIN   0.30
#define L1_LAUNCH_SPEED_GAIN 1.40
#define L1_CEILING_Y       470.0     /* wheels never higher than this     */

/* fuel */
#define L1_FUEL_MAX        100.0
#define L1_FUEL_START      100.0
#define L1_FUEL_DRAIN        0.035
#define L1_FUEL_PICKUP      35.0

/* a non-fatal bump */
#define L1_BUMP_TICKS        45
#define L1_BUMP_SLOWDOWN     0.35    /* speed kept after a harmless hit   */

/* ==================== WORLD LAYOUT ==================== */

#define L1_PANEL_SPAN       (SCREEN_WIDTH / L1_BG_PARALLAX)
#define L1_PANEL(i)         ((i) * L1_PANEL_SPAN + L1_BIKE_SCREEN_X)

const double L1_ROUTE_END = (L1_BG_COUNT - 1) * (double)SCREEN_WIDTH / L1_BG_PARALLAX;

/*  BG8 exactly fills the screen once the camera reaches L1_ROUTE_END, but
    the last ramp and the final obstacle stand further into that picture.
    So the BACKGROUND stops scrolling there while the Honda keeps riding
    on across it - the same way level 03 plays out its final screen. The
    camera runs on to L1_CAM_END, which is past the final obstacle.     */
const double L1_CAM_END = L1_ROUTE_END + 1750.0;

#define L1_RAMP_COUNT      2
#define L1_OBSTACLE_COUNT  7         /* BG2..BG8, one each */
#define L1_FUEL_COUNT      4
#define L1_MAX_COINS     220
#define L1_PROFILE_N      72

/* ==================== TYPES ==================== */

struct L1Ramp
{
	unsigned int tex;
	double x, w, h;
	double prof[L1_PROFILE_N];       /* top edge, above the road */
};

struct L1Obstacle
{
	double x, w, h;
	double insetX, insetY;
	int    art;                      /* 0..3 -> Obstacle_1..4 */
	bool   fatal;                    /* BG5 onwards            */
	bool   touching;                 /* one bump per contact   */
	bool   passed;
};

struct L1Coin
{
	double x, y, size;
	int    value, art;
	bool   taken;
};

struct L1Fuel
{
	double x, y;
	bool   taken;
};

/* ==================== STATE ==================== */

#define LEVEL01_PLAYING   0
#define LEVEL01_WIN       1
#define LEVEL01_GAMEOVER  2

#define L1_LOSE_NONE      0
#define L1_LOSE_FUEL      1
#define L1_LOSE_CRASH     2

int  lvlState;                       /* iMain.cpp reads these three names */
bool lvlPaused;
bool lvlJumpRequested;
int  l1LoseReason;

unsigned int l1BgTex[L1_BG_COUNT] = { 0 };
SpriteSheet  l1ObsArt[4];        /* cropped to the artwork, so they sit on the road */
unsigned int l1CoinTex[5] = { 0 };
int          l1CoinArtCount = 0;
SpriteSheet  l1BikeNormal, l1BikeFast, l1FuelArt;

L1Ramp     l1Ramps[L1_RAMP_COUNT];
L1Obstacle l1Obstacles[L1_OBSTACLE_COUNT];
L1Coin     l1Coins[L1_MAX_COINS];
L1Fuel     l1Fuels[L1_FUEL_COUNT];
int        l1CoinCount;

double l1CameraX, l1Speed;
double l1BikeY, l1BikeVY, l1BikeAngle;
double l1PrevGround, l1LastClimb;
int    l1BikeW, l1BikeH, l1FastW, l1FastH;
bool   l1OnGround;
int    l1BumpTicks;

int    l1Score, l1CoinsTaken, l1Ticks;
double l1Fuel;

static const int L1_COIN_VALUE[5] = { 5, 10, 30, 50, 100 };

/*  Which background each obstacle stands in, and which picture it uses:
    BG2->O1, BG3->O2, BG4->O3, BG5->O4, BG6->O2, BG7->O3, BG8->O4.
    Panel 4 is BG5, so everything from there on is deadly.              */
static const int L1_OBS_PANEL[L1_OBSTACLE_COUNT] = { 1, 2, 3, 4, 5, 6, 7 };
static const int L1_OBS_ART  [L1_OBSTACLE_COUNT] = { 0, 1, 2, 3, 1, 2, 3 };

/*  How tall each obstacle stands on the road. The width is worked out
    from the picture's own shape once it is loaded, so nothing is ever
    stretched.                                                          */
static const double L1_OBS_TARGET_H[4] = { 158, 142, 152, 152 };
double l1ObsW[4], l1ObsH[4];

/* ==================== MUSIC ==================== */

void playLevel01Music()
{
	musicPlay(MUSIC_LEVEL01);        /* ../Level1/Game_music.mp3 */
}

void stopLevel01Music()
{
	musicStop();
}

/* ==================== LOADING ==================== */

/*  Ramp artwork plus the surface its wheels will ride: the height of the
    top edge is measured column by column out of the alpha channel, so the
    Honda climbs exactly the shape that is drawn.                        */
static bool l1LoadRamp(L1Ramp *r, const char *path, double drawW)
{
	int w, h, bpp, i, x, y, col, top;
	int minX, minY, maxX, maxY, cw, ch;
	unsigned char *data, *crop;

	r->tex = 0;
	r->w = drawW;
	r->h = drawW;
	for (i = 0; i < L1_PROFILE_N; i++) r->prof[i] = 0;

	data = stbi_load(path, &w, &h, &bpp, 4);
	if (data == NULL)
	{
		printf("[level01] FAILED to load ramp: %s\n", path);
		return false;
	}

	/*  The supplied pictures have empty space around the structure. Drawn
	    as they are, the ramp would hang in the air above the road, so the
	    artwork is cropped to its visible pixels first - then the bottom of
	    the picture IS the bottom of the ramp, and it stands on the road. */
	minX = w; minY = h; maxX = -1; maxY = -1;
	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			if (data[(y * w + x) * 4 + 3] > 20)
			{
				if (x < minX) minX = x;
				if (x > maxX) maxX = x;
				if (y < minY) minY = y;
				if (y > maxY) maxY = y;
			}

	if (maxX < minX || maxY < minY)
	{
		minX = 0; minY = 0; maxX = w - 1; maxY = h - 1;
	}

	cw = maxX - minX + 1;
	ch = maxY - minY + 1;

	crop = (unsigned char *)malloc(cw * ch * 4);
	if (crop == NULL) { stbi_image_free(data); return false; }

	for (y = 0; y < ch; y++)
		memcpy(crop + y * cw * 4, data + (((minY + y) * w) + minX) * 4, cw * 4);

	r->h = drawW * ch / (double)cw;

	/* the surface the wheels ride, measured off the cropped picture */
	for (i = 0; i < L1_PROFILE_N; i++)
	{
		col = (int)((i + 0.5) * cw / L1_PROFILE_N);
		if (col < 0)   col = 0;
		if (col >= cw) col = cw - 1;

		top = -1;
		for (y = 0; y < ch; y++)
			if (crop[(y * cw + col) * 4 + 3] > 60) { top = y; break; }

		r->prof[i] = (top < 0) ? 0.0 : (ch - top) * r->h / (double)ch;
	}

	glGenTextures(1, &r->tex);
	glBindTexture(GL_TEXTURE_2D, r->tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cw, ch, 0, GL_RGBA, GL_UNSIGNED_BYTE, crop);

	printf("[level01] ramp %s cropped %dx%d -> drawn %.0fx%.0f\n", path, cw, ch, r->w, r->h);

	free(crop);
	stbi_image_free(data);
	return true;
}

/*  The supplied coin sheet holds five coins of different sizes on a flat
    background, so it cannot be cut into equal columns. The background is
    keyed away first, then the coins are found by looking for the empty
    columns between them.                                                */
static int l1LoadCoinSheet(unsigned int *out, int maxN, const char *path)
{
	int w, h, bpp, x, y, n = 0;
	int runStart = -1;
	unsigned char *data;

	data = stbi_load(path, &w, &h, &bpp, 4);
	if (data == NULL)
	{
		printf("[level01] FAILED to load coins: %s\n", path);
		return 0;
	}

	spriteKeyOutFrame(data, w, 0, 0, w, h);

	for (x = 0; x <= w; x++)
	{
		bool solid = false;

		if (x < w)
			for (y = 0; y < h; y++)
				if (data[(y * w + x) * 4 + 3] > 60) { solid = true; break; }

		if (solid && runStart < 0)
		{
			runStart = x;
		}
		else if (!solid && runStart >= 0)
		{
			int cw = x - runStart, ch, y0 = -1, y1 = -1, r;

			for (y = 0; y < h; y++)
			{
				bool any = false;
				for (r = runStart; r < x; r++)
					if (data[(y * w + r) * 4 + 3] > 60) { any = true; break; }
				if (any) { if (y0 < 0) y0 = y; y1 = y; }
			}

			ch = (y0 < 0) ? 0 : (y1 - y0 + 1);

			if (cw > 20 && ch > 20 && n < maxN)
			{
				unsigned int tex;
				unsigned char *buf = (unsigned char *)malloc(cw * ch * 4);

				if (buf != NULL)
				{
					for (y = 0; y < ch; y++)
						memcpy(buf + y * cw * 4,
						       data + (((y0 + y) * w) + runStart) * 4, cw * 4);

					glGenTextures(1, &tex);
					glBindTexture(GL_TEXTURE_2D, tex);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cw, ch, 0,
					             GL_RGBA, GL_UNSIGNED_BYTE, buf);
					out[n++] = tex;
					free(buf);
				}
			}
			runStart = -1;
		}
	}

	stbi_image_free(data);
	printf("[level01] coin sheet: %d coins found\n", n);
	return n;
}

void level01Init()
{
	int i;

	{
		int maxTex = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTex);
		printf("[level01] GL max texture size = %d\n", maxTex);
	}

	for (i = 0; i < L1_BG_COUNT; i++)
	{
		int bw, bh, bbpp;
		unsigned char *probe = stbi_load(L1_BG_FILE[i], &bw, &bh, &bbpp, 4);
		l1BgTex[i] = loadImage(L1_BG_FILE[i]);
		printf("[level01] BG%d %-28s %s  tex=%u  %dx%d\n", i + 1, L1_BG_FILE[i],
		       probe ? "decoded" : "DECODE FAILED", l1BgTex[i],
		       probe ? bw : 0, probe ? bh : 0);
		if (probe) stbi_image_free(probe);
	}

	/*  Cropped to their own artwork so they stand on the road instead of
	    floating above it, and sized from the picture's real shape.      */
	for (i = 0; i < 4; i++)
	{
		loadSpriteSheet(&l1ObsArt[i], L1_OBS_FILE[i], 1, 1);
		l1ObsH[i] = L1_OBS_TARGET_H[i];
		l1ObsW[i] = (l1ObsArt[i].frameH > 0)
		          ? l1ObsH[i] * l1ObsArt[i].frameW / (double)l1ObsArt[i].frameH
		          : l1ObsH[i];
		printf("[level01] obstacle %d drawn %.0f x %.0f\n", i + 1, l1ObsW[i], l1ObsH[i]);
	}
	fflush(stdout);

	/*  The two riding pictures are different sizes, so each is cropped to
	    its own artwork and drawn at the same width - both put their wheels
	    on the same line.                                                 */
	loadSpriteSheet(&l1BikeNormal, L1_BIKE_NORMAL, 1, 1);
	loadSpriteSheet(&l1BikeFast,   L1_BIKE_FAST,   1, 1);
	loadSpriteSheet(&l1FuelArt,    L1_FUEL_ART,    1, 1);
	printf("[level01] bike+fuel art ok\n"); fflush(stdout);

	l1BikeW = L1_BIKE_WIDTH;
	l1BikeH = (l1BikeNormal.frameW > 0)
	        ? (int)((double)L1_BIKE_WIDTH * l1BikeNormal.frameH / l1BikeNormal.frameW + 0.5)
	        : 160;

	l1FastW = L1_BIKE_WIDTH;
	l1FastH = (l1BikeFast.frameW > 0)
	        ? (int)((double)L1_BIKE_WIDTH * l1BikeFast.frameH / l1BikeFast.frameW + 0.5)
	        : l1BikeH;

	l1CoinArtCount = l1LoadCoinSheet(l1CoinTex, 5, L1_COIN_SHEET);
	printf("[level01] coin sheet ok\n"); fflush(stdout);

	l1LoadRamp(&l1Ramps[0], L1_RAMP1_ART, 470);   /* BG4 */
	l1LoadRamp(&l1Ramps[1], L1_RAMP2_ART, 470);   /* BG8 */
	printf("[level01] ramps ok\n"); fflush(stdout);
}

/* ==================== THE ROAD SURFACE ==================== */

/*  The backgrounds stop at BG8; everything else keeps using l1CameraX. */
static double l1BgCam()
{
	return (l1CameraX > L1_ROUTE_END) ? L1_ROUTE_END : l1CameraX;
}

double l1SurfaceAt(double worldX)
{
	double best = L1_GROUND_Y;
	int i;

	for (i = 0; i < L1_RAMP_COUNT; i++)
	{
		L1Ramp *r = &l1Ramps[i];
		double t, frac, a, b, hgt;
		int k;

		if (r->tex == 0) continue;
		if (worldX < r->x || worldX > r->x + r->w) continue;

		t = (worldX - r->x) / r->w * (L1_PROFILE_N - 1);
		k = (int)t;
		if (k < 0) k = 0;
		if (k > L1_PROFILE_N - 2) k = L1_PROFILE_N - 2;
		frac = t - k;

		a = r->prof[k];
		b = r->prof[k + 1];
		hgt = L1_GROUND_Y + a + (b - a) * frac;
		if (hgt > best) best = hgt;
	}
	return best;
}

/* ==================== BUILDING THE ROUTE ==================== */

static void l1AddCoin(double x, double y, int art)
{
	if (l1CoinCount >= L1_MAX_COINS) return;
	if (art < 0) art = 0;
	if (art > 4) art = 4;

	l1Coins[l1CoinCount].x     = x;
	l1Coins[l1CoinCount].y     = y;
	l1Coins[l1CoinCount].art   = art;
	l1Coins[l1CoinCount].value = L1_COIN_VALUE[art];
	l1Coins[l1CoinCount].size  = 34.0 + art * 5.0;
	l1Coins[l1CoinCount].taken = false;
	l1CoinCount++;
}

static void l1AddCoinRow(double x, double gap, int n, int art, double y)
{
	int i;
	for (i = 0; i < n; i++)
		l1AddCoin(x + i * gap, y, art);
}

static void l1AddCoinArc(double centreX, double spread, int art, double peak)
{
	int i;
	for (i = 0; i < 5; i++)
	{
		double t = (i - 2) / 2.0;
		l1AddCoin(centreX + t * spread,
		          L1_GROUND_Y + peak - t * t * (peak * 0.45), art);
	}
}

/*  The fixed layout. Ramp 1 stands in BG4 and ramp 2 in BG8, each with its
    obstacle just past the end of the ramp, in the landing path.         */
static void buildLevel01Track()
{
	int i;
	double rampX[L1_RAMP_COUNT];

	l1CoinCount = 0;

	rampX[0] = L1_PANEL(3) + 900.0;      /* BG4 */
	rampX[1] = L1_PANEL(7) + 700.0;      /* BG8 */

	for (i = 0; i < L1_RAMP_COUNT; i++)
		l1Ramps[i].x = rampX[i];

	for (i = 0; i < L1_OBSTACLE_COUNT; i++)
	{
		L1Obstacle *o = &l1Obstacles[i];
		int panel = L1_OBS_PANEL[i];

		o->art    = L1_OBS_ART[i];
		o->w      = l1ObsW[o->art];
		o->h      = l1ObsH[o->art];
		o->insetX = o->w * 0.16;
		o->insetY = o->h * 0.14;
		o->fatal  = (panel >= 4);        /* BG5 onwards */
		o->touching = false;
		o->passed   = false;

		if (panel == 3)                  /* BG4 - ramp 1's landing path */
			o->x = rampX[0] + l1Ramps[0].w + 70.0;
		else if (panel == 7)             /* BG8 - ramp 2's landing path */
			o->x = rampX[1] + l1Ramps[1].w + 70.0;
		else
			o->x = L1_PANEL(panel) + 1350.0;
	}

	/* ---- coins: rows along the road, arcs over the two jumps ---- */
	l1AddCoinRow(L1_PANEL(0) +  700, 70, 6, 0, L1_GROUND_Y + 55);
	l1AddCoinRow(L1_PANEL(0) + 1700, 70, 6, 0, L1_GROUND_Y + 100);
	l1AddCoinRow(L1_PANEL(1) +  400, 70, 5, 0, L1_GROUND_Y + 55);
	l1AddCoinRow(L1_PANEL(1) + 2000, 70, 5, 1, L1_GROUND_Y + 95);
	l1AddCoinRow(L1_PANEL(2) +  500, 70, 6, 1, L1_GROUND_Y + 55);
	l1AddCoinRow(L1_PANEL(2) + 2000, 70, 5, 1, L1_GROUND_Y + 100);

	l1AddCoinRow(L1_PANEL(4) +  600, 75, 5, 2, L1_GROUND_Y + 60);
	l1AddCoinRow(L1_PANEL(4) + 2000, 75, 4, 2, L1_GROUND_Y + 105);
	l1AddCoinRow(L1_PANEL(5) +  500, 75, 5, 2, L1_GROUND_Y + 60);
	l1AddCoinRow(L1_PANEL(6) +  500, 80, 4, 3, L1_GROUND_Y + 60);
	l1AddCoinRow(L1_PANEL(7) + 1900, 85, 4, 3, L1_GROUND_Y + 60);

	/*  An arc of coins riding over the top of every obstacle - the reward
	    for jumping it cleanly rather than crawling into it. The arc clears
	    the obstacle itself, so the coins are only reachable in the air.  */
	for (i = 0; i < L1_OBSTACLE_COUNT; i++)
	{
		L1Obstacle *o = &l1Obstacles[i];
		double peak = o->h + 95.0;
		int art = (i < 2) ? 1 : (i < 5) ? 2 : 4;
		l1AddCoinArc(o->x + o->w * 0.5, o->w * 0.60 + 130.0, art, peak);
	}

	/*  ---- four fuel cans ----
	    Deliberately parked in the clear stretches, well away from any
	    obstacle, so picking one up is never tangled with a jump.       */
	l1Fuels[0].x = L1_PANEL(0) + 2250;      /* BG1, no obstacles at all */
	l1Fuels[1].x = L1_PANEL(2) + 2350;      /* BG3, long after its obstacle */
	l1Fuels[2].x = L1_PANEL(4) + 2350;      /* BG5, long after its obstacle */
	l1Fuels[3].x = L1_PANEL(6) + 2400;      /* BG7, long after its obstacle */

	for (i = 0; i < L1_FUEL_COUNT; i++)
	{
		l1Fuels[i].y     = L1_GROUND_Y + 30;
		l1Fuels[i].taken = false;
	}
}

void level01Reset()
{
	l1CameraX   = 0;
	l1Speed     = 0;                  /* starts stationary */
	l1BikeY     = L1_GROUND_Y;
	l1BikeVY    = 0;
	l1BikeAngle = 0;
	l1OnGround  = true;
	l1PrevGround = L1_GROUND_Y;
	l1LastClimb  = 0;
	l1BumpTicks  = 0;

	l1Score      = 0;
	l1CoinsTaken = 0;
	l1Fuel       = L1_FUEL_START;
	l1Ticks      = 0;

	lvlState     = LEVEL01_PLAYING;
	lvlPaused    = false;
	lvlJumpRequested = false;
	l1LoseReason = L1_LOSE_NONE;

	buildLevel01Track();
}

/* ==================== COLLISION HELPERS ==================== */

static void l1BikeBox(double *bx, double *by, double *bw, double *bh)
{
	double worldX = l1CameraX + L1_BIKE_SCREEN_X;
	*bx = worldX + l1BikeW * 0.16;
	*by = l1BikeY + 6;
	*bw = l1BikeW * 0.68;
	*bh = l1BikeH * 0.62;
}

static void enterLevel01Win()
{
	if (lvlState != LEVEL01_PLAYING) return;
	lvlState = LEVEL01_WIN;
	stopLevel01Music();
	sfxPlay(SFX_WIN);
}

static void enterLevel01GameOver(int reason)
{
	if (lvlState != LEVEL01_PLAYING) return;
	lvlState     = LEVEL01_GAMEOVER;
	l1LoseReason = reason;
	stopLevel01Music();
	sfxPlay(SFX_LOSE);
}

/*  Trims a launch so the arc tops out at the ceiling instead of leaving
    the screen - the same rule level 02 uses.                            */
static double l1LimitLaunch(double vy)
{
	double room = L1_CEILING_Y - l1BikeY;
	double vmax;

	if (room <= 0.0) return 0.0;
	vmax = sqrt(2.0 * L1_GRAVITY * room);
	return (vy > vmax) ? vmax : vy;
}

/* ==================== UPDATE ==================== */

static void updateLevel01Player()
{
	double worldX, rearX, frontX, gRear, gFront, ground, climb;

	/* ---- D throttle, A brake and reverse ---- */
	if (keyHeld('d') || keyHeld('D'))
		l1Speed += L1_ACCEL;
	else if (keyHeld('a') || keyHeld('A'))
		l1Speed -= L1_BRAKE;
	else if (l1Speed > 0)
		l1Speed -= L1_COAST;
	else if (l1Speed < 0)
		l1Speed += L1_COAST;

	if (l1Speed >  L1_MAX_SPEED)   l1Speed =  L1_MAX_SPEED;
	if (l1Speed < -L1_REVERSE_MAX) l1Speed = -L1_REVERSE_MAX;

	l1CameraX += l1Speed;
	if (l1CameraX < 0) l1CameraX = 0;

	/* ---- the two wheels, and the surface under them ---- */
	worldX = l1CameraX + L1_BIKE_SCREEN_X;
	rearX  = worldX + l1BikeW * L1_WHEEL_REAR;
	frontX = worldX + l1BikeW * L1_WHEEL_FRONT;

	gRear  = l1SurfaceAt(rearX);
	gFront = l1SurfaceAt(frontX);
	ground = (gRear > gFront) ? gRear : gFront;

	climb = ground - l1PrevGround;
	l1PrevGround = ground;

	/* ---- W jump ---- */
	if (lvlJumpRequested && l1OnGround)
	{
		l1BikeVY   = l1LimitLaunch(L1_JUMP_VELOCITY);
		l1OnGround = false;
	}
	lvlJumpRequested = false;

	if (l1OnGround)
	{
		/* glued to the surface - no bouncing, no floating */
		l1BikeY  = ground;
		l1BikeVY = 0;

		if (climb > 0.4)
			l1LastClimb = climb;

		/*  Over the crest the ramp throws the Honda, and how hard depends
		    on how fast it arrived.                                      */
		if (climb < -1.0 && l1Speed > 2.5 && l1LastClimb > 0.6)
		{
			double gain = L1_LAUNCH_MIN_GAIN +
			              L1_LAUNCH_SPEED_GAIN * (l1Speed / L1_MAX_SPEED);
			l1BikeVY    = l1LimitLaunch(l1LastClimb * L1_RAMP_LAUNCH * gain);
			l1OnGround  = false;
			l1LastClimb = 0;
		}
		else if (climb >= -1.0 && climb <= 0.4)
		{
			l1LastClimb *= 0.90;
		}
	}
	else
	{
		l1BikeVY -= L1_GRAVITY;

		if (keyHeld('s') || keyHeld('S'))
			l1BikeVY -= L1_FAST_FALL;        /* S drops it faster */

		l1BikeY += l1BikeVY;

		if (l1BikeY > L1_CEILING_Y)
		{
			l1BikeY = L1_CEILING_Y;
			if (l1BikeVY > 0) l1BikeVY = 0;
		}

		if (l1BikeY <= ground)
		{
			l1BikeY     = ground;
			l1BikeVY    = 0;
			l1OnGround  = true;
			l1LastClimb = 0;
		}
	}

	/* ---- lie along the slope so both wheels stay down ---- */
	{
		double span   = l1BikeW * (L1_WHEEL_FRONT - L1_WHEEL_REAR);
		double target = 0;

		if (l1OnGround && span > 1.0)
		{
			target = atan2(gFront - gRear, span) * 180.0 / 3.14159265;
			if (target >  32.0) target =  32.0;
			if (target < -32.0) target = -32.0;
		}
		l1BikeAngle += (target - l1BikeAngle) * (l1OnGround ? 0.35 : 0.08);
	}

	if (l1BumpTicks > 0) l1BumpTicks--;
}

/*  BG2..BG4 only knock the speed down; from BG5 a hit ends the run.     */
static void checkLevel01Obstacles()
{
	double bx, by, bw, bh;
	int i;

	l1BikeBox(&bx, &by, &bw, &bh);

	for (i = 0; i < L1_OBSTACLE_COUNT; i++)
	{
		L1Obstacle *o = &l1Obstacles[i];
		double ox = o->x + o->insetX;
		double oy = L1_GROUND_Y + o->insetY;
		double ow = o->w - 2 * o->insetX;
		double oh = o->h - o->insetY;

		if (rectsOverlap(bx, by, bw, bh, ox, oy, ow, oh))
		{
			if (o->fatal)
			{
				sfxPlay(SFX_COLLISION);
				enterLevel01GameOver(L1_LOSE_CRASH);
				return;
			}

			if (!o->touching)            /* one bump per contact */
			{
				o->touching = true;
				l1Speed *= L1_BUMP_SLOWDOWN;
				l1BumpTicks = L1_BUMP_TICKS;
				sfxPlay(SFX_COLLISION);
			}
		}
		else
		{
			o->touching = false;
		}

		if (!o->passed && bx > o->x + o->w)
			o->passed = true;
	}
}

static void checkLevel01Coins()
{
	double bx, by, bw, bh;
	int i;

	l1BikeBox(&bx, &by, &bw, &bh);

	for (i = 0; i < l1CoinCount; i++)
	{
		double s;
		if (l1Coins[i].taken) continue;

		s = l1Coins[i].size;
		if (rectsOverlap(bx, by, bw, bh,
		                 l1Coins[i].x - s / 2, l1Coins[i].y - s / 2, s, s))
		{
			l1Coins[i].taken = true;
			l1Score += l1Coins[i].value;
			l1CoinsTaken++;
			sfxPlay(SFX_COIN);
		}
	}
}

static void updateLevel01Fuel()
{
	double bx, by, bw, bh;
	int i;

	l1BikeBox(&bx, &by, &bw, &bh);

	for (i = 0; i < L1_FUEL_COUNT; i++)
	{
		if (l1Fuels[i].taken) continue;

		if (rectsOverlap(bx, by, bw, bh, l1Fuels[i].x, l1Fuels[i].y, 52, 56))
		{
			l1Fuels[i].taken = true;
			l1Fuel += L1_FUEL_PICKUP;
			if (l1Fuel > L1_FUEL_MAX) l1Fuel = L1_FUEL_MAX;
			sfxPlay(SFX_FUEL);
		}
	}

	if (l1Speed > 0.05 || l1Speed < -0.05)
		l1Fuel -= L1_FUEL_DRAIN;

	if (l1Fuel <= 0)
	{
		l1Fuel = 0;
		enterLevel01GameOver(L1_LOSE_FUEL);
	}
}

/*  BG8 is the last background: clearing its obstacle and reaching the end
    of the route finishes the level.                                     */
static void checkLevel01Finish()
{
	if (l1CameraX >= L1_CAM_END)
	{
		l1CameraX = L1_CAM_END;
		if (l1Speed > 0) l1Speed = 0;

		if (l1Obstacles[L1_OBSTACLE_COUNT - 1].passed)
			enterLevel01Win();
	}
}

void level01Update()
{
	if (lvlState != LEVEL01_PLAYING || lvlPaused)
		return;

	l1Ticks++;

	updateLevel01Player();

	checkLevel01Obstacles();
	if (lvlState != LEVEL01_PLAYING) return;

	checkLevel01Coins();

	updateLevel01Fuel();
	if (lvlState != LEVEL01_PLAYING) return;

	checkLevel01Finish();
}

/* ==================== DRAWING ==================== */

int level01BackgroundIndex()
{
	int i = (int)((l1BgCam() * L1_BG_PARALLAX) / SCREEN_WIDTH);
	if (i < 0) i = 0;
	if (i > L1_BG_COUNT - 1) i = L1_BG_COUNT - 1;
	return i;
}

static void l1DrawBackgrounds()
{
	double slide = l1BgCam() * L1_BG_PARALLAX;
	int i;

	iSetColor(255, 255, 255);
	for (i = 0; i < L1_BG_COUNT; i++)
	{
		double sx = i * (double)SCREEN_WIDTH - slide;
		if (sx >= SCREEN_WIDTH)     continue;
		if (sx + SCREEN_WIDTH <= 0) continue;
		iShowImage((int)sx, L1_BG_Y_OFFSET, SCREEN_WIDTH, SCREEN_HEIGHT, l1BgTex[i]);
	}
}

static void l1DrawRoad()
{
	double period = 130.0, dashW = 74.0, x, off;

	iSetColor(56, 56, 60);
	iFilledRectangle(0, 0, SCREEN_WIDTH, L1_GROUND_Y);
	iSetColor(44, 44, 48);
	iFilledRectangle(0, 0, SCREEN_WIDTH, L1_GROUND_Y * 0.42);

	iSetColor(96, 96, 102);
	iFilledRectangle(0, L1_GROUND_Y - 8, SCREEN_WIDTH, 8);
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	iFilledRectangle(0, L1_GROUND_Y - 2, SCREEN_WIDTH, 2);

	iSetColor(150, 150, 148);
	iFilledRectangle(0, L1_GROUND_Y - 22, SCREEN_WIDTH, 3);

	off = l1CameraX - floor(l1CameraX / period) * period;
	for (x = -off; x < SCREEN_WIDTH; x += period)
	{
		iSetColor(205, 205, 195);
		iFilledRectangle(x, L1_GROUND_Y * 0.40, dashW, 6);
	}
}

static void l1DrawHud()
{
	char buf[64];
	double barW = 210, barH = 16;
	double progress;

	iSetColor(COL_BG_R, COL_BG_G, COL_BG_B);
	iFilledRectangle(0, SCREEN_HEIGHT - 74, SCREEN_WIDTH, 74);
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	iFilledRectangle(0, SCREEN_HEIGHT - 77, SCREEN_WIDTH, 3);

	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	drawText(26, SCREEN_HEIGHT - 38, "LEVEL 01", GLUT_BITMAP_TIMES_ROMAN_24);

	/* fuel */
	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(180, SCREEN_HEIGHT - 30, "FUEL", GLUT_BITMAP_HELVETICA_12);
	iSetColor(COL_PANEL_R, COL_PANEL_G, COL_PANEL_B);
	iFilledRectangle(220, SCREEN_HEIGHT - 34, barW, barH);
	if (l1Fuel > L1_FUEL_MAX * 0.25) iSetColor(60, 160, 235);
	else                             iSetColor(240, 160, 40);
	iFilledRectangle(220, SCREEN_HEIGHT - 34, barW * l1Fuel / L1_FUEL_MAX, barH);
	iSetColor(120, 120, 130);
	iRectangle(220, SCREEN_HEIGHT - 34, barW, barH);
	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	sprintf_s(buf, "%d", (int)l1Fuel);
	drawText(220 + barW + 10, SCREEN_HEIGHT - 32, buf, GLUT_BITMAP_HELVETICA_12);

	/* coins + score */
	iSetColor(255, 206, 64);
	iFilledCircle(500, SCREEN_HEIGHT - 26, 11);
	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	sprintf_s(buf, "%d", l1CoinsTaken);
	drawText(518, SCREEN_HEIGHT - 32, buf, GLUT_BITMAP_HELVETICA_18);

	sprintf_s(buf, "SCORE  %d", l1Score);
	drawText(590, SCREEN_HEIGHT - 32, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	/* route */
	progress = l1CameraX / L1_CAM_END;
	if (progress > 1) progress = 1;

	sprintf_s(buf, "ROUTE  %d / %d", level01BackgroundIndex() + 1, L1_BG_COUNT);
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

static void l1DrawResult()
{
	char buf[80];
	double pw = 620, ph = 260;
	double px = SCREEN_WIDTH / 2 - pw / 2;
	double py = SCREEN_HEIGHT / 2 - ph / 2;
	const char *title, *line;

	if (lvlState == LEVEL01_WIN)
	{
		title = "LEVEL 1 COMPLETED";
		line  = "You cleared the final ramp and reached the end of the route.";
	}
	else if (l1LoseReason == L1_LOSE_FUEL)
	{
		title = "GAME OVER";
		line  = "Out of fuel - grab the OCTANE cans along the road.";
	}
	else
	{
		title = "GAME OVER";
		line  = "Crashed into the obstacle - use the ramp and jump it.";
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

	sprintf_s(buf, "FINAL SCORE   %d", l1Score);
	drawTextCentered(SCREEN_WIDTH / 2, py + 96, buf, GLUT_BITMAP_TIMES_ROMAN_24);

	sprintf_s(buf, "COINS  %d        FUEL LEFT  %d", l1CoinsTaken, (int)l1Fuel);
	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawTextCentered(SCREEN_WIDTH / 2, py + 66, buf, GLUT_BITMAP_HELVETICA_12);

	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	drawTextCentered(SCREEN_WIDTH / 2, py + 28,
	                 "R  ride again          ESC  main menu", GLUT_BITMAP_HELVETICA_18);
}

void level01Draw()
{
	int i;
	double sx;

	l1DrawBackgrounds();
	l1DrawRoad();

	/* ---- ramps ---- */
	for (i = 0; i < L1_RAMP_COUNT; i++)
	{
		sx = l1Ramps[i].x - l1CameraX;
		if (sx > SCREEN_WIDTH || sx + l1Ramps[i].w < 0) continue;
		iSetColor(255, 255, 255);
		iShowImage((int)sx, (int)L1_GROUND_Y, (int)l1Ramps[i].w, (int)l1Ramps[i].h,
		           l1Ramps[i].tex);
	}

	/* ---- obstacles ---- */
	for (i = 0; i < L1_OBSTACLE_COUNT; i++)
	{
		L1Obstacle *o = &l1Obstacles[i];
		sx = o->x - l1CameraX;
		if (sx > SCREEN_WIDTH || sx + o->w < 0) continue;
		iSetColor(255, 255, 255);
		if (l1ObsArt[o->art].count < 1) continue;
		iShowImage((int)sx, (int)L1_GROUND_Y, (int)o->w, (int)o->h,
		           l1ObsArt[o->art].frame[0]);
	}

	/* ---- fuel cans ---- */
	for (i = 0; i < L1_FUEL_COUNT; i++)
	{
		if (l1Fuels[i].taken) continue;
		if (l1FuelArt.count < 1) continue;
		sx = l1Fuels[i].x - l1CameraX;
		if (sx > SCREEN_WIDTH + 60 || sx < -60) continue;
		iSetColor(255, 255, 255);
		iShowImage((int)sx, (int)(l1Fuels[i].y + sin((l1Ticks + i * 20) * 0.05) * 5.0),
		           52, 56, l1FuelArt.frame[0]);
	}

	/* ---- coins ---- */
	for (i = 0; i < l1CoinCount; i++)
	{
		double s;
		if (l1Coins[i].taken) continue;
		if (l1Coins[i].art >= l1CoinArtCount) continue;
		sx = l1Coins[i].x - l1CameraX;
		if (sx > SCREEN_WIDTH + 60 || sx < -60) continue;
		s = l1Coins[i].size;
		iSetColor(255, 255, 255);
		iShowImage((int)(sx - s / 2),
		           (int)(l1Coins[i].y - s / 2 + sin((l1Ticks + i * 12) * 0.06) * 4.0),
		           (int)s, (int)s, l1CoinTex[l1Coins[i].art]);
	}

	/* ---- the Honda: the normal picture, or the fast one at speed ---- */
	{
		bool fast = (l1Speed >= L1_FAST_SPRITE_AT) && (l1BikeFast.count > 0);
		SpriteSheet *pose = fast ? &l1BikeFast : &l1BikeNormal;
		int dw = fast ? l1FastW : l1BikeW;
		int dh = fast ? l1FastH : l1BikeH;
		bool blink = (l1BumpTicks > 0 && (l1BumpTicks / 4) % 2 == 1);

		if (pose->count > 0 && !blink)
		{
			double cx = L1_BIKE_SCREEN_X + dw * 0.5;
			iSetColor(255, 255, 255);
			iRotate(cx, l1BikeY, l1BikeAngle);
			iShowImage((int)L1_BIKE_SCREEN_X, (int)l1BikeY, dw, dh, pose->frame[0]);
			iUnRotate();
		}
	}

	l1DrawHud();

	if (lvlState != LEVEL01_PLAYING)
	{
		l1DrawResult();
	}
	else if (lvlPaused)
	{
		double pw = 460, ph = 150;
		double px = SCREEN_WIDTH / 2 - pw / 2;
		double py = SCREEN_HEIGHT / 2 - ph / 2;

		iSetColor(COL_BG_R, COL_BG_G, COL_BG_B);
		iFilledRectangle(px, py, pw, ph);
		iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
		drawBorder(px, py, pw, ph, 3);
		iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
		drawTextCentered(SCREEN_WIDTH / 2, py + 92, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);
		drawTextCentered(SCREEN_WIDTH / 2, py + 46,
		                 "P  resume        R  restart        ESC  main menu",
		                 GLUT_BITMAP_HELVETICA_12);
	}
}

#endif
