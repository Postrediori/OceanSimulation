#include "stdafx.h"
#include "FPSCounter.h"

FPSCounter::FPSCounter()
    : frame(0)
    , timePrev(0.f)
    , fps(0.f) {
}

void FPSCounter::update(float t) {
    frame++;

    int dt = (int)(t - timePrev);
    if (dt < 1) {
        return;
    }

    fps = frame / (float)dt;
    timePrev = t;
    frame = 0;
}
