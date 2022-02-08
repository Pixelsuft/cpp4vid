#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <string>
#include <vector>
#include <SDL.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "argparse.h"
#include "audioman.h"
#include "fps.h"


using namespace std;
using namespace cv;


bool running = true;


int SDL_main(int argc, char** argv) {
	ArgParse::parse(argc, argv);
	string video_fn(ArgParse::get_vfn());
	AudioMan::init(ArgParse::get_vfn(), ArgParse::get_af(), ArgParse::get_speed(), ArgParse::get_enable_sound());
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << "Failed to init SDL2: " << SDL_GetError() << endl;
		exit(1);
	}
	VideoCapture cap(video_fn);
	if (!cap.isOpened()) {
		cout << "Failed to load video" << endl;
		exit(1);
	}
	float fps = (float)cap.get(CAP_PROP_FPS);
	SDL_Window* window = SDL_CreateWindow(
		("cpp4vid - " + video_fn + " - " + to_string((int)round((float)cap.get(CAP_PROP_FRAME_COUNT) / fps)) + "s").c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		(int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT),
		0
	);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		cout << "Failed create renderer: " << SDL_GetError() << endl;
		exit(1);
	}

	Mat frame;
	SDL_Texture* tex = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, frame.cols,
		frame.rows
	);
	SDL_UpdateTexture(tex, NULL, (void*)frame.data, (int)frame.step1());
	SDL_RenderCopyEx(renderer, tex, NULL, NULL, 0, NULL, SDL_FLIP_NONE);;
	cap >> frame;
	if (frame.empty()) {
		running = false;
	}
	AudioMan::play();
	FPS::init(fps * ArgParse::get_speed());

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}
		if (!FPS::try_tick()) {
			continue;
		}
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(tex);
		cap >> frame;
		if (frame.empty()) {
			running = false;
			break;
		}
		tex = SDL_CreateTexture(
			renderer, SDL_PIXELFORMAT_BGR24, SDL_TEXTUREACCESS_STREAMING, frame.cols,
			frame.rows
		);
		SDL_UpdateTexture(tex, NULL, (void*)frame.data, (int)frame.step1());
		SDL_RenderCopyEx(renderer, tex, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
	}

	AudioMan::close();
	cap.release();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
