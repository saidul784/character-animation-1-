/*
 *  ============================================================
 *   HONDA RUSH : CAREER RIDER
 *   CSE-1200  Software Development I  --  AUST, Dept. of CSE
 *
 *   Foysal Ahmed Sami            00725105101149
 *   Parijit Sarker               00725105101152
 *   Mujahidul Islam Ansary Niloy 00725105101164
 *  ============================================================
 *
 *  Built with C++ / iGraphics / OpenGL-GLUT, Visual Studio 2013.
 *
 *  This file owns the game loop and routes every callback
 *  (draw, mouse, keyboard) to whichever screen is currently open.
 *
 *  Screens:
 *      STATE_INTRO    new intro poster with a single MENU button
 *      STATE_MENU     LEVEL 1 / LEVEL 2 / LEVEL 3 / GAME KEY / EXIT
 *      STATE_KEYS     keyboard control list (GAME KEY)
 *      STATE_ABOUT    project proposal summary (kept, not on the menu)
 *      STATE_LEVEL01  level 01 track
 *      STATE_LEVEL02  level 02 route
 *      STATE_LEVEL03  level 03 route (fights, the girl, One Bank, AUST)
 *
 *  IMPORTANT: iGraphics.h has no include guard, so it is included here
 *  once and nowhere else. Every other header below depends on it and must
 *  stay in this order.
 */

#include <cstdio>
#include "iGraphics.h"

#include "Config.h"        /* window size, states, colours, draw helpers */
#include "Input.hpp"       /* edge triggered key helpers                 */
#include "Audio.hpp"       /* MCI background music                       */
#include "Button.hpp"      /* menu button widget                         */
#include "ViceText.hpp"    /* neon Vice City style lettering + backdrop  */
#include "MenuScreen.hpp"  /* new intro poster + main menu               */
#include "KeysScreen.hpp"  /* controls list                              */
#include "AboutScreen.hpp" /* proposal summary                           */
#include "Level01.hpp"     /* level 01                                   */
#include "Level02.hpp"     /* level 02                                   */
#include "Level03.hpp"     /* level 03 (uses level 02's coin + fuel art) */

/* -------------------- STATE SWITCHING -------------------- */

void quitGame()
{
	audioShutdown();
	exit(0);
}

/*  Music rule: the intro song loops behind the poster, the KEYS screen and
    the ABOUT screen, and only stops when the player starts a level.
    musicPlay() ignores a request for the track already running, so moving
    between the three intro screens never restarts the song.              */
void gotoState(GameState next)
{
	gState = next;

	if (next == STATE_LEVEL01)
	{
		level01Reset();
		playLevel01Music();
	}
	else if (next == STATE_LEVEL02)
	{
		level02Reset();
		playLevel02Music();
	}
	else if (next == STATE_LEVEL03)
	{
		level03Reset();
		playLevel03Music();          /* Song 1 */
	}
	else
	{
		musicPlay(MUSIC_INTRO);
	}
}

void menuActivate(int item)
{
	switch (item)
	{
	case MENU_LEVEL1:   gotoState(STATE_LEVEL01); break;
	case MENU_LEVEL2:   gotoState(STATE_LEVEL02); break;
	case MENU_LEVEL3:   gotoState(STATE_LEVEL03); break;
	case MENU_GAME_KEY: gotoState(STATE_KEYS);    break;
	case MENU_EXIT:     quitGame();               break;
	}
}

/* -------------------- DRAW -------------------- */
void iDraw()
{
	iClear();

	switch (gState)
	{
	case STATE_INTRO:   introDraw();   break;
	case STATE_MENU:    menuDraw();    break;
	case STATE_KEYS:    keysDraw();    break;
	case STATE_ABOUT:   aboutDraw();   break;
	case STATE_LEVEL01: level01Draw(); break;
	case STATE_LEVEL02: drawLevel02();  break;
	case STATE_LEVEL03: drawLevel03();  break;
	}
}

/* -------------------- MOUSE -------------------- */
void iMouse(int button, int state, int mx, int my)
{
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;

	if (gState == STATE_INTRO)
	{
		if (introHitMenu(mx, my))
			gotoState(STATE_MENU);
	}
	else if (gState == STATE_MENU)
	{
		int item = menuHitTest(mx, my);
		if (item >= 0)
			menuActivate(item);
	}
	else if (gState == STATE_KEYS || gState == STATE_ABOUT)
	{
		gotoState(STATE_MENU);
	}
}

void iPassiveMouseMove(int mx, int my)
{
	if (gState == STATE_INTRO)
		introMouseMove(mx, my);
	else if (gState == STATE_MENU)
		menuMouseMove(mx, my);
}

void iMouseMove(int mx, int my)
{
	if (gState == STATE_INTRO)
		introMouseMove(mx, my);
	else if (gState == STATE_MENU)
		menuMouseMove(mx, my);
}

