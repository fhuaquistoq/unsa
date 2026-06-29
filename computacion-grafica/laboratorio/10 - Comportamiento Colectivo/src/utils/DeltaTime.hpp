#pragma once

class DeltaTime
{
public:
    static float get();
    static void update();

private:
    static float lastTime;
    static float deltaTime;
};
