/*
 *  Audio.hpp  --  background music through the Windows MCI interface
 *
 *  iGraphics has no sound API of its own, so we use mciSendString() from
 *  the Windows multimedia library.  winmm.lib is already linked by the
 *  #pragma comment(lib, "winmm.lib") inside glut.h.
 *
 *  Each track is opened once at start-up under an alias, then started and
 *  stopped by name.  "play <alias> repeat" loops the track; a 1 second
 *  watchdog timer restarts it if a driver ignores the repeat flag, so the
 *  intro song keeps looping until the player presses NEW GAME.
 */

#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <mmsystem.h>
#include <string.h>
#pragma comment(lib, "winmm.lib")

#define MUSIC_NONE     0
#define MUSIC_INTRO    1
#define MUSIC_LEVEL01  2
#define MUSIC_LEVEL02  3
#define MUSIC_L3_SONG1 4     /* level 03, from the start until the rider gets off */
#define MUSIC_L3_SONG2 5     /* level 03, from the moment the rider gets off      */
#define MUSIC_L3_SONG3 6     /* level 03, opened for later sections               */

int  gCurrentMusic = MUSIC_NONE;   /* what SHOULD be playing right now */
bool gMusicMuted   = false;
bool gAudioReady   = false;

/* ---- low level ------------------------------------------------------- */

/* sends one MCI command; returns true on success */
bool mciCommand(const char *cmd)
{
	MCIERROR err = mciSendString(cmd, NULL, 0, NULL);
	if (err != 0)
	{
		char msg[256];
		mciGetErrorString(err, msg, sizeof(msg));
		printf("[audio] \"%s\"  ->  %s\n", cmd, msg);
		return false;
	}
	return true;
}

/* reads back a value, e.g. the playing mode of a track */
bool mciQuery(const char *cmd, char *out, int outSize)
{
	out[0] = '\0';
	return mciSendString(cmd, out, outSize, NULL) == 0;
}

/* opens one media file under an alias */
bool audioOpen(const char *path, const char *alias)
{
	char cmd[512];

	sprintf_s(cmd, "open \"%s\" type mpegvideo alias %s", path, alias);
	if (mciCommand(cmd))
		return true;

	/* fall back to letting MCI pick the device for the file extension */
	sprintf_s(cmd, "open \"%s\" alias %s", path, alias);
	return mciCommand(cmd);
}

const char *musicAlias(int music)
{
	if (music == MUSIC_INTRO)   return "introsong";
	if (music == MUSIC_LEVEL01) return "level01song";
	if (music == MUSIC_LEVEL02) return "level02song";
	if (music == MUSIC_L3_SONG1) return "l3song1";
	if (music == MUSIC_L3_SONG2) return "l3song2";
	if (music == MUSIC_L3_SONG3) return "l3song3";
	return "";
}

/* ---- public API ------------------------------------------------------ */

void audioInit()
{
	bool a = audioOpen("Audios/introsong.mp3", "introsong");
	bool b = audioOpen("../Level1/Game_music.mp3", "level01song");
	bool c = audioOpen("../level 2/Level02_bgsound.mp3", "level02song");
	bool d = audioOpen("../Level 3/Song_1.mp3", "l3song1");
	bool e = audioOpen("../Level 3/Song_2.mp3", "l3song2");
	bool f = audioOpen("../Level 3/Song_3.mp3", "l3song3");
	gAudioReady = (a || b || c || d || e || f);

	if (!gAudioReady)
		printf("[audio] no track could be opened - the game runs silently.\n");
}

void musicStop()
{
	if (gCurrentMusic != MUSIC_NONE)
	{
		char cmd[128];
		sprintf_s(cmd, "stop %s", musicAlias(gCurrentMusic));
		mciCommand(cmd);
		sprintf_s(cmd, "seek %s to start", musicAlias(gCurrentMusic));
		mciCommand(cmd);
	}
	gCurrentMusic = MUSIC_NONE;
}

