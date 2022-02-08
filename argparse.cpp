#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <string>
#include <vector>
#include <filesystem>
#include "argparse.h"


using namespace std;


namespace ArgParse {
	string video_fn;
	string audio_format;
	bool enable_sound = true;
	float speed = 1.0f;

	const char* ArgParse::get_vfn() {
		return ArgParse::video_fn.c_str();
	}

	const char* ArgParse::get_af() {
		return ArgParse::audio_format.c_str();
	}

	float ArgParse::get_speed() {
		return ArgParse::speed;
	}

	bool ArgParse::get_enable_sound() {
		return ArgParse::enable_sound;
	}

	void ArgParse::parse(int argc, char** argv) {
		if (argc <= 1) {
			cout << "Usage: \"" << (argc > 0 ? argv[0] : "") << "\" \"%file_name%\" [--no-sound] [--audio-format %format%] [--speed %speed%]";
			exit(0);
		}

		ArgParse::video_fn = string(argv[1]);
		if (!filesystem::exists(video_fn) || !filesystem::is_regular_file(video_fn)) {
			cout << "File \"" << argv[1] << "\" not found!";
			exit(1);
		}

		for (int i = 2; i < argc; i++) {
			string arg(argv[i]);
			if (arg == "--no-sound") {
				ArgParse::enable_sound = false;
				continue;
			}
			if (arg == "--audio-format" && i + 1 < argc) {
				i++;
				ArgParse::audio_format = string(argv[i]);
				continue;
			}
			if (arg == "--speed" && i + 1 < argc) {
				i++;
				string speed_str;
				string speed_arg(argv[i]);
				bool used_float = false;
				for (int j = 0; j < speed_arg.size(); j++) {
					if (isdigit(speed_arg.at(j))) {
						speed_str += speed_arg.at(j);
						continue;
					}
					if (!used_float && (speed_arg.at(j) == '.' || speed_arg.at(j) == ',')) {
						used_float = true;
						speed_str += '.';
						continue;
					}
				}
				if (speed_str.size() <= 0) {
					continue;
				}
				ArgParse::speed = (float)stof(speed_str);
				continue;
			}
			cout << "Unknown Argument: " << arg << endl;
			exit(1);
		}

		if (ArgParse::enable_sound) {
			if (system("ffmpeg > nul -version") != 0) {
				ArgParse::enable_sound = false;
			}
			else if (audio_format.size() <= 0) {
				ArgParse::audio_format = (ArgParse::speed == 1.0f ? "wav" : "mp3");
			}
		}
	}
}
