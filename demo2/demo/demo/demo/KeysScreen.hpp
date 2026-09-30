/*
 *  KeysScreen.hpp  --  the control list ("GAME KEY" from the main menu)
 *
 *  Two columns: levels 1 and 2 plus the menus on the left, level 3 on the
 *  right (it adds VOLT SPEED, getting on / off the Honda, walking and the
 *  machine gun).
 */

#ifndef KEYSSCREEN_HPP
#define KEYSSCREEN_HPP

struct KeyRow
{
	const char *keys;      /* NULL keys  =>  section heading */
	const char *action;
};

static KeyRow gKeyRowsLeft[] =
{
	{ NULL,                "LEVEL 1 AND LEVEL 2"                  },
	{ "D / RIGHT",         "Throttle - ride forward faster"       },
	{ "A / LEFT",          "Brake"                                },
	{ "W / SPACE / UP",    "Jump"                                 },
	{ "S",                 "Level 2: drop out of a jump faster"   },
	{ "P",                 "Level 1: pause / resume"              },
	{ "R",                 "Restart the level"                    },
	{ "ESC",               "Back to the main menu"                },

	{ NULL,                "MENUS AND SYSTEM"                     },
	{ "UP / DOWN",         "Move the menu highlight"              },
	{ "ENTER",             "Choose the highlighted option"        },
	{ "LEFT MOUSE",        "Click MENU or any menu bar"           },
	{ "M",                 "Mute / unmute the music"              }
};

static KeyRow gKeyRowsRight[] =
{
	{ NULL,                "LEVEL 3  -  ON THE HONDA"             },
	{ "D",                 "Throttle"                             },
	{ "A",                 "Brake, then roll backwards"           },
	{ "W",                 "Jump the oncoming cars"               },
	{ "S",                 "After a jump - land much faster"      },
	{ "V",                 "VOLT SPEED - needed for the 360 loop" },
	{ "ENTER",             "Get off when robbers stop the Honda"  },

	{ NULL,                "LEVEL 3  -  ON FOOT"                  },
	{ "D / hold D",        "Walk / run forward"                   },
	{ "A / hold A",        "Stop / walk back"                     },
	{ "W  then  S",        "Jump, S drops fast - land on heads"   },
	{ "ENTER",             "Back on the Honda (robbers cleared)"  },
	{ "SPACE (hold)",      "Shoot - once the machine gun is on"   },

	{ NULL,                "LEVEL 3  -  CODES AND STORY"          },
	{ "MACHINEGUNON",      "Machine gun (any time)"               },
	{ "GETONTHEBIKE",      "BG6: the girl gets on the Honda"      },
	{ "COMPLETEYOUR...",   "BG9: COMPLETEYOURPAYMENTQUICKLY, 20 s"},
	{ "ENTER",             "Next subtitle line"                   }
};

static void keysDrawColumn(const KeyRow *rows, int count, double x0)
{
	int i;
	double y = SCREEN_HEIGHT - 118;

	for (i = 0; i < count; i++)
	{
		if (rows[i].keys == NULL)
		{
			y -= 6;
			iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
			drawText(x0, y, rows[i].action, GLUT_BITMAP_HELVETICA_18);
			iFilledRectangle(x0, y - 7, 560, 2);
			y -= 30;
		}
		else
		{
			iSetColor(COL_PANEL_R, COL_PANEL_G, COL_PANEL_B);
			iFilledRectangle(x0 + 6, y - 6, 176, 21);
			iSetColor(70, 70, 80);
			iRectangle(x0 + 6, y - 6, 176, 21);

			iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
			drawTextCentered(x0 + 94, y, rows[i].keys, GLUT_BITMAP_HELVETICA_12);
			drawText(x0 + 196, y, rows[i].action, GLUT_BITMAP_HELVETICA_12);
			y -= 25;
		}
	}
}

void keysDraw()
{
	drawBackdrop();
	drawScreenHeader("GAME KEYS");

	keysDrawColumn(gKeyRowsLeft,  sizeof(gKeyRowsLeft)  / sizeof(gKeyRowsLeft[0]),  50);
	keysDrawColumn(gKeyRowsRight, sizeof(gKeyRowsRight) / sizeof(gKeyRowsRight[0]), 660);

	iSetColor(COL_MUTED_R, COL_MUTED_G, COL_MUTED_B);
	drawText(50, 26, "Level 3: VOLT SPEED for the loop. At BG4 / BG5 the Honda stops - get off and fight "
	                 "(3 hits in BG4, 6 in BG5). P / R / ESC: pause / restart / menu.",
	         GLUT_BITMAP_HELVETICA_12);
}

#endif
