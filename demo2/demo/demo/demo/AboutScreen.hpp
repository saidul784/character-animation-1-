/*
 *  AboutScreen.hpp  --  summary of the CSE-1200 project proposal
 *                       ("ABOUT THE GAME" from the main menu)
 *
 *  Row styles:  'H' heading   'B' body line   '-' bullet   ' ' blank spacer
 */

#ifndef ABOUTSCREEN_HPP
#define ABOUTSCREEN_HPP

struct AboutRow
{
	char        style;
	const char *text;
};

static AboutRow gAboutRows[] =
{
	{ 'H', "GAME DESCRIPTION" },
	{ 'B', "Honda Rush: Career Rider is a 2D side-scrolling, physics-based motorcycle racing" },
	{ 'B', "game. The player rides Honda motorcycles through tracks filled with ramps, obstacles" },
	{ 'B', "and dangerous terrain - holding the bike balanced, avoiding crashes, collecting fuel" },
	{ 'B', "cans, and reaching the finish line before the tank runs dry." },
	{ ' ', "" },

	{ 'H', "CORE FEATURES" },
	{ '-', "Physics based bike control - acceleration, braking, rotation, jump and landing" },
	{ '-', "Dynamic fuel management - fuel drains while riding, fuel cans refill the tank" },
	{ '-', "Obstacle system - ramps, jumps, moving platforms, traps and barriers" },
	{ '-', "Career progression - earn coins, unlock faster and more efficient Honda bikes" },
	{ '-', "Level progression - every track adds harder obstacles than the one before" },
	{ '-', "Score system - completion time bonus, remaining fuel bonus, coins collected" },
	{ ' ', "" },

	{ 'H', "OBJECTIVE" },
	{ 'B', "To test the player's reflexes, bike control and fuel strategy while giving a career" },
	{ 'B', "worth grinding through. Built for players aged 12 and above, inspired by Moto X3M," },
	{ 'B', "with a Honda themed progression and fuel management system of its own." },
	{ ' ', "" },

	{ 'H', "THE TEAM  -  CSE-1200, SOFTWARE DEVELOPMENT I" },
	{ 'B', "Foysal Ahmed Sami  (00725105101149)          Parijit Sarker  (00725105101152)" },
	{ 'B', "Mujahidul Islam Ansary Niloy  (00725105101164)" },
	{ 'B', "Supervised by Saha Reno, Assistant Professor, and Mustofa Ahmed, Lecturer, CSE, AUST" }
};

static const int gAboutRowCount = sizeof(gAboutRows) / sizeof(gAboutRows[0]);

void aboutDraw()
{
	int i;
	double y = SCREEN_HEIGHT - 122;

	drawBackdrop();
	drawScreenHeader("ABOUT THE GAME");

	for (i = 0; i < gAboutRowCount; i++)
	{
		switch (gAboutRows[i].style)
		{
		case 'H':
			iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
			drawText(90, y, gAboutRows[i].text, GLUT_BITMAP_HELVETICA_18);
			iFilledRectangle(90, y - 9, 1100, 2);
			y -= 38;
			break;

		case '-':
			iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
			iFilledRectangle(96, y + 5, 8, 8);
			iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
			drawText(118, y, gAboutRows[i].text, GLUT_BITMAP_HELVETICA_18);
			y -= 24;
			break;

		case 'B':
			iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
			drawText(96, y, gAboutRows[i].text, GLUT_BITMAP_HELVETICA_18);
			y -= 24;
			break;

		default:
			y -= 12;
			break;
		}
	}
}

#endif
