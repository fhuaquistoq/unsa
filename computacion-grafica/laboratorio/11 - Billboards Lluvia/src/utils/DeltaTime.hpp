#pragma once

class DeltaTime
{
public:
    static float get();
    static void reset();
    static void update();

private:
    static float lastTime;
    static float deltaTime;
};
