#define ANKERL_NANOBENCH_IMPLEMENT
#include <antity/antity.h>
#include <nanobench.h>

#include <antity/core/archetype.hpp>
#include <entt/entt.hpp>

enum ECS : uint8_t { _entt, _ant };

template <ECS ecs>
const char *to_string() {
    if constexpr (ecs == ECS::_entt) {
        return "EnTT";
    }
    if constexpr (ecs == ECS::_ant) {
        return "antity";
    }
}

struct position {
    float x;
    float y;
};

struct speed {
    float x;
    float y;
};

struct acceleration {
    float x;
    float y;
};

template <ECS ecs>
class reg {};

template <>
class reg<_entt> {
   public:
    using type = typename entt::registry;
    using entity_type = entt::registry::entity_type;

    entity_type create() { return entityManager.create(); };

    template <typename C, typename... Args>
    void add_component(entity_type entity_t, Args &&...args) {
        entityManager.emplace<C>(entity_t, std::forward<Args>(args)...);
    }

    template <typename... Cs>
    auto get_components() {
        return entityManager.view<Cs...>().each();
    }

    template<typename F>
    auto for_each(F&& f) {}

   private:
    type entityManager;
};

template <>
class reg<_ant> {
   public:
    using type = typename ant::registry;
    using entity_type = ant::registry::entity_type;
    entity_type create() { return entityManager.create(); };

    template <typename C, typename... Args>
    void add_component(entity_type entity_t, Args &&...args) {
        entityManager.add<C>(entity_t, std::forward<Args>(args)...);
    }

    template <typename... Cs>
    auto get_components() {
        return entityManager.get<Cs...>();
    }

    template <typename F>
    auto for_each(F &&f) {
        return entityManager.for_each(std::forward<F>(f));
    }

   private:
    type entityManager;
};

template <ECS ecs>
void EmptyEntitiesBench(size_t count) {
    auto registry = reg<ecs>();
    ankerl::nanobench::Bench().run(
        (std::to_string(count) + " empty entities : " + to_string<ecs>()), [&] {
            for (size_t i = 0; i < count; i++) {
                auto entity_t = registry.create();
            }
        });
}

template <ECS ecs, size_t comp_count>
void EntitiesWithComps(size_t count) {
    auto registry = reg<ecs>();
    ankerl::nanobench::Bench().run(
        std::to_string(count) + " entities with " + std::to_string(comp_count)
            + " Comp " + to_string<ecs>(),
        [&] {
            for (size_t i = 0; i < count; i++) {
                auto entity_t = registry.create();
                if constexpr (comp_count >= 1) {
                    registry.add_component<position>(entity_t,
                                                     position{.5f, .8f});
                }
                if constexpr (comp_count >= 2) {
                    registry.add_component<speed>(entity_t, speed{.5f, .8f});
                }
                if constexpr (comp_count >= 3) {
                    registry.add_component<acceleration>(
                        entity_t, acceleration{.5f, .8f});
                }
            }
        });
}

template <ECS ecs, size_t comp_count>
void SystemBench(size_t count) {
    auto registry = reg<ecs>();
    for (size_t i = 0; i < count; i++) {
        auto entity_t = registry.create();
        if constexpr (comp_count >= 1) {
            registry.add_component<position>(entity_t, position{.5f, .8f});
        }
        if constexpr (comp_count >= 2) {
            registry.add_component<speed>(entity_t, speed{.5f, .8f});
        }
        if constexpr (comp_count >= 3) {
            registry.add_component<acceleration>(entity_t,
                                                 acceleration{.5f, .8f});
        }
    }

    ankerl::nanobench::Bench().run(
        std::to_string(count) + " entities with " + std::to_string(comp_count)
            + " Comp Update " + to_string<ecs>(),
        [&] {
            if constexpr (comp_count == 1) {
                for (auto [ent, pos] : registry.get_components<position>()) {
                    pos.x += .1f;
                }
            }
            if constexpr (comp_count == 2) {
                for (auto [ent, pos, s] :
                     registry.get_components<position, speed>()) {
                    pos.x += s.x;
                    pos.y += s.y;
                }
            }
            if constexpr (comp_count == 3) {
                for (auto [ent, pos, s, accel] :
                     registry.get_components<position, speed, acceleration>()) {
                    pos.x += s.x;
                    pos.y += s.y;
                    s.x += accel.x;
                    s.y += accel.y;
                }
            }
        });
}

template <size_t comp_count>
void antt_for_each_bench(size_t count) {
    auto registry = reg<ECS::_ant>();
    for (size_t i = 0; i < count; i++) {
        auto entity_t = registry.create();
        if constexpr (comp_count >= 1) {
            registry.add_component<position>(entity_t, position{.5f, .8f});
        }
        if constexpr (comp_count >= 2) {
            registry.add_component<speed>(entity_t, speed{.5f, .8f});
        }
        if constexpr (comp_count >= 3) {
            registry.add_component<acceleration>(entity_t,
                                                 acceleration{.5f, .8f});
        }
    }
    ankerl::nanobench::Bench().run(
        std::to_string(count) + " ant | entities with " + std::to_string(comp_count)
            + " ant::for_each ",
        [&] {
            if constexpr (comp_count == 1) {
                registry.for_each(
                    [](ant::entity_t e, position &pos) { pos.x += .1f; });
            }
            if constexpr (comp_count == 2) {
                registry.for_each([](ant::entity_t e, position &pos, speed &s) {
                    pos.x += s.x;
                    pos.y += s.y;
                });
            }
            if constexpr (comp_count == 3) {
                registry.for_each([](ant::entity_t e, position &pos, speed &s,
                                     acceleration &accel) {
                    pos.x += s.x;
                    pos.y += s.y;
                    s.x += accel.x;
                    s.y += accel.y;
                });
            }
        });
}

void EmptyEntitiesBenchmmark(const std::vector<size_t> &v) {
    for (auto i : v) {
        EmptyEntitiesBench<_entt>(i);
        EmptyEntitiesBench<_ant>(i);
    }
}

void CompBenchmark(const std::vector<size_t> &v) {
    // for (auto i : v)
    //{
    //    EntitiesWithComps<_entt, 1>(i);
    //    EntitiesWithComps<_ant, 1>(i);
    //}
    // for (auto i : v)
    //{
    //    EntitiesWithComps<_entt, 2>(i);
    //    EntitiesWithComps<_ant, 2>(i);
    //}
    // for (auto i : v)
    //{
    //    EntitiesWithComps<_entt, 3>(i);
    //    EntitiesWithComps<_ant, 3>(i);
    //}
    for (auto i : v) {
        SystemBench<_entt, 1>(i);
        //SystemBench<_ant, 1>(i);
        antt_for_each_bench<1>(i);
    }
    for (auto i : v) {
        SystemBench<_entt, 2>(i);
        //SystemBench<_ant, 2>(i);
        antt_for_each_bench<2>(i);
    }
    for (auto i : v) {
        SystemBench<_entt, 3>(i);
        //SystemBench<_ant, 3>(i);
        antt_for_each_bench<3>(i);
    }
}

void UtilityBenchmark() {
    ankerl::nanobench::Bench().run("get_type_signature",
                                   [&] { ant::get_type_signature<int>(); });

    ankerl::nanobench::Bench().run("getSingature",
                                   [&] { ant::get_signature<int>(); });
}

int main() {
    std::vector<size_t> v = {1, 10, 100, 1000, 100000, 1000000};
    // EmptyEntitiesBenchmmark(v);
    UtilityBenchmark();

    CompBenchmark(v);
}
