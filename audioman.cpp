#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <string>
#include <vector>
#include <mmsystem.h>
#include <filesystem>
#include "audioman.h"
#pragma comment(lib, "Winmm.lib")


using namespace std;


namespace AudioMan {
	bool use_audio = false;
	float speed = 1.0f;
	string audio_format;
	string audio_fn;
	string video_fn;

	void AudioMan::clear() {
		if (filesystem::exists(AudioMan::audio_fn) && filesystem::is_regular_file(AudioMan::audio_fn)) {
			filesystem::remove(AudioMan::audio_fn);
		}
	}

	void AudioMan::close() {
		if (AudioMan::use_audio) {
			mciSendStringA("stop _audioman", NULL, NULL, NULL);
			mciSendStringA("close _audioman", NULL, NULL, NULL);
		}
		AudioMan::clear();
	}

	void AudioMan::pause() {
		mciSendStringA("pause _audioman", NULL, NULL, NULL);
	}

	void AudioMan::resume() {
		mciSendStringA("resume _audioman", NULL, NULL, NULL);
	}

	void AudioMan::play() {
		mciSendStringA("play _audioman", NULL, NULL, NULL);
	}

	void AudioMan::init(const char* _video_fn, const char* _audio_format, float _speed, bool _use_audio) {
		AudioMan::audio_format = string(_audio_format);
		AudioMan::video_fn = string(_video_fn);
		AudioMan::audio_fn = AudioMan::video_fn + '.' + AudioMan::audio_format;
		AudioMan::use_audio = _use_audio;
		AudioMan::speed = _speed;
		if (!AudioMan::use_audio) {
			return;
		}
		AudioMan::clear();
		if (system((
			"ffmpeg > nul -i \"" +
			video_fn +
			"\" -q:a 0 -map a \"" +
			audio_fn +
			"\""
		).c_str()) != 0) {
			AudioMan::use_audio = false;
			AudioMan::clear();
			return;
		}
		mciSendStringA(
			("open \"" + AudioMan::audio_fn + "\" alias _audioman").c_str(),
			NULL,
			NULL,
			NULL
		);
		if (speed != 1.0f) {
			mciSendStringA(
				("set _audioman speed " + to_string((int)round(1000.0f * AudioMan::speed))).c_str(),
				NULL,
				NULL,
				NULL
			);
		}
	}
}
