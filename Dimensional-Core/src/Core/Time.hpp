#ifndef DM_TIME_H
#define DM_TIME_H
namespace Dimensional {
class DMCORE_API Time {
public:
    static float deltaTime() { return m_DeltaTime; }
    static float getTime();

    static void Update()
    {
        calculateDelta();
    };

    static void calculateDelta();

private:
    static float m_DeltaTime;
    static float m_LastFrame;
};
}
#endif
