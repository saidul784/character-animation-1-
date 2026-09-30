/*
 *  ViceText.hpp  --  neon, slanted "Vice City" style lettering
 *
 *  The project has no font files and iGraphics only draws GLUT fonts, so
 *  the look is built from GLUT's scalable stroke font: every string is
 *  slanted, drawn fat several times (dark outline, pink or cyan glow,
 *  bright core, white highlight) - the 80s neon sign look of the GTA
 *  Vice City menus, using nothing but the existing graphics system.
 *
 *  Also holds the sunset backdrop shared by the intro and the menu.
 */

#ifndef VICETEXT_HPP
#define VICETEXT_HPP

#define VICE_SLANT 0.24    /* italic lean */

/* width of a string in GLUT stroke units (a capital is about 100 tall) */
double viceStrokeWidth(const char *s)
{
	double w = 0;
	for (; *s; s++)
		w += glutStrokeWidth(GLUT_STROKE_ROMAN, *s);
	return w;
}

/* width on screen for a given cap height */
double viceTextWidth(const char *s, double size)
{
	return viceStrokeWidth(s) * size / 100.0 + size * VICE_SLANT;
}

/* one pass of slanted stroke text */
static void viceStrokePass(double x, double y, const char *s, double size, double lineW)
{
	GLfloat shear[16] =
	{
		1, 0, 0, 0,
		(GLfloat)VICE_SLANT, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	double sc = size / 100.0;

	glLineWidth((GLfloat)lineW);
	glPushMatrix();
	glTranslated(x, y, 0);
	glMultMatrixf(shear);
	glScaled(sc, sc, 1);
	for (; *s; s++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *s);
	glPopMatrix();
}

/*  Draws one neon label with its baseline at (x, y).
    r,g,b    main neon colour
    glow     0..1, how strong the halo is (selected items glow more)      */
void drawViceText(double x, double y, const char *s, double size,
                  int r, int g, int b, double glow)
{
	double fat = size / 11.0;       /* stroke weight follows the size */
	int i;

	if (fat < 2.0) fat = 2.0;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LINE_SMOOTH);

	/* drop shadow */
	glColor4f(0.08f, 0.0f, 0.12f, 0.85f);
	for (i = 0; i < 4; i++)
		viceStrokePass(x + 4 + (i % 2), y - 4 - (i / 2), s, size, fat + 2);

	/* neon halo */
	if (glow > 0)
	{
		glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, (GLfloat)(0.18 * glow));
		viceStrokePass(x, y, s, size, fat * 4.0);
		glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, (GLfloat)(0.30 * glow));
		viceStrokePass(x, y, s, size, fat * 2.4);
	}

	/* dark outline, then the colour, then a hot core */
	glColor4f(0.12f, 0.0f, 0.18f, 1.0f);
	viceStrokePass(x, y, s, size, fat + 3);

	glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	viceStrokePass(x, y, s, size, fat);
	viceStrokePass(x + 1, y, s, size, fat);

	glColor4f((r + 255) / 510.0f, (g + 255) / 510.0f, (b + 255) / 510.0f, 1.0f);
	viceStrokePass(x + 0.5, y + size * 0.02, s, size, fat * 0.35 < 1.0 ? 1.0 : fat * 0.35);

	glLineWidth(1.0f);
	glDisable(GL_LINE_SMOOTH);
}

void drawViceTextCentered(double cx, double y, const char *s, double size,
                          int r, int g, int b, double glow)
{
	drawViceText(cx - viceTextWidth(s, size) / 2.0, y, s, size, r, g, b, glow);
}

/*  A flat slanted plate - the bar behind a menu item. Built with raw GL
    because iFilledPolygon in this iGraphics version never calls glEnd(). */
void drawVicePlate(double x, double y, double w, double h,
                   float r, float g, float b, float a)
{
	double lean = h * VICE_SLANT;

	glDisable(GL_TEXTURE_2D);
	glColor4f(r, g, b, a);
	glBegin(GL_QUADS);
	glVertex2d(x,            y);
	glVertex2d(x + w,        y);
	glVertex2d(x + w + lean, y + h);
	glVertex2d(x + lean,     y + h);
	glEnd();
}

void drawVicePlateOutline(double x, double y, double w, double h,
                          float r, float g, float b, float a, float lineW)
{
	double lean = h * VICE_SLANT;

	glDisable(GL_TEXTURE_2D);
	glLineWidth(lineW);
	glColor4f(r, g, b, a);
	glBegin(GL_LINE_LOOP);
	glVertex2d(x,            y);
	glVertex2d(x + w,        y);
	glVertex2d(x + w + lean, y + h);
	glVertex2d(x + lean,     y + h);
	glEnd();
	glLineWidth(1.0f);
}

/*  Vice City sunset: deep purple at the top, hot pink, then orange at the
    horizon, with a neon grid on the ground below it.                    */
void drawViceBackdrop(int tick)
{
	int i;
	double horizon = 190;

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	/* sky: purple -> magenta */
	glColor3f(0.10f, 0.02f, 0.22f); glVertex2d(0, SCREEN_HEIGHT); glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
	glColor3f(0.55f, 0.08f, 0.45f); glVertex2d(SCREEN_WIDTH, 420); glVertex2d(0, 420);
	/* magenta -> orange at the horizon */
	glColor3f(0.55f, 0.08f, 0.45f); glVertex2d(0, 420); glVertex2d(SCREEN_WIDTH, 420);
	glColor3f(1.00f, 0.45f, 0.25f); glVertex2d(SCREEN_WIDTH, horizon); glVertex2d(0, horizon);
	/* ground */
	glColor3f(0.12f, 0.02f, 0.18f); glVertex2d(0, horizon); glVertex2d(SCREEN_WIDTH, horizon);
	glColor3f(0.03f, 0.00f, 0.06f); glVertex2d(SCREEN_WIDTH, 0); glVertex2d(0, 0);
	glEnd();

	/* neon grid: horizontal lines drift toward the viewer */
	glLineWidth(1.5f);
	glBegin(GL_LINES);
	for (i = 0; i < 10; i++)
	{
		double t = fmod(i / 10.0 + tick * 0.002, 1.0);
		double y = horizon - (t * t) * horizon;
		glColor4f(1.0f, 0.25f, 0.75f, (GLfloat)(0.25 + 0.6 * t));
		glVertex2d(0, y);
		glVertex2d(SCREEN_WIDTH, y);
	}
	for (i = -12; i <= 12; i++)
	{
		double xTop = SCREEN_WIDTH / 2.0 + i * 40.0;
		double xBot = SCREEN_WIDTH / 2.0 + i * 190.0;
		glColor4f(0.2f, 0.85f, 1.0f, 0.35f);
		glVertex2d(xTop, horizon);
		glVertex2d(xBot, 0);
	}
	glEnd();
	glLineWidth(1.0f);
}

#endif
