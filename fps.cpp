#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <string>
#include <vector>
#include <chrono>
#include "fps.h"


using namespace std;
using namespace std::chrono;
uint64_t launch_time;


namespace FPS {
	float fps;
	float frame_rate;
	float last_tick;

	bool FPS::try_tick() {
		float _now = FPS::get_time_since_launch();
		if (_now < FPS::last_tick + FPS::frame_rate)
			return false;
		FPS::last_tick += FPS::frame_rate;
		return true;
	}

	float FPS::get_time_since_launch() {
		uint64_t _now = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() - launch_time;
		return (float)(_now / 1000) + (float)(_now % 1000) / 1000.0f;
	}

	void FPS::init(float _fps) {
		launch_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		FPS::fps = _fps;
		FPS::frame_rate = 1.0f / _fps;
		FPS::last_tick = FPS::get_time_since_launch();
	}
}
