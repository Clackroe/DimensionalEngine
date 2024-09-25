// This Class was derived from Dimensional Engine developed by TheCherno.
#ifndef DM_LOGH
#define DM_LOGH

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <memory>

#include <spdlog/spdlog.h>
//
#include <spdlog/fmt/ostr.h>

//#include <fmt/format.h>
#include <glm/glm.hpp>

namespace Dimensional {

class DMCORE_API Log {
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
    return os << glm::to_string(vector);
}
//
template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}


// Core log macros
#define DM_CORE_TRACE(...) Dimensional::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define DM_CORE_INFO(...) Dimensional::Log::GetCoreLogger()->info(__VA_ARGS__);
#define DM_CORE_WARN(...) Dimensional::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define DM_CORE_ERROR(...) Dimensional::Log::GetCoreLogger()->error(__VA_ARGS__);
#define DM_CORE_CRITICAL(...) Dimensional::Log::GetCoreLogger()->critical(__VA_ARGS__);

// Client log macros
#define DM_TRACE(...) Dimensional::Log::GetClientLogger()->trace(__VA_ARGS__);
#define DM_INFO(...) Dimensional::Log::GetClientLogger()->info(__VA_ARGS__);
#define DM_WARN(...) Dimensional::Log::GetClientLogger()->warn(__VA_ARGS__);
#define DM_ERROR(...) Dimensional::Log::GetClientLogger()->error(__VA_ARGS__);
#define DM_CRITICAL(...) Dimensional::Log::GetClientLogger()->critical(__VA_ARGS__);
#endif
