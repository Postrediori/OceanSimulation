// FpsCounter.h
#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

struct FPSCounter {
    int frame;
    int timePrev;
    float fps;

    FPSCounter() : frame(0), timePrev(0), fps(0.f) { }
    ~FPSCounter() { }

    void update(int t) {
        frame++;

        int dt = t - timePrev;
        if (dt<1000) return;

        fps = frame * 1000.f / (float)dt;
        timePrev = t;
        frame = 0;
    }
};

#endif // FPS_COUNTER_H
