/*
 *  Input.hpp  --  edge triggered keyboard helpers
 *
 *  iGraphics gives us isKeyPressed() / isSpecialKeyPressed(), which stay 1
 *  for as long as a key is held down.  That is what we want for driving the
 *  bike, but it is useless for menus (one tap would scroll through every
 *  item).  These helpers remember last frame's state so we can also ask
 *  "was this key pressed *this* frame?".
 *
 *  inputEndFrame() MUST be called at the very end of fixedUpdate().
 */

#ifndef INPUT_HPP
#define INPUT_HPP

unsigned char prevKey[256]     = { 0 };
unsigned char prevSpecialKey[256] = { 0 };

/* true only on the frame the key goes down */
bool keyJustPressed(unsigned char key)
{
	return (isKeyPressed(key) != 0) && (prevKey[key] == 0);
}

bool specialKeyJustPressed(unsigned char key)
{
	return (isSpecialKeyPressed(key) != 0) && (prevSpecialKey[key] == 0);
}

/* true for as long as the key is held */
bool keyHeld(unsigned char key)
{
	return isKeyPressed(key) != 0;
}

bool specialKeyHeld(unsigned char key)
{
	return isSpecialKeyPressed(key) != 0;
}

void inputEndFrame()
{
	int i;
	for (i = 0; i < 256; i++)
	{
		prevKey[i]        = (unsigned char)(isKeyPressed((unsigned char)i) ? 1 : 0);
		prevSpecialKey[i] = (unsigned char)(isSpecialKeyPressed((unsigned char)i) ? 1 : 0);
	}
}

#define KEY_ESC        27
#define KEY_ENTER      13
#define KEY_BACKSPACE   8

#endif
