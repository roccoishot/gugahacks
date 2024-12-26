#pragma once

#include "./valve_sdk/csgostructs.hpp"
#include "./helpers/math.hpp"
#include "./options.hpp"
#include <d3dx9math.h>

struct record_info {
    float forwardmove;
    float sidemove;
    int buttons;
    QAngle viewangle;
    int number;
};

struct record {
    record() {
    }
    record(Vector vec) {
        start_pos = vec;
    }
    Vector start_pos;
    std::vector<record_info> info;
    int size = 0;
    char* name;
    char* map;
};

class movement_recorder : public  Singleton< movement_recorder > {
public:
    void run(CUserCmd* cmd);
    void record_movement(CUserCmd* cmd);
    void play_movement(CUserCmd* cmd);

    void save_record(record rec);
    void load_records();
    void get_record(record load_record);

    std::vector<record> current_recordings;
    std::vector<record> recordings;
};