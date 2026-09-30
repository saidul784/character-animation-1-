/*
 *  Config.h  --  Honda Rush: Career Rider
 *
 *  Global window settings, the game-state machine and small drawing
 *  helpers used by every screen.
 *
 *  NOTE: this header (and every other .h / .hpp written for this game)
 *        is included ONLY from iMain.cpp, and always AFTER "iGraphics.h".
 *        iGraphics.h has no include guard of its own, so it must never be
 *        included a second time.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* -------------------- WINDOW -------------------- */
/* 1280x720 keeps the same 16:9 shape as the 1364x768 street backdrops,
   so level art is displayed without any distortion.                     */
#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720
#define GAME_TITLE     "Honda Rush: Career Rider"

/* -------------------- GAME STATES -------------------- */
enum GameState
{
	STATE_MENU = 0,   /* intro poster + main menu   */
	STATE_KEYS,       /* keyboard control list      */
	STATE_ABOUT,      /* project proposal summary   */
	STATE_LEVEL01,    /* gameplay - level 01        */
	STATE_LEVEL02,    /* gameplay - level 02        */
	STATE_LEVEL03,    /* gameplay - level 03        */
	STATE_INTRO       /* new intro poster + MENU    */
};

GameState gState = STATE_INTRO;

/* -------------------- THEME (Honda red / carbon black) -------------------- */
#define COL_BG_R        14
#define COL_BG_G        14
#define COL_BG_B        18

#define COL_PANEL_R     28
#define COL_PANEL_G     28
#define COL_PANEL_B     34

#define COL_RED_R      216
#define COL_RED_G       24
#define COL_RED_B       32

#define COL_TEXT_R     236
#define COL_TEXT_G     236
#define COL_TEXT_B     240

#define COL_MUTED_R    150
#define COL_MUTED_G    150
#define COL_MUTED_B    160

/* -------------------- SMALL DRAW HELPERS -------------------- */

/*  iGraphics takes char* rather than const char*; these wrappers keep the
    call sites clean and warning free.                                     */
void drawText(double x, double y, const char *str, void *font = GLUT_BITMAP_HELVETICA_18)
{
	iText(x, y, (char *)str, font);
}

int textWidth(const char *str, void *font = GLUT_BITMAP_HELVETICA_18)
{
	return glutBitmapLength(font, (const unsigned char *)str);
}

void drawTextCentered(double centerX, double y, const char *str, void *font = GLUT_BITMAP_HELVETICA_18)
{
	drawText(centerX - textWidth(str, font) / 2.0, y, str, font);
}

unsigned int loadImage(const char *path)
{
	unsigned int tex = iLoadImage((char *)path);
	if (tex == 0)
		printf("[assets] FAILED to load: %s\n", path);
	return tex;
}

/* ==================== SPRITE SHEETS ====================
 *
 *  iShowImage() always draws a whole texture, so a sprite sheet cannot be
 *  drawn frame by frame from a single texture. Instead the sheet is cut up
 *  once at load time: the file is read with stb_image (already bundled with
 *  iGraphics), sliced into cols x rows frames, and every frame is uploaded
 *  as its own OpenGL texture. After that each frame is an ordinary image
 *  that iShowImage() can draw.
 *
 *  Two clean-up passes happen while loading:
 *
 *   1. If the file has no real alpha channel (a checkerboard painted into
 *      the pixels, or a flat studio background), the background colour is
 *      keyed out. Only pixels connected to the edge of a frame are removed,
 *      so dark tyres or grey chrome inside the bike survive.
 *
 *   2. The tightest box containing all visible pixels of ALL frames is
 *      computed, and every frame is cropped to that same box. This strips
 *      the empty padding without changing how the frames line up, so the
 *      wheels sit exactly on the ground and the animation does not jitter.
 */

#define MAX_SPRITE_FRAMES 32

struct SpriteSheet
{
	unsigned int frame[MAX_SPRITE_FRAMES];
	int count;                 /* frames actually loaded    */
	int frameW, frameH;        /* size of one frame, cropped */
};

