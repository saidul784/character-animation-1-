/*
 *  MenuScreen.hpp  --  intro poster + main menu
 *
 *  Left  : the "Honda Rush - Career Rider" intro poster, full height.
 *  Right : NEW GAME / KEYS / ABOUT THE GAME / EXIT.
 *
 *  Works with the mouse (hover + click) and with the keyboard
 *  (UP / DOWN / W / S to move, ENTER or SPACE to choose).
 */

#ifndef MENUSCREEN_HPP
#define MENUSCREEN_HPP

/* menu item ids - also the index into menuBtn[] */
#define MENU_NEW_GAME   0
#define MENU_LEVEL2     1
#define MENU_KEYS       2
#define MENU_ABOUT      3
#define MENU_EXIT       4
#define MENU_COUNT      5

/* poster keeps its 1086 x 1448 shape, scaled to the full window height */
#define POSTER_X        32
#define POSTER_Y        0
#define POSTER_H        SCREEN_HEIGHT
#define POSTER_W        540

Button menuBtn[MENU_COUNT];
int    menuSelected = MENU_NEW_GAME;   /* keyboard / hover highlight */

unsigned int imgPoster = 0;

void menuInit()
{
	double bx = 700, bw = 480, bh = 62;

	imgPoster = loadImage("Images/intropoester.png");

	menuBtn[MENU_NEW_GAME].set(bx, 500, bw, bh, "NEW GAME");
	menuBtn[MENU_LEVEL2  ].set(bx, 424, bw, bh, "LEVEL 2");
	menuBtn[MENU_KEYS    ].set(bx, 348, bw, bh, "KEYS");
	menuBtn[MENU_ABOUT   ].set(bx, 272, bw, bh, "ABOUT THE GAME");
	menuBtn[MENU_EXIT    ].set(bx, 196, bw, bh, "EXIT");
}

void menuDraw()
{
	int i;

	drawBackdrop();

	/* ---- intro poster ---- */
	iSetColor(255, 255, 255);
	iShowImage(POSTER_X, POSTER_Y, POSTER_W, POSTER_H, imgPoster);

	/* thin red frame around the poster */
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	drawBorder(POSTER_X, POSTER_Y, POSTER_W, POSTER_H, 2);

	/* ---- title block ---- */
	iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
	drawText(700, 632, "HONDA RUSH", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	drawText(700, 604, "C A R E E R   R I D E R", GLUT_BITMAP_HELVETICA_18);
	iFilledRectangle(700, 580, 480, 3);

	/* ---- menu buttons ---- */
	for (i = 0; i < MENU_COUNT; i++)
		menuBtn[i].draw(i == menuSelected);

	/* ---- hints / credits ---- */
	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(700, 150, "Click an option, or use UP / DOWN and press ENTER.",
	         GLUT_BITMAP_HELVETICA_12);
	drawText(700, 130, "M  -  mute or unmute the music.", GLUT_BITMAP_HELVETICA_12);

	iSetColor(90, 90, 100);
	iFilledRectangle(700, 108, 480, 1);

	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(700, 84, "CSE-1200  Software Development I   -   AUST, Dept. of CSE",
	         GLUT_BITMAP_HELVETICA_12);
	drawText(700, 62, "Foysal Ahmed Sami   -   Parijit Sarker   -   Mujahidul Islam Ansary Niloy",
	         GLUT_BITMAP_HELVETICA_12);
}

/* highlight whatever the mouse is over */
void menuMouseMove(int mx, int my)
{
	int i;
	for (i = 0; i < MENU_COUNT; i++)
	{
		if (menuBtn[i].contains(mx, my))
		{
			menuSelected = i;
			return;
		}
	}
}

/* returns the clicked item, or -1 */
int menuHitTest(int mx, int my)
{
	int i;
	for (i = 0; i < MENU_COUNT; i++)
		if (menuBtn[i].contains(mx, my))
			return i;
	return -1;
}

void menuMoveSelection(int delta)
{
	menuSelected = (menuSelected + delta + MENU_COUNT) % MENU_COUNT;
}

#endif
