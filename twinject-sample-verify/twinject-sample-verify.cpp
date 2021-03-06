/*
 * The sole purpose of this program is to check if the samples were
 * collected properly by reading them then drawing a visualization
 */

#include "stdafx.h"
#include "SDL.h"

#include <iostream>
#include <fstream>
#include <vector>

union th_kbd_state
{
	struct {
		bool shot;		// 0
		bool bomb;		// 1
		bool slow;		// 2
		bool skip;		// 3
		bool up;		// 4
		bool left;		// 5
		bool down;		// 6
		bool right;		// 7
	};
	bool keys[8];
};

struct vec2
{
	float x, y;
};

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const int SAMPLE_SIZE = 64;

struct sample
{
	int sampleCount;
	std::vector<vec2> pos;
	std::vector<vec2> vel;
	th_kbd_state keys;
};

th_kbd_state u8_to_kbd_state(uint8_t s)
{
	return th_kbd_state{
		(bool)(s & (1 << 0)),
		(bool)(s & (1 << 1)),
		(bool)(s & (1 << 2)),
		(bool)(s & (1 << 3)),
		(bool)(s & (1 << 4)),
		(bool)(s & (1 << 5)),
		(bool)(s & (1 << 6)),
		(bool)(s & (1 << 7)),
	};
}

struct key_graphic
{
	int keyIdx;
	int x, y;
};

key_graphic keys[] = {
	{4, 1, 0},
	{6,1,1},
	{5,0,1},
	{7,2,1},
	{2,0,0}
};

SDL_Window* SDLWindow = NULL;
SDL_Surface* SDLSurface = NULL;
SDL_Renderer* SDLRenderer = NULL;

bool CSDL_Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDLWindow = SDL_CreateWindow("twinject-sample-verify", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (SDLWindow == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDLSurface = SDL_GetWindowSurface(SDLWindow);
	if (SDLSurface == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	SDLRenderer = SDL_CreateRenderer(SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (SDLRenderer == NULL)
	{
		printf("SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	return true;
}

void CSDL_Destroy()
{
	SDL_DestroyRenderer(SDLRenderer);
	SDL_FreeSurface(SDLSurface);
	SDL_DestroyWindow(SDLWindow);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	if (!CSDL_Init())
		return 1;
	std::cout << "twinject-sample-verify" << std::endl;

	std::ifstream ifs("D:\\Programming\\Multi\\th15\\nn_response.samp", std::ios::in | std::ios::binary);
	if (ifs.fail())
	{
		std::cerr << "file not found" << std::endl;
		return 1;
	}

	std::vector<sample> samples;
	samples.clear();
	int sampleCount;
	while (ifs.read((char*)&sampleCount, sizeof(int)))	// this is also disgusting
	{
		sample samp;
		samp.sampleCount = sampleCount;
		for (int i = 0; i < sampleCount; ++i)
		{
			vec2 p, v;
			ifs.read((char*)&p.x, sizeof(float));
			ifs.read((char*)&p.y, sizeof(float));
			ifs.read((char*)&v.x, sizeof(float));
			ifs.read((char*)&v.y, sizeof(float));
			samp.pos.push_back(p);
			samp.vel.push_back(v);
		}
		uint8_t keyu8;
		ifs.read((char*)&keyu8, sizeof(uint8_t));
		samp.keys = u8_to_kbd_state(keyu8);
		samples.push_back(samp);
	}
	std::cout << "loaded " << samples.size() << " samples" << std::endl;

	std::vector<sample>::const_iterator s = samples.cbegin();

	SDL_Rect rects[SAMPLE_SIZE] = { 0 };
	SDL_Rect center = { WINDOW_WIDTH / 2 - 2, WINDOW_HEIGHT / 2 - 2,4,4 };

	bool quit = false;
	SDL_Event e;
	while (!quit && s != samples.cend())
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
		SDL_SetRenderDrawColor(SDLRenderer, 0, 0, 0, 0xFF);
		SDL_RenderClear(SDLRenderer);

		SDL_SetRenderDrawColor(SDLRenderer, 0, 255, 0, 0xFF);
		SDL_RenderFillRect(SDLRenderer, &center);

		SDL_SetRenderDrawColor(SDLRenderer, 0, 0, 255, 0xFF);
		for (int i = 0; i < s->sampleCount; ++i)
		{
			int x = (int)s->pos[i].x;
			int y = (int)s->pos[i].y;
			if (x == 0 && y == 0)
				continue;
			rects[i].x = WINDOW_WIDTH / 2 + x - 2;
			rects[i].y = WINDOW_HEIGHT / 2 + y - 2;
			rects[i].w = 4;
			rects[i].h = 4;
			SDL_RenderDrawLine(SDLRenderer,
				WINDOW_WIDTH / 2 + x, WINDOW_HEIGHT / 2 + y,
				WINDOW_WIDTH / 2 + x + (int)(s->vel[i].x * 10),
				WINDOW_HEIGHT / 2 + y + (int)(s->vel[i].y * 10));
		}
		SDL_SetRenderDrawColor(SDLRenderer, 255, 0, 0, 0xFF);
		SDL_RenderFillRects(SDLRenderer, rects, s->sampleCount);

		SDL_SetRenderDrawColor(SDLRenderer, 0, 255, 0, 0xFF);
		for (int i = 0; i < 5; i++)
		{
			int w = 10;
			int h = 10;
			int x = keys[i].x * w + 10;
			int y = keys[i].y * h + 10;
			SDL_Rect rect = { x,y,w,h };
			if (s->keys.keys[keys[i].keyIdx])
				SDL_RenderFillRect(SDLRenderer, &rect);
		}
		SDL_RenderPresent(SDLRenderer);
		++s;
	}

	CSDL_Destroy();
	return 0;
}