/* stops whatever is playing and loops the requested track from the top */
void musicPlay(int music)
{
	char cmd[128];

	if (music == gCurrentMusic)
		return;

	musicStop();
	gCurrentMusic = music;

	if (music == MUSIC_NONE || gMusicMuted || !gAudioReady)
		return;

	sprintf_s(cmd, "seek %s to start", musicAlias(music));
	mciCommand(cmd);
	sprintf_s(cmd, "play %s repeat", musicAlias(music));
	if (!mciCommand(cmd))
	{
		/* some drivers reject "repeat" - the watchdog below keeps it looping */
		sprintf_s(cmd, "play %s", musicAlias(music));
		mciCommand(cmd);
	}
}

void musicToggleMute()
{
	int playing = gCurrentMusic;

	gMusicMuted = !gMusicMuted;

	if (gMusicMuted)
	{
		musicStop();
		gCurrentMusic = playing;   /* remember it so unmute can resume */
	}
	else
	{
		gCurrentMusic = MUSIC_NONE;
		musicPlay(playing);
	}
}

/* called once a second by a timer - guarantees the loop never dies */
void musicWatchdog()
{
	char cmd[128], mode[64];

	if (!gAudioReady || gMusicMuted || gCurrentMusic == MUSIC_NONE)
		return;

	sprintf_s(cmd, "status %s mode", musicAlias(gCurrentMusic));
	if (!mciQuery(cmd, mode, sizeof(mode)))
		return;

	if (strcmp(mode, "playing") != 0)
	{
		sprintf_s(cmd, "seek %s to start", musicAlias(gCurrentMusic));
		mciCommand(cmd);
		sprintf_s(cmd, "play %s repeat", musicAlias(gCurrentMusic));
		mciCommand(cmd);
	}
}

/* ==================== SOUND EFFECTS ====================
 *
 *  Short one-shot sounds, opened once under their own aliases so they can
 *  play on top of the music. A file that does not exist simply stays
 *  silent - drop the missing mp3 into Audios/ and it starts working.
 */

#define SFX_COLLISION  0
#define SFX_COIN       1
#define SFX_FUEL       2
#define SFX_WIN        3
#define SFX_LOSE       4
#define SFX_BOMB       5     /* level 03 crashes and exploding tanks */
#define SFX_COUNT      6

static const char *sfxAlias[SFX_COUNT] =
{
	"sfxcollision", "sfxcoin", "sfxfuel", "sfxwin", "sfxlose", "sfxbomb"
};

static const char *sfxFile[SFX_COUNT] =
{
	"Audios/collision.mp3",   /* the project's collision sound */
	"Audios/coin.mp3",        /* optional */
	"Audios/fuel.mp3",        /* optional */
	"Audios/win.mp3",         /* optional */
	"Audios/gameover.mp3",    /* the project's lose sound      */
	"../Level 3/bomb collision sound.mp3"
};

bool sfxReady[SFX_COUNT] = { false };

void sfxInit()
{
	int i;
	for (i = 0; i < SFX_COUNT; i++)
		sfxReady[i] = audioOpen(sfxFile[i], sfxAlias[i]);
}

/*  Plays one sound from the beginning. Calling it again restarts it, so a
    single collision event gives exactly one sound.                       */
void sfxPlay(int id)
{
	char cmd[128];

	if (id < 0 || id >= SFX_COUNT || !sfxReady[id] || gMusicMuted)
		return;

	sprintf_s(cmd, "stop %s", sfxAlias[id]);
	mciSendString(cmd, NULL, 0, NULL);
	sprintf_s(cmd, "seek %s to start", sfxAlias[id]);
	mciSendString(cmd, NULL, 0, NULL);
	sprintf_s(cmd, "play %s", sfxAlias[id]);
	mciSendString(cmd, NULL, 0, NULL);
}

void audioShutdown()
{
	int i;
	char cmd[128];

	musicStop();
	mciCommand("close introsong");
	mciCommand("close level01song");
	mciCommand("close level02song");
	mciCommand("close l3song1");
	mciCommand("close l3song2");
	mciCommand("close l3song3");

	for (i = 0; i < SFX_COUNT; i++)
	{
		if (sfxReady[i])
		{
			sprintf_s(cmd, "close %s", sfxAlias[i]);
			mciSendString(cmd, NULL, 0, NULL);
		}
	}
}

#endif
