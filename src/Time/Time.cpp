#include "LexviEngine/pch.hpp"
#include "LexviEngine/Time/Time.hpp"

namespace Lexvi {
    namespace Time {
        namespace {
            // TODO: globals here
            using Clock = std::chrono::steady_clock;
            using TimePoint = Clock::time_point;

            TimePoint s_startTime;
            TimePoint s_lastFrameTime;

            double s_deltaTime = 0.0;
        }

        void Init() {
            s_startTime = Clock::now();
            s_lastFrameTime = s_startTime;
        }

        void Update() {
            TimePoint now = Clock::now();

            s_deltaTime = std::chrono::duration<double>(now - s_lastFrameTime).count();
            s_deltaTime = std::min(s_deltaTime, 0.1);

            s_lastFrameTime = now;
        }

        double GetTime() {
            return std::chrono::duration<double>(Clock::now() - s_startTime).count();
        }

        double GetDeltaTime() {
            return s_deltaTime;
        }
    }
}
