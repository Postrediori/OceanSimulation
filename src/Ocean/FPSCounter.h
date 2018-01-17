// FpsCounter.h
#ifndef FPS_COUNTER_H
#define FPS_COUNTER_H

class FPSCounter {
public:
    FPSCounter();

    void update(float t);

public:
    int frame;
    float timePrev;
    int fps;
};

#endif // FPS_COUNTER_H