/* sum of the channel differences between two colours */
static int spriteColorDiff(const unsigned char *a, const unsigned char *b)
{
	return abs((int)a[0] - (int)b[0]) + abs((int)a[1] - (int)b[1]) + abs((int)a[2] - (int)b[2]);
}

/*  Clears the background of one frame: flood fills inward from the frame
    border over every pixel that matches a colour sampled along that border. */
static void spriteKeyOutFrame(unsigned char *px, int imgW, int fx, int fy, int fw, int fh)
{
	const int TOLERANCE = 45;
	const int SAMPLES   = 24;

	unsigned char key[6][3];
	int keyCount = 0;
	int i, k, sp = 0;
	int *stack;
	unsigned char *seen;

	/* ---- sample the four borders for background colours ---- */
	for (i = 0; i < SAMPLES * 4; i++)
	{
		int side = i / SAMPLES;
		int t    = (i % SAMPLES) * (side < 2 ? fw : fh) / SAMPLES;
		int x    = (side == 0 || side == 1) ? t : (side == 2 ? 0 : fw - 1);
		int y    = (side == 0) ? 0 : (side == 1 ? fh - 1 : t);
		unsigned char *p = px + (((fy + y) * imgW) + (fx + x)) * 4;
		int isNew = 1;

		for (k = 0; k < keyCount; k++)
			if (spriteColorDiff(key[k], p) <= TOLERANCE)
				isNew = 0;

		if (isNew && keyCount < 6)
		{
			key[keyCount][0] = p[0];
			key[keyCount][1] = p[1];
			key[keyCount][2] = p[2];
			keyCount++;
		}
	}

	seen  = (unsigned char *)calloc(fw * fh, 1);
	stack = (int *)malloc(fw * fh * sizeof(int));
	if (seen == NULL || stack == NULL) { free(seen); free(stack); return; }

	/*  A pixel is marked the moment it is PUSHED, not when it is popped.
	    Marking on pop let the same pixel be pushed by each of its four
	    neighbours, so on a big picture with a large flat background the
	    stack could grow past its fw*fh allocation and run off the end of
	    the buffer. Marking on push means every pixel is stacked at most
	    once, which keeps sp within bounds by construction.             */
	#define L_PUSH(nIdx) do { int _n = (nIdx); if (!seen[_n]) { seen[_n] = 1; stack[sp++] = _n; } } while (0)

	/* ---- seed the fill with every border pixel ---- */
	for (i = 0; i < fw; i++)
	{
		L_PUSH(i);                          /* top row    */
		L_PUSH((fh - 1) * fw + i);          /* bottom row */
	}
	for (i = 0; i < fh; i++)
	{
		L_PUSH(i * fw);                     /* left  edge */
		L_PUSH(i * fw + fw - 1);            /* right edge */
	}

	/* ---- flood fill ---- */
	while (sp > 0)
	{
		int idx = stack[--sp];
		int x   = idx % fw;
		int y   = idx / fw;
		unsigned char *p;
		int matches = 0;

		p = px + (((fy + y) * imgW) + (fx + x)) * 4;
		for (k = 0; k < keyCount; k++)
			if (spriteColorDiff(key[k], p) <= TOLERANCE)
				matches = 1;

		if (!matches)
			continue;

		p[3] = 0;   /* background -> fully transparent */

		if (x > 0)      L_PUSH(idx - 1);
		if (x < fw - 1) L_PUSH(idx + 1);
		if (y > 0)      L_PUSH(idx - fw);
		if (y < fh - 1) L_PUSH(idx + fw);
	}

	#undef L_PUSH

	free(seen);
	free(stack);
}

/*  Loads one sprite sheet and slices it into cols x rows frames.
    Frame 0 is the top-left frame, then left to right, row by row.       */
