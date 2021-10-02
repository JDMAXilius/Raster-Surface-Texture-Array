#include <iostream>
#include "RasterSurface.h"
#include "tiles_12.h"
#include <Windows.h>
//#include <time.h>
#include <random>
#include "XTime.h"
#include "blood_hit_04.h"

unsigned int raster[500 * 500] = { 0, };

unsigned int lerp(unsigned int A, unsigned int B);
unsigned int colorConverter(unsigned int color);

//A function to clear the color buffer to a solid color of your choice
//not used for now!
void clearColor(unsigned color)
{
	std::memset(raster, color, sizeof raster);
}

//A function to convert 2 dimensional coordinates to a 1 dimensional coordinate
int convertDimensionalCordinates(int X, int Y, int Width)//find coordinates
{
	int conversion = (X + Y * Width);
	return conversion;
}

//A function to draw a pixel (fill a certain pixel with a specific color)
void drawPixel(int x, int y, unsigned int color)
{
	unsigned int alphaColor = (color & 0xFF000000);
	alphaColor >> 24;

	if (alphaColor < 255)
	{
		raster[convertDimensionalCordinates(x, y, 500)] = lerp(raster[convertDimensionalCordinates(x, y, 500)], color);
	}
	else
	{
		raster[convertDimensionalCordinates(x, y, 500)] = color;
	}

}

//A function to BLIT (Block Image Transfer)
void blit(const unsigned int* source, int nSourceTextureWidth, RECT SourceRect, int DestX, int DestY)
{
	int heigth = SourceRect.bottom - SourceRect.top;
	int width = SourceRect.right - SourceRect.left;

	for (int i = 0; i < heigth; i++)
	{
		for (int j = 0; j < width; j++)
		{
			drawPixel(j + DestX, i + DestY, colorConverter(source[convertDimensionalCordinates(j + SourceRect.left, i + SourceRect.top, nSourceTextureWidth)]));
		}
	}

}

//A function to clear the color buffer to a solid color of your choice.
//Swap the color bits from BGRA to ARGB and get 10 random objects in the color array
unsigned int lerp(unsigned int A, unsigned int B)
{
	//A colors
	unsigned int Aalpha = (A & 0xFF000000) >> 24;
	unsigned int Ared = (A & 0x00FF0000) >> 16;
	unsigned int Agreen = (A & 0x0000FF00) >> 8;
	unsigned int Ablue = (A & 0x000000FF);

	//B colors
	unsigned int Balpha = (B & 0xFF000000) >> 24;
	unsigned int Bred = (B & 0x00FF0000) >> 16;
	unsigned int Bgreen = (A & 0x0000FF00) >> 8;
	unsigned int Bblue = (A & 0x000000FF);
	
	//Ratio
	float ratio = Balpha / 255.0f;

	//Result ofthe learp colors
	unsigned int Calpha = (Balpha - Aalpha) * ratio + Aalpha;
	unsigned int Cred = (Bred - Ared) * ratio + Ared;
	unsigned int Cgreen = (Bgreen - Agreen) * ratio + Agreen;
	unsigned int Cblue = (Bblue - Ablue) * ratio + Ablue;

	unsigned int Lerp = (Calpha << 24) | (Cred << 16) | (Cgreen << 8) | (Cblue);
	return Lerp;
}

unsigned int colorConverter(unsigned int color)
{
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	unsigned int alpha;

	//colors
	red = (color & 0x0000FF00) << 8;
	green = (color & 0x00FF0000) >> 8;
	blue = (color & 0xFF000000) >> 24;
	alpha = (color & 0x000000FF) << 24;

	//RGBA
	unsigned int RGBA = alpha | red | green | blue;
	return RGBA;

}

