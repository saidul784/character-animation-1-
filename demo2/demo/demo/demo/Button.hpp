/*
 *  Button.hpp  --  a simple clickable / selectable menu button
 */

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <string.h>

struct Button
{
	double x, y, w, h;      /* x,y = bottom-left corner (OpenGL coordinates) */
	char   label[48];

	void set(double bx, double by, double bw, double bh, const char *text)
	{
		x = bx; y = by; w = bw; h = bh;
		strcpy_s(label, text);
	}

	bool contains(int mx, int my) const
	{
		return (mx >= x && mx <= x + w && my >= y && my <= y + h);
	}

	/*  selected == mouse is over it, or it is the current keyboard choice  */
	void draw(bool selected) const
	{
		if (selected)
		{
			iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
			iFilledRectangle(x, y, w, h);

			iSetColor(255, 255, 255);
			drawBorder(x, y, w, h, 2);

			/* red speed marker on the left edge */
			iSetColor(255, 255, 255);
			drawText(x + 18, y + h / 2 - 7, ">", GLUT_BITMAP_HELVETICA_18);

			iSetColor(255, 255, 255);
			drawTextCentered(x + w / 2, y + h / 2 - 7, label, GLUT_BITMAP_HELVETICA_18);
		}
		else
		{
			iSetColor(COL_PANEL_R, COL_PANEL_G, COL_PANEL_B);
			iFilledRectangle(x, y, w, h);

			iSetColor(70, 70, 80);
			drawBorder(x, y, w, h, 1);

			iSetColor(COL_TEXT_R, COL_TEXT_G, COL_TEXT_B);
			drawTextCentered(x + w / 2, y + h / 2 - 7, label, GLUT_BITMAP_HELVETICA_18);
		}
	}
};

#endif
