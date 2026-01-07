#pragma once

#include <string_view>

namespace Lexvi {
    namespace ECS {
        enum class AddError {
            OK = 0,
            ALREADY_EXISTS,
            INVALID_ID
        };

        [[nodiscard]] inline std::string_view GetErrorString(AddError e) {
            switch (e) {
                case Lexvi::ECS::AddError::ALREADY_EXISTS:
                    return "Entity already exists";
                    break;
                case Lexvi::ECS::AddError::INVALID_ID:
                    return "Invalid entity";
                    break;
                default:
                    return "No or unknown error";
                    break;
            };
        }

        enum class GetError {
            OK = 0,
            DOES_NOT_EXISTS,
            INVALID_ID
        };

        [[nodiscard]] inline std::string_view GetErrorString(GetError e) {
            switch (e) {
                case Lexvi::ECS::GetError::DOES_NOT_EXISTS:
                    return "Entity does not exists";
                    break;
                case Lexvi::ECS::GetError::INVALID_ID:
                    return "Invalid entity";
                    break;
                default:
                    return "No or unknown error";
                    break;
            };
        }
    }
}
