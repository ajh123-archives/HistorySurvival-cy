#pragma once

#include "core/log.hpp"

#if defined(_DEBUG) || defined(NDEBUG)
#define CYBRION_DEBUG
#endif

#ifdef CYBRION_DEBUG
#define CYBRION_ASSERT(b, m)                       \
    do                                             \
    {                                              \
        if (!(b))                                  \
        {                                          \
            CYBRION_ERROR("Assert failed: {}", m); \
        }                                          \
    } while (0);
#else
#define CYBRION_ASSERT(b, m)
#endif

#define CYBRION_STATIC_ERROR(message) static_assert(cybrion::detail::always_false<T> && message);

namespace cybrion
{
#ifdef WIN32
    using time_point = std::chrono::steady_clock::time_point;
#else
    using time_point = std::chrono::system_clock::time_point;
#endif

    namespace detail
    {
        template <class...>
        constexpr std::false_type always_false{};
    };

    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using f32 = float;
    using f64 = double;

    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;

    using ivec2 = glm::ivec2;
    using ivec3 = glm::ivec3;
    using ivec4 = glm::ivec4;

    using uvec2 = glm::uvec2;
    using uvec3 = glm::uvec3;
    using uvec4 = glm::uvec4;

    using mat2 = glm::mat2;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;

    using string = std::string;

    template <typename K, typename V>
    using umap = std::unordered_map<K, V>;

    template <typename V>
    using uset = std::unordered_set<V>;

    template <typename V, u32 size>
    using array = std::array<V, size>;

    template <typename... V>
    using tuple = std::tuple<V...>;

    template <typename V>
    using vector = std::vector<V>;

    template <typename V>
    using stack = std::stack<V>;

    template <typename V>
    using queue = std::queue<V>;

    template <typename T>
    using ref = std::shared_ptr<T>;

    constexpr f32 pi = 3.14159265358979323846;
    constexpr f32 two_pi = pi * 2;
} // namespace nith