//Get at least 1 “cell” of an animation rendering properly with blending
void Animation(const unsigned int* source, int nSourceTextureWidth, int pixelX, int pixelY, int cutFrame, int x, int y)
{
	for (int i = 0; i < cutFrame; i++)
	{
		for (int j = 0; j < cutFrame; j++)
		{
			drawPixel(x + j, y + i, colorConverter(source[convertDimensionalCordinates(pixelX + j, pixelY + i, nSourceTextureWidth)]));
		}
	}
}

void PlayAnimation(float FPS, int frameX, int frameY)
{
	if (FPS > (1.0f / 50))
	{
		frameX = frameX + 130;

		if (frameX > blood_hit_04_width + 1)
		{
			frameY = frameY + 130;
			frameX = 0;

			if (frameY > blood_hit_04_height + 1)
			{
				frameY = 0;
			}

		}

		FPS = 0;
	}
}

int main()
{
	RS_Initialize(500, 500);
	
	//Get a tiled background showing on the entire color array
	RECT grass;
	grass.left = 286;
	grass.right = 320;
	grass.top = 125;
	grass.bottom = 160;

	RECT redHouse;
	redHouse.left = 128;
	redHouse.right = 191;
	redHouse.top = 32;
	redHouse.bottom = 98;

	RECT blueHouse;
	blueHouse.left = 0;
	blueHouse.right = 120;
	blueHouse.top = 0;
	blueHouse.bottom = 98;

	RECT garden;
	garden.left = 0;
	garden.right = 170;
	garden.top = 110;
	garden.bottom = 190;

	RECT tree;
	tree.left = 320;
	tree.right = 400;
	tree.top = 12;
	tree.bottom = 98;

	// Random textures
	srand(time(NULL));
	std::vector<int> randNum;

	for (int i = 0; i < 20; i++)
	{
		randNum.push_back(rand() % 458);
	}

	//alternative version
	unsigned int* randNumA = new unsigned int[20];
	for (size_t i = 0; i < 20; i++)
	{
		randNumA[i] = rand() % 458;
	}


	//Play an animation with a high speed timer ( The XTime class is great for this)
	int cutFrame = 128;
	float FPS = 0.0f;
	int frameX = 0;
	int frameY = 0;

	XTime timer;
	timer.Restart();

	do {
		
		//time
		float delta = timer.Delta();
		float tTime = timer.TotalTime();
		timer.Signal();
		FPS += delta;

		//Get a tiled background
		for (int i = 0; i < (500 / (grass.right - grass.left)) + 1; i++)
		{
			for (int j = 0; j < (500 / (grass.bottom - grass.top)) + 1; j++)
			{
				blit(tiles_12_pixels, tiles_12_width, grass, i * (grass.right - grass.left), j * (grass.bottom - grass.top));
			}
		}

		//Get tiles props textures
		for (int i = 0; i < 20; i += 3)
		{
			blit(tiles_12_pixels, tiles_12_width, redHouse, randNumA[i], randNumA[i+1]);
		}

		for (int i = 0; i < 20; i += 2)
		{
			blit(tiles_12_pixels, tiles_12_width, tree, randNum[i], randNum[i + 1]);
		}

		for (int i = 0; i < 1; i += 1)
		{
			blit(tiles_12_pixels, tiles_12_width, blueHouse,200, 200);
		}

		for (int i = 0; i < 1; i += 1)
		{
			blit(tiles_12_pixels, tiles_12_width, garden, 175, 300);
		}

		//Play an animation with a high speed timer ( The XTime class is great for this)
		//PlayAnimation( FPS, frameX, frameY);//is not working as a function 
		if (FPS > (1.0f / 50))
		{
			frameX = frameX + 128;

			if (frameX > blood_hit_04_width + 1)
			{
				frameY = frameY + 130;
				frameX = 0;

				if (frameY > blood_hit_04_height + 1)
				{
					frameY = 0;
				}
			}
			FPS = 0;
		}
		Animation(blood_hit_04_pixels, blood_hit_04_width, frameX, frameY, cutFrame, 200, 100);

	} while (RS_Update(raster, 250000));

	delete[] randNumA;
	
	RS_Shutdown();
	return 0;

}