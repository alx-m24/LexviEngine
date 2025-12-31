#pragma once

namespace Lexvi {
    namespace Time {
        void Init();
        void Update();

        double GetTime(); // seconds since engine start
        double GetDeltaTime(); // seconds since last frame
    }
}