bool loadSpriteSheet(SpriteSheet *sheet, const char *path, int cols, int rows)
{
	int imgW, imgH, bpp;
	int fw, fh, r, c, x, y, i;
	int minX, minY, maxX, maxY;
	int cropW, cropH;
	int hasAlpha = 0;
	unsigned char *data, *frameBuf;

	sheet->count  = 0;
	sheet->frameW = 0;
	sheet->frameH = 0;

	if (cols < 1 || rows < 1 || cols * rows > MAX_SPRITE_FRAMES)
	{
		printf("[sheet] bad layout %d x %d for %s\n", cols, rows, path);
		return false;
	}

	data = stbi_load(path, &imgW, &imgH, &bpp, 4);
	if (data == NULL)
	{
		printf("[sheet] FAILED to load: %s\n", path);
		return false;
	}

	fw = imgW / cols;
	fh = imgH / rows;

	/* does the file carry real transparency of its own? */
	for (i = 3; i < imgW * imgH * 4; i += 4)
		if (data[i] < 255) { hasAlpha = 1; break; }

	if (!hasAlpha)
		for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++)
				spriteKeyOutFrame(data, imgW, c * fw, r * fh, fw, fh);

	/* ---- union bounding box of the visible pixels of every frame ----
	 *
	 *  A row or column only counts as part of the picture when enough of it
	 *  is really opaque. Keying a chequerboard out of a photo always leaves
	 *  a few stray specks and a faint drop shadow behind, and a box drawn
	 *  around those pads the sprite with empty space - which is exactly what
	 *  makes a bike look like it is floating above the road.
	 */
	{
		int *colHits = (int *)calloc(fw, sizeof(int));
		int *rowHits = (int *)calloc(fh, sizeof(int));
		int colMin = fh / 40;
		int rowMin = fw / 40;

		if (colMin < 6) colMin = 6;
		if (rowMin < 6) rowMin = 6;

		if (colHits == NULL || rowHits == NULL)
		{
			free(colHits);
			free(rowHits);
			stbi_image_free(data);
			return false;
		}

		for (r = 0; r < rows; r++)
			for (c = 0; c < cols; c++)
				for (y = 0; y < fh; y++)
					for (x = 0; x < fw; x++)
						if (data[(((r * fh + y) * imgW) + (c * fw + x)) * 4 + 3] > 40)
						{
							colHits[x]++;
							rowHits[y]++;
						}

		minX = fw; minY = fh; maxX = -1; maxY = -1;

		for (x = 0; x < fw; x++)
			if (colHits[x] >= colMin)
			{
				if (x < minX) minX = x;
				if (x > maxX) maxX = x;
			}

		for (y = 0; y < fh; y++)
			if (rowHits[y] >= rowMin)
			{
				if (y < minY) minY = y;
				if (y > maxY) maxY = y;
			}

		free(colHits);
		free(rowHits);
	}

	if (maxX < minX || maxY < minY)      /* nothing visible - keep it whole */
	{
		minX = 0; minY = 0; maxX = fw - 1; maxY = fh - 1;
	}

	cropW = maxX - minX + 1;
	cropH = maxY - minY + 1;

	/* ---- upload every frame as its own texture ---- */
	frameBuf = (unsigned char *)malloc(cropW * cropH * 4);
	if (frameBuf == NULL) { stbi_image_free(data); return false; }

	for (r = 0; r < rows; r++)
	{
		for (c = 0; c < cols; c++)
		{
			unsigned int tex;

			for (y = 0; y < cropH; y++)
				memcpy(frameBuf + y * cropW * 4,
				       data + ((((r * fh) + minY + y) * imgW) + (c * fw) + minX) * 4,
				       cropW * 4);

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cropW, cropH, 0,
			             GL_RGBA, GL_UNSIGNED_BYTE, frameBuf);

			sheet->frame[sheet->count++] = tex;
		}
	}

	free(frameBuf);
	stbi_image_free(data);

	sheet->frameW = cropW;
	sheet->frameH = cropH;

	printf("[sheet] %s : %d frames, %dx%d each (source %dx%d, %s)\n",
	       path, sheet->count, cropW, cropH, imgW, imgH,
	       hasAlpha ? "own alpha" : "background keyed out");
	return true;
}

/*  Loads an animation whose frames are separate files, e.g.
        "bike images/frame_%02d.png" with firstIndex 1 and count 8.
    All frames must share the same pixel size. They are cropped with one
    shared bounding box so the bike does not jitter between frames.       */
