/*
 *  KeysScreen.hpp  --  the control list ("KEYS" from the main menu)
 */

#ifndef KEYSSCREEN_HPP
#define KEYSSCREEN_HPP

struct KeyRow
{
	const char *keys;      /* NULL keys + text != NULL  =>  section heading */
	const char *action;
	int         planned;   /* 1 = feature arrives with a later build        */
};

static KeyRow gKeyRows[] =
{
	{ NULL,                  "RIDING",                                        0 },
	{ "SPACE / W / UP",      "Jump - clear the barriers and the fire",        0 },
	{ "D  /  RIGHT ARROW",   "Throttle - rush forward faster",                0 },
	{ "A  /  LEFT ARROW",    "Brake - slow down before an obstacle",          0 },

	{ NULL,                  "LEVEL",                                         0 },
	{ "P",                   "Pause / resume the level",                      0 },
	{ "R",                   "Restart the current level",                     0 },
	{ "ESC",                 "Leave the level and return to the main menu",   0 },

	{ NULL,                  "MENUS AND SYSTEM",                              0 },
	{ "UP / DOWN",           "Move the menu highlight",                       0 },
	{ "ENTER",               "Choose the highlighted option",                 0 },
	{ "LEFT MOUSE",          "Click any menu option directly",                0 },
	{ "M",                   "Mute / unmute the music",                       0 }
};

static const int gKeyRowCount = sizeof(gKeyRows) / sizeof(gKeyRows[0]);

void keysDraw()
{
	int i;
	double y = SCREEN_HEIGHT - 130;

	drawBackdrop();
	drawScreenHeader("KEYBOARD CONTROLS");

	/* two column table */
	for (i = 0; i < gKeyRowCount; i++)
	{
		if (gKeyRows[i].keys == NULL)          /* section heading */
		{
			y -= 14;
			iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
			drawText(90, y, gKeyRows[i].action, GLUT_BITMAP_HELVETICA_18);
			iFilledRectangle(90, y - 10, 1100, 2);
			y -= 34;
		}
		else
		{
			/* key capsule */
			iSetColor(COL_PANEL_R, COL_PANEL_G, COL_PANEL_B);
			iFilledRectangle(104, y - 8, 250, 30);
			iSetColor(70, 70, 80);
			iRectangle(104, y - 8, 250, 30);

			iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
			drawTextCentered(229, y, gKeyRows[i].keys, GLUT_BITMAP_HELVETICA_12);

			if (gKeyRows[i].planned)
				iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
			else
				iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);

			drawText(400, y, gKeyRows[i].action, GLUT_BITMAP_HELVETICA_18);

			if (gKeyRows[i].planned)
			{
				iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
				drawText(400 + textWidth(gKeyRows[i].action) + 12, y, "*",
				         GLUT_BITMAP_HELVETICA_18);
			}

			y -= 34;
		}
	}

	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(104, 40, "The bike never stops - it rushes forward. Collect coins, grab the OCTANE "
	                  "cans before the tank runs dry, and jump everything else.",
	         GLUT_BITMAP_HELVETICA_12);
}

#endif
