/*
 *  MenuScreen.hpp  --  the new intro poster + the main menu
 *
 *  STATE_INTRO : the new Honda Rush poster (Level 3 folder) fills the
 *                window height; the only control on it is MENU, in the
 *                poster's lower-right corner.
 *  STATE_MENU  : five neon bars in the Vice City style -
 *                LEVEL 1, LEVEL 2, LEVEL 3, GAME KEY, EXIT.
 *
 *  Mouse: hover + click.  Keyboard: UP / DOWN (or W / S) and ENTER.
 */

#ifndef MENUSCREEN_HPP
#define MENUSCREEN_HPP

/* menu item ids - also the order on screen */
#define MENU_LEVEL1     0
#define MENU_LEVEL2     1
#define MENU_LEVEL3     2
#define MENU_GAME_KEY   3
#define MENU_EXIT       4
#define MENU_COUNT      5

static const char *MENU_LABEL[MENU_COUNT] =
{
	"LEVEL 1", "LEVEL 2", "LEVEL 3", "GAME KEY", "EXIT"
};

/* the new poster is 941 x 1672 - kept in shape at full window height */
#define POSTER_SRC_W    941.0
#define POSTER_SRC_H   1672.0

unsigned int imgPoster = 0;
int  menuSelected = MENU_LEVEL1;
int  gUiTick = 0;
bool introMenuHover = false;

/* menu bar geometry */
#define MENU_BAR_X     640.0
#define MENU_BAR_W     520.0
#define MENU_BAR_H      62.0
#define MENU_BAR_TOP   452.0
#define MENU_BAR_STEP   80.0

static double menuBarY(int i) { return MENU_BAR_TOP - i * MENU_BAR_STEP; }

/* poster placement on the intro screen: centred, full height */
static double introPosterW() { return SCREEN_HEIGHT * POSTER_SRC_W / POSTER_SRC_H; }
static double introPosterX() { return (SCREEN_WIDTH - introPosterW()) / 2.0; }

/* MENU button: lower-right corner of the poster */
#define INTRO_BTN_W    150.0
#define INTRO_BTN_H     52.0
static double introBtnX() { return introPosterX() + introPosterW() - INTRO_BTN_W - 22.0; }
static double introBtnY() { return 20.0; }

void menuInit()
{
	imgPoster = loadImage("../Level 3/Game_Intro_poster.jpeg");
}

/* ==================== INTRO ==================== */

void introDraw()
{
	double px = introPosterX(), pw = introPosterW();
	double bx = introBtnX(), by = introBtnY();
	double pulse = 0.5 + 0.5 * sin(gUiTick * 0.08);

	gUiTick++;

	drawViceBackdrop(gUiTick);

	/* the poster */
	iSetColor(255, 255, 255);
	iShowImage((int)px, 0, (int)pw, SCREEN_HEIGHT, imgPoster);

	/* thin neon frame down both sides */
	glColor4f(1.0f, 0.25f, 0.75f, 0.9f);
	iFilledRectangle(px - 4, 0, 4, SCREEN_HEIGHT);
	iFilledRectangle(px + pw, 0, 4, SCREEN_HEIGHT);

	/* MENU - the only thing on the poster */
	drawVicePlate(bx, by, INTRO_BTN_W, INTRO_BTN_H,
	              introMenuHover ? 0.95f : 0.08f, introMenuHover ? 0.20f : 0.02f,
	              introMenuHover ? 0.60f : 0.12f, introMenuHover ? 0.92f : 0.78f);
	drawVicePlateOutline(bx, by, INTRO_BTN_W, INTRO_BTN_H,
	                     0.3f, 0.9f, 1.0f, (float)(0.55 + 0.45 * pulse), 2.5f);

	if (introMenuHover)
		drawViceTextCentered(bx + INTRO_BTN_W / 2.0 + 6, by + 14, "MENU", 26, 120, 240, 255, 1.0);
	else
		drawViceTextCentered(bx + INTRO_BTN_W / 2.0 + 6, by + 14, "MENU", 26, 255, 70, 190, 0.4 + 0.6 * pulse);
}