bool loadImageSequence(SpriteSheet *sheet, const char *pathFormat, int firstIndex, int count)
{
	unsigned char *img[MAX_SPRITE_FRAMES];
	unsigned char *frameBuf;
	char path[300];
	int w = 0, h = 0, iw, ih, bpp;
	int i, x, y;
	int minX, minY, maxX, maxY, cropW, cropH;

	sheet->count  = 0;
	sheet->frameW = 0;
	sheet->frameH = 0;

	if (count < 1 || count > MAX_SPRITE_FRAMES)
		return false;

	for (i = 0; i < count; i++)
	{
		sprintf_s(path, pathFormat, firstIndex + i);
		img[i] = stbi_load(path, &iw, &ih, &bpp, 4);

		if (img[i] == NULL)
		{
			printf("[frames] FAILED to load: %s\n", path);
			while (--i >= 0) stbi_image_free(img[i]);
			return false;
		}

		if (i == 0) { w = iw; h = ih; }
		else if (iw != w || ih != h)
		{
			printf("[frames] %s is %dx%d but frame 1 is %dx%d - sizes must match\n",
			       path, iw, ih, w, h);
			while (i >= 0) stbi_image_free(img[i--]);
			return false;
		}
	}

	/* key the background out of any frame that has no alpha of its own */
	for (i = 0; i < count; i++)
	{
		int hasAlpha = 0, p;
		for (p = 3; p < w * h * 4; p += 4)
			if (img[i][p] < 255) { hasAlpha = 1; break; }
		if (!hasAlpha)
			spriteKeyOutFrame(img[i], w, 0, 0, w, h);
	}

	/* one bounding box covering the visible pixels of every frame */
	minX = w; minY = h; maxX = -1; maxY = -1;

	for (i = 0; i < count; i++)
		for (y = 0; y < h; y++)
			for (x = 0; x < w; x++)
				if (img[i][(y * w + x) * 4 + 3] > 8)
				{
					if (x < minX) minX = x;
					if (x > maxX) maxX = x;
					if (y < minY) minY = y;
					if (y > maxY) maxY = y;
				}

	if (maxX < minX || maxY < minY)
	{
		minX = 0; minY = 0; maxX = w - 1; maxY = h - 1;
	}

	cropW = maxX - minX + 1;
	cropH = maxY - minY + 1;

	frameBuf = (unsigned char *)malloc(cropW * cropH * 4);
	if (frameBuf == NULL)
	{
		for (i = 0; i < count; i++) stbi_image_free(img[i]);
		return false;
	}

	for (i = 0; i < count; i++)
	{
		unsigned int tex;

		for (y = 0; y < cropH; y++)
			memcpy(frameBuf + y * cropW * 4,
			       img[i] + (((minY + y) * w) + minX) * 4,
			       cropW * 4);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cropW, cropH, 0,
		             GL_RGBA, GL_UNSIGNED_BYTE, frameBuf);

		sheet->frame[sheet->count++] = tex;
		stbi_image_free(img[i]);
	}

	free(frameBuf);
	sheet->frameW = cropW;
	sheet->frameH = cropH;

	printf("[frames] %s : %d frames, %dx%d each\n", pathFormat, sheet->count, cropW, cropH);
	return true;
}

/*  Same as loadImageSequence, but for frames whose file names do not follow
    a number pattern. Every pose is cropped with ONE shared bounding box, so
    a set of poses of the same vehicle keeps its registration: the wheels
    stay on exactly the same line whichever pose is drawn.                */
