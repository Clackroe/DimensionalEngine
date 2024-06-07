// This Class was derived from Voxler Engine developed by TheCherno.
#ifndef VX_LOGH
#define VX_LOGH

#define GLM_ENABLE_EXPERIMENTAL
// #include <glm/gtx/string_cast.hpp>
#include <memory>

#include <spdlog/spdlog.h>
//
#include <spdlog/fmt/ostr.h>

namespace Voxler {

class Log {
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

}

// template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
// inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
// {
//     return os << glm::to_string(vector);
// }
//
// template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
// inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
// {
//     return os << glm::to_string(matrix);
// }
//
// template <typename OStream, typename T, glm::qualifier Q>
// inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
// {
//     return os << glm::to_string(quaternion);
// }

// Core log macros
#define VX_CORE_TRACE(...) Voxler::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define VX_CORE_INFO(...) Voxler::Log::GetCoreLogger()->info(__VA_ARGS__);
#define VX_CORE_WARN(...) Voxler::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define VX_CORE_ERROR(...) Voxler::Log::GetCoreLogger()->error(__VA_ARGS__);
#define VX_CORE_CRITICAL(...) Voxler::Log::GetCoreLogger()->critical(__VA_ARGS__);

// Client log macros
#define VX_TRACE(...) Voxler::Log::GetClientLogger()->trace(__VA_ARGS__);
#define VX_INFO(...) Voxler::Log::GetClientLogger()->info(__VA_ARGS__);
#define VX_WARN(...) Voxler::Log::GetClientLogger()->warn(__VA_ARGS__);
#define VX_ERROR(...) Voxler::Log::GetClientLogger()->error(__VA_ARGS__);
#define VX_CRITICAL(...) Voxler::Log::GetClientLogger()->critical(__VA_ARGS__);
#endif