bool introHitMenu(int mx, int my)
{
	double bx = introBtnX(), by = introBtnY();
	return mx >= bx && mx <= bx + INTRO_BTN_W + INTRO_BTN_H * VICE_SLANT &&
	       my >= by && my <= by + INTRO_BTN_H;
}

void introMouseMove(int mx, int my)
{
	introMenuHover = introHitMenu(mx, my);
}

/* ==================== MAIN MENU ==================== */

void menuDraw()
{
	int i;
	double pw = introPosterW();

	gUiTick++;

	drawViceBackdrop(gUiTick);

	/* poster on the left */
	iSetColor(255, 255, 255);
	iShowImage(60, 0, (int)pw, SCREEN_HEIGHT, imgPoster);
	glColor4f(1.0f, 0.25f, 0.75f, 0.9f);
	iFilledRectangle(60 + pw, 0, 4, SCREEN_HEIGHT);

	/* title */
	drawViceText(MENU_BAR_X, 620, "HONDA RUSH", 58, 255, 70, 190, 0.8);
	drawViceText(MENU_BAR_X + 120, 562, "Career Rider", 32, 90, 230, 255, 0.7);

	/* the five bars */
	for (i = 0; i < MENU_COUNT; i++)
	{
		double y = menuBarY(i);
		bool sel = (i == menuSelected);

		if (sel)
		{
			drawVicePlate(MENU_BAR_X, y, MENU_BAR_W, MENU_BAR_H, 0.95f, 0.18f, 0.62f, 0.90f);
			drawVicePlate(MENU_BAR_X, y, MENU_BAR_W, MENU_BAR_H * 0.45f, 0.55f, 0.05f, 0.55f, 0.55f);
			drawVicePlateOutline(MENU_BAR_X, y, MENU_BAR_W, MENU_BAR_H, 0.35f, 0.92f, 1.0f, 1.0f, 3.0f);
			drawViceTextCentered(MENU_BAR_X + MENU_BAR_W / 2.0 + 8, y + 17, MENU_LABEL[i], 30,
			                     120, 240, 255, 1.0);

			/* chevrons either side */
			drawViceText(MENU_BAR_X + 22, y + 17, ">", 30, 255, 255, 255, 0.0);
			drawViceText(MENU_BAR_X + MENU_BAR_W - 30, y + 17, "<", 30, 255, 255, 255, 0.0);
		}
		else
		{
			drawVicePlate(MENU_BAR_X, y, MENU_BAR_W, MENU_BAR_H, 0.06f, 0.02f, 0.10f, 0.78f);
			drawVicePlateOutline(MENU_BAR_X, y, MENU_BAR_W, MENU_BAR_H, 1.0f, 0.3f, 0.75f, 0.85f, 2.0f);
			drawViceTextCentered(MENU_BAR_X + MENU_BAR_W / 2.0 + 8, y + 17, MENU_LABEL[i], 28,
			                     255, 90, 200, 0.35);
		}
	}

	/* small print */
	iSetColor(235, 200, 240);
	drawText(MENU_BAR_X, 40, "UP / DOWN + ENTER, or click.   M  mute.   ESC  back to the poster.",
	         GLUT_BITMAP_HELVETICA_12);
	drawText(MENU_BAR_X, 20, "CSE-1200  -  AUST   |   Foysal Ahmed Sami  -  Parijit Sarker  -  Mujahidul Islam Ansary Niloy",
	         GLUT_BITMAP_HELVETICA_12);
}

/* returns the bar under the mouse, or -1 */
int menuHitTest(int mx, int my)
{
	int i;
	for (i = 0; i < MENU_COUNT; i++)
	{
		double y = menuBarY(i);
		if (mx >= MENU_BAR_X && mx <= MENU_BAR_X + MENU_BAR_W + MENU_BAR_H * VICE_SLANT &&
		    my >= y && my <= y + MENU_BAR_H)
			return i;
	}
	return -1;
}

void menuMouseMove(int mx, int my)
{
	int i = menuHitTest(mx, my);
	if (i >= 0) menuSelected = i;
}

void menuMoveSelection(int delta)
{
	menuSelected = (menuSelected + delta + MENU_COUNT) % MENU_COUNT;
}

#endif