/* -------------------- KEYBOARD / FIXED UPDATE --------------------
 *
 *  iGraphics calls fixedUpdate() about 60 times a second (see the
 *  keyboardSamplingRate argument of iInitialize).  Held keys are read with
 *  keyHeld(); single taps with keyJustPressed().
 */
void fixedUpdate()
{
	switch (gState)
	{
	case STATE_INTRO:
		if (keyJustPressed(KEY_ENTER) || keyJustPressed(' '))
			gotoState(STATE_MENU);

		if (keyJustPressed(KEY_ESC))
			quitGame();
		break;

	case STATE_MENU:
		if (specialKeyJustPressed(GLUT_KEY_DOWN) || keyJustPressed('s') || keyJustPressed('S'))
			menuMoveSelection(1);

		if (specialKeyJustPressed(GLUT_KEY_UP) || keyJustPressed('w') || keyJustPressed('W'))
			menuMoveSelection(-1);

		if (keyJustPressed(KEY_ENTER) || keyJustPressed(' '))
			menuActivate(menuSelected);

		/* number keys jump straight into a level */
		if (keyJustPressed('1'))
			gotoState(STATE_LEVEL01);
		if (keyJustPressed('2'))
			gotoState(STATE_LEVEL02);
		if (keyJustPressed('3'))
			gotoState(STATE_LEVEL03);

		if (keyJustPressed(KEY_ESC))
			gotoState(STATE_INTRO);      /* back to the poster */
		break;

	case STATE_KEYS:
	case STATE_ABOUT:
		if (keyJustPressed(KEY_ESC) || keyJustPressed(KEY_BACKSPACE) || keyJustPressed(KEY_ENTER))
			gotoState(STATE_MENU);
		break;

	case STATE_LEVEL01:
		if (keyJustPressed(KEY_ESC))
		{
			gotoState(STATE_MENU);       /* also swaps back to the intro song */
			break;
		}

		/* R restarts the level, from the result screens too */
		if (keyJustPressed('r') || keyJustPressed('R'))
		{
			level01Reset();
			playLevel01Music();
		}
		else if (lvlState == LEVEL01_PLAYING)
		{
			if (keyJustPressed('p') || keyJustPressed('P'))
				lvlPaused = !lvlPaused;

			/* jumping is edge triggered: one press, one jump */
			if (keyJustPressed(' ') || keyJustPressed('w') || keyJustPressed('W') ||
			    specialKeyJustPressed(GLUT_KEY_UP))
				lvlJumpRequested = true;

			level01Update();
		}
		break;

	case STATE_LEVEL02:
		if (keyJustPressed(KEY_ESC))
		{
			gotoState(STATE_MENU);       /* also swaps back to the intro song */
			break;
		}

		if (keyJustPressed('r') || keyJustPressed('R'))
		{
			level02Reset();
			playLevel02Music();
		}

		/* jumping is edge triggered: one press, one jump */
		if (keyJustPressed('w') || keyJustPressed('W') || keyJustPressed(' ') ||
		    specialKeyJustPressed(GLUT_KEY_UP))
			l2JumpRequested = true;

		updateLevel02();
		break;

	case STATE_LEVEL03:
		if (keyJustPressed(KEY_ESC))
		{
			gotoState(STATE_MENU);       /* also swaps back to the intro song */
			break;
		}

		/* R restarts - except while a code is being typed (R is a letter
		   of COMPLETEYOURPAYMENTQUICKLY) */
		if ((keyJustPressed('r') || keyJustPressed('R')) && !level03BlocksRestart())
		{
			level03Reset();
			playLevel03Music();
			break;
		}

		/* ENTER, V, W, P, subtitles and the typed codes (which also own M) */
		level03HandleInput();
		updateLevel03();
		break;
	}

	/* works on every screen; level 03 handles M itself because M is also
	   the first letter of its cheat code */
	if (gState != STATE_LEVEL03 && (keyJustPressed('m') || keyJustPressed('M')))
		musicToggleMute();

	inputEndFrame();   /* must stay last */
}

/* -------------------- MAIN -------------------- */
int main()
{
	/* On a display scaled above 100%, Windows stretches the window and the
	   1280x720 canvas can end up wider than the screen. Declaring the game
	   DPI aware keeps one game pixel equal to one screen pixel. */
	SetProcessDPIAware();

	/* creates the window and the OpenGL context - images can only be
	   loaded after this call */
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);

	enableAlphaBlending();   /* smooth edges on the sprite PNGs */

	menuInit();    printf("[init] menu ok\n");    fflush(stdout);
	level01Init(); printf("[init] level01 ok\n"); fflush(stdout);
	level02Init(); printf("[init] level02 ok\n"); fflush(stdout);
	level03Init(); printf("[init] level03 ok\n"); fflush(stdout);                   /* after level 02: shares its coins */

	audioInit();
	sfxInit();                       /* collision / coin / win / lose sounds */
	musicPlay(MUSIC_INTRO);          /* intro song starts with the poster */
	iSetTimer(1000, musicWatchdog);  /* keeps the track looping           */

	iStart();
	return 0;
}