bool loadImageList(SpriteSheet *sheet, const char **paths, int count)
{
	unsigned char *img[MAX_SPRITE_FRAMES];
	unsigned char *frameBuf;
	int w = 0, h = 0, iw, ih, bpp;
	int i, x, y;
	int minX, minY, maxX, maxY, cropW, cropH;

	sheet->count  = 0;
	sheet->frameW = 0;
	sheet->frameH = 0;

	if (count < 1 || count > MAX_SPRITE_FRAMES)
		return false;

	for (i = 0; i < count; i++)
	{
		img[i] = stbi_load(paths[i], &iw, &ih, &bpp, 4);

		if (img[i] == NULL)
		{
			printf("[frames] FAILED to load: %s\n", paths[i]);
			while (--i >= 0) stbi_image_free(img[i]);
			return false;
		}

		if (i == 0) { w = iw; h = ih; }
		else if (iw != w || ih != h)
		{
			printf("[frames] %s is %dx%d but the first is %dx%d - sizes must match\n",
			       paths[i], iw, ih, w, h);
			while (i >= 0) stbi_image_free(img[i--]);
			return false;
		}
	}

	/* only needed if a file arrives without real transparency */
	for (i = 0; i < count; i++)
	{
		int hasAlpha = 0, p;
		for (p = 3; p < w * h * 4; p += 4)
			if (img[i][p] < 255) { hasAlpha = 1; break; }
		if (!hasAlpha)
			spriteKeyOutFrame(img[i], w, 0, 0, w, h);
	}

	minX = w; minY = h; maxX = -1; maxY = -1;

	for (i = 0; i < count; i++)
		for (y = 0; y < h; y++)
			for (x = 0; x < w; x++)
				if (img[i][(y * w + x) * 4 + 3] > 8)
				{
					if (x < minX) minX = x;
					if (x > maxX) maxX = x;
					if (y < minY) minY = y;
					if (y > maxY) maxY = y;
				}

	if (maxX < minX || maxY < minY)
	{
		minX = 0; minY = 0; maxX = w - 1; maxY = h - 1;
	}

	cropW = maxX - minX + 1;
	cropH = maxY - minY + 1;

	frameBuf = (unsigned char *)malloc(cropW * cropH * 4);
	if (frameBuf == NULL)
	{
		for (i = 0; i < count; i++) stbi_image_free(img[i]);
		return false;
	}

	for (i = 0; i < count; i++)
	{
		unsigned int tex;

		for (y = 0; y < cropH; y++)
			memcpy(frameBuf + y * cropW * 4,
			       img[i] + (((minY + y) * w) + minX) * 4,
			       cropW * 4);

		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cropW, cropH, 0,
		             GL_RGBA, GL_UNSIGNED_BYTE, frameBuf);

		sheet->frame[sheet->count++] = tex;
		stbi_image_free(img[i]);
	}

	free(frameBuf);
	sheet->frameW = cropW;
	sheet->frameH = cropH;

	printf("[frames] %d poses, %dx%d each (one shared crop)\n", sheet->count, cropW, cropH);
	return true;
}

/*  Axis aligned box overlap - the collision test used all over level 01. */
bool rectsOverlap(double ax, double ay, double aw, double ah,
                  double bx, double by, double bw, double bh)
{
	return (ax < bx + bw) && (ax + aw > bx) && (ay < by + bh) && (ay + ah > by);
}

/*  Sprite PNGs have soft, semi-transparent edges. iGraphics only enables
    alpha *testing*, which leaves a hard fringe, so we switch real alpha
    blending on once after the window exists.                              */
void enableAlphaBlending()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*  A 1px outline rectangle drawn thick, used for button / panel borders. */
void drawBorder(double left, double bottom, double w, double h, int thickness)
{
	int i;
	for (i = 0; i < thickness; i++)
		iRectangle(left + i, bottom + i, w - 2 * i, h - 2 * i);
}

/*  Fills the whole window with the base background colour. */
void drawBackdrop()
{
	iSetColor(COL_BG_R, COL_BG_G, COL_BG_B);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/*  Standard screen header: red bar + title, used by Keys / About. */
void drawScreenHeader(const char *title)
{
	iSetColor(COL_RED_R, COL_RED_G, COL_RED_B);
	iFilledRectangle(0, SCREEN_HEIGHT - 74, SCREEN_WIDTH, 74);

	iSetColor(255, 255, 255);
	drawText(48, SCREEN_HEIGHT - 48, title, GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	drawText(SCREEN_WIDTH - 300, SCREEN_HEIGHT - 45, "ESC  -  BACK TO MENU", GLUT_BITMAP_HELVETICA_18);
}

#endif
