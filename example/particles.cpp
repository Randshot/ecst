// Copyright (c) 2015-2016 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0
// http://vittorioromeo.info | vittorio.romeo@outlook.com

#include <random>
#include <iostream>
#include <chrono>
#include <ecst.hpp>
#include "../test/ecst/settings_generator.hpp"

namespace example
{
    using ft = float;

    using vrm::core::uint;
    using vrm::core::sz_t;

    namespace component
    {
        struct position
        {
            float _x;
            float _y;
        };

        struct velocity
        {
            float _x;
            float _y;
        };

        struct acceleration
        {
            float _x;
            float _y;
        };

        struct curve
        {
            float _radians;
        };

        struct color
        {
        };

        struct circle_shape
        {
            float _radius;
        };

        struct life
        {
            float _v;
        };
    }

    namespace c = example::component;

    namespace ct
    {
        namespace sct = ecst::signature::component;

        constexpr auto position = sct::tag<c::position>;
        constexpr auto velocity = sct::tag<c::velocity>;
        constexpr auto acceleration = sct::tag<c::acceleration>;
        constexpr auto curve = sct::tag<c::curve>;
        constexpr auto color = sct::tag<c::color>;
        constexpr auto circle_shape = sct::tag<c::circle_shape>;
        constexpr auto life = sct::tag<c::life>;
    }

    namespace actions
    {
        void accelerate(ft dt, c::velocity& v, const c::acceleration& a)
        {
            v._x += a._x * dt;
            v._y += a._y * dt;
        }

        void move(ft dt, c::position& p, const c::velocity& v)
        {
            p._x += v._x * dt;
            p._y += v._y * dt;
        }

        void curve(ft dt, c::velocity& v, const c::curve& c)
        {
            (void)dt;
            (void)v;
            (void)c;

            // vec2f vel_vector{v._x, v._y};

            // auto vel_radians = ssvs::getRad(vel_vector);
            // auto vel_magnitude = ssvs::getMag(vel_vector);

            // auto new_radians = vel_radians + c._radians * dt;
            // auto new_vel = ssvs::getVecFromRad(new_radians, vel_magnitude);

            // v._x = new_vel.x;
            // v._y = new_vel.y;
        }

        void hurt(ft dt, c::life& l)
        {
            l._v -= dt;
        }

        void fade(c::color& c, c::circle_shape& cs, const c::life& l)
        {
            (void)c;

            cs._radius = l._v * 0.2f;
        }
    }


#define SYS_TAG(x)                                                  \
    namespace system                                                \
    {                                                               \
        struct x;                                                   \
    }                                                               \
    namespace st                                                    \
    {                                                               \
        constexpr auto x = ecst::tag::system::v<system::x>; \
    }

    SYS_TAG(acceleration)
    SYS_TAG(velocity)
    SYS_TAG(curve)
    SYS_TAG(fade)
    SYS_TAG(life)
    SYS_TAG(render_colored_circle)


    namespace system
    {
        struct curve
        {
            template <typename TData>
            void process(ft dt, TData& data)
            {
                data.for_entities([&](auto eid)
                    {
                        auto& v = data.get(ct::velocity, eid);
                        const auto& c = data.get(ct::curve, eid);
                        actions::curve(dt, v, c);
                    });
            }
        };


        struct render_colored_circle
        {
            void prepare()
            {
            }

            template <typename TRenderTarget>
            void render_on(TRenderTarget&)
            {
            }

            template <typename TRenderTarget, typename TData>
            void process(TRenderTarget, TData& data)
            {
                auto& va = data.output();
                va.clear();

                data.for_entities([this, &va, &data](auto eid)
                    {
                        const auto& p = data.get(ct::position, eid);
                        // const auto& c = data.get(ct::color, eid);
                        auto& cs = data.get(ct::circle_shape, eid);

                        // actions::update_circle(cs, p, c);

                        auto mk_triangle = [this, &va, &data, &p, &cs](
                            auto, auto)
                        {

                            // va.emplace_back(1);
                            // va.emplace_back(2);
                            // va.emplace_back(3);

                            // va.emplace_back(p0, color);
                            // va.emplace_back(p1, color);
                            // va.emplace_back(p2, color);
                        };

                        float tau = 6.28f;
                        sz_t precision = 3;
                        float inc = tau / precision;

                        for(sz_t i = 0; i < precision; ++i)
                        {
                            mk_triangle(inc * i, inc * (i + 1));
                        }
                    });
            }
        };

        struct acceleration
        {
            template <typename TData>
            void process(ft dt, TData& data)
            {
                (void)dt;


                data.for_entities([&](auto eid)
                    {
                        auto& v = data.get(ct::velocity, eid);
                        const auto& a = data.get(ct::acceleration, eid);
                        actions::accelerate(dt, v, a);
                    });
            }
        };


        struct velocity
        {
            template <typename TData>
            void process(ft dt, TData& data)
            {
                data.for_previous_outputs(st::acceleration, [](auto&, auto&&)
                    {
                    });

                data.for_entities([&](auto eid)
                    {
                        auto& p = data.get(ct::position, eid);
                        const auto& v = data.get(ct::velocity, eid);
                        actions::move(dt, p, v);
                    });
            }
        };


        struct life
        {
            template <typename TData>
            void process(ft dt, TData& data)
            {
                data.for_entities([&](auto eid)
                    {
                        auto& l = data.get(ct::life, eid);
                        actions::hurt(dt, l);

                        if(l._v <= 0.f)
                        {
                            data.kill_entity(eid);
                        }
                    });
            }
        };

        struct fade
        {
            template <typename TData>
            void process(TData& data)
            {
                data.for_entities([&](auto eid)
                    {
                        const auto& l = data.get(ct::life, eid);
                        // auto& c = data.get(ct::color, eid);
                        auto& cs = data.get(ct::circle_shape, eid);
                        // actions::fade(c, cs, l);

                        (void)l;
                        // (void)c;
                        (void)cs;
                    });
            }
        };
    }



    constexpr auto entity_count = ecst::sz_v<1000000>;

    namespace ecst_setup
    {
        constexpr auto make_csl()
        {
            namespace c = example::component;
            namespace slc = ecst::signature_list::component;

            return slc::v<                                 // .
                c::position, c::velocity, c::acceleration, // .
                c::curve, c::color,                        // .
                c::circle_shape, c::life                   // .
                >;
        }

        constexpr auto make_ssl()
        {
            using ecst::sz_v;

            namespace c = example::component;
            namespace s = example::system;
            namespace ss = ecst::signature::system;
            namespace sls = ecst::signature_list::system;

            namespace ips = ecst::inner_parallelism::strategy;
            namespace ipc = ecst::inner_parallelism::composer;


            constexpr auto test_p2 = ips::none::v();
            constexpr auto test_p =                       // .
                ipc::none_below_threshold::v(sz_v<10000>, // .
                    ips::split_evenly_fn::v_cores()       // .
                    );

            (void)test_p2;

            constexpr auto ssig_acceleration =    // .
                ss::make<s::acceleration>(        // .
                    test_p,                       // .
                    ss::no_dependencies,          // .
                    ss::component_use(            // .
                        ss::mutate<c::velocity>,  // .
                        ss::read<c::acceleration> // .
                        ),                        // .
                    ss::output::none              // .
                    );

            constexpr auto ssig_curve =              // .
                ss::make<s::curve>(                  // .
                    test_p,                          // .
                    ss::depends_on<s::acceleration>, // .
                    ss::component_use(               // .
                        ss::mutate<c::velocity>,     // .
                        ss::read<c::curve>           // .
                        ),                           // .
                    ss::output::none                 // .
                    );

            constexpr auto ssig_velocity =       // .
                ss::make<s::velocity>(           // .
                    test_p,                      // .
                    ss::depends_on<s::curve>,    // .
                    ss::component_use(           // .
                        ss::mutate<c::position>, // .
                        ss::read<c::velocity>    // .
                        ),                       // .
                    ss::output::none             // .
                    );

            constexpr auto ssig_render_colored_circle = // .
                ss::make<s::render_colored_circle>(     // .
                    test_p,                             // .
                    ss::depends_on<s::velocity>,        // .
                    ss::component_use(                  // .
                        ss::mutate<c::circle_shape>,    // .
                        ss::read<c::position>,          // .
                        ss::read<c::color>              // .
                        ),                              // .
                    ss::output<std::vector<int>>  // .
                    );

            constexpr auto ssig_life =      // .
                ss::make<s::life>(          // .
                    test_p,                 // .
                    ss::no_dependencies,    // .
                    ss::component_use(      // .
                        ss::mutate<c::life> // .
                        ),                  // .
                    ss::output::none        // .
                    );

            constexpr auto ssig_fade =               // .
                ss::make<s::fade>(                   // .
                    test_p,                          // .
                    ss::depends_on<s::life>,         // .
                    ss::component_use(               // .
                        ss::mutate<c::color>,        // .
                        ss::mutate<c::circle_shape>, // .
                        ss::read<c::life>            // .
                        ),                           // .
                    ss::output::none                 // .
                    );


            return sls::make(               // .
                ssig_acceleration,          // .
                ssig_curve,                 // .
                ssig_velocity,              // .
                ssig_render_colored_circle, // .
                ssig_life,                  // .
                ssig_fade                   // .
                );
        }
    }

    namespace c = example::component;
    namespace s = example::system;

    std::random_device rnd_device;
    std::default_random_engine rnd_gen{rnd_device()};

    auto rndf = [](float min, float max)
    {
#if 1
        return max;
        (void)min;
        (void)max;
#else
        using dist_t = std::uniform_real_distribution<float>;
        return dist_t(min, max)(rnd_gen);
#endif
    };

    template <typename TContext>
    class game_app
    {
    private:
        TContext& _ctx;

        void init_loops()
        {
            std::chrono::high_resolution_clock hrc;
            using ft_dur = std::chrono::duration<ft, std::ratio<1, 1000>>;

            ft dt = 1;

            while(true)
            {
                auto cb = hrc.now();
                (void)cb;

                update_ctx(dt);

                dt = 0.5f;

                if(!_running)
                {
                    break;
                }
            }
        }

        template <typename TProxy>
        void mk_particle(TProxy& proxy)
        {
            auto eid = proxy.create_entity();

            auto& ca = proxy.add_component(ct::acceleration, eid);
            ca._x = rndf(-2, 2);
            ca._y = rndf(-2, 2);

            auto& cv = proxy.add_component(ct::velocity, eid);
            cv._x = rndf(-5, 5);
            cv._y = rndf(-5, 5);

            auto& cp = proxy.add_component(ct::position, eid);
            cp._x = rndf(0, 1024);
            cp._y = rndf(0, 768);

            auto& ccrv = proxy.add_component(ct::curve, eid);
            ccrv._radians = rndf(-5, 5);

            proxy.add_component(ct::color, eid);

            auto& ccs = proxy.add_component(ct::circle_shape, eid);
            (void)ccs;

            auto& cl = proxy.add_component(ct::life, eid);
            static int x = 0;

            cl._v = 10 + x;
            x += 10;
            x = x % 60;
        }

        bool _running = true;

        void update_ctx(ft dt)
        {
            // SSVU_BENCHMARK_INIT_GROUP_SCOPE_EXIT("execution");
            // SSVU_BENCHMARK_RUN_GROUP_SCOPE_EXIT("execution");

            namespace sea = ::ecst::system_execution_adapter;

            _ctx.step([this, dt](auto& proxy)
                {
                    proxy.system(st::render_colored_circle).prepare();

                    proxy.execute_systems( // .
                        sea::t(
                            st::acceleration, st::velocity, st::curve, st::life)
                            .for_subtasks([dt](auto& s, auto& data)
                                {
                                    s.process(dt, data);
                                }),
                        sea::t(st::fade).for_subtasks([](auto& s, auto& data)
                            {
                                s.process(data);
                            }),
                        sea::t(st::render_colored_circle)
                            .for_subtasks([this, dt](auto& s, auto& data)
                                {
                                    s.process(0, data);
                                }));

                    if(!_ctx.any_entity_in(st::life))
                    {
                        _running = false;
                    }
                });
        }

        void init()
        {
            _ctx.step([this](auto& proxy)
                {
                    for(sz_t i = 0; i < entity_count; ++i)
                    {
                        this->mk_particle(proxy);
                    }
                });

            init_loops();
        }

    public:
        game_app(TContext& ctx) : _ctx{ctx}
        {
            init();
        }
    };
}

int main()
{
    using namespace example;
    using namespace example::ecst_setup;

    auto test_impl = [&](auto& ctx)
    {
        using ct = std::remove_reference_t<decltype(ctx)>;
        game_app<ct> a{ctx};
        (void)a;
    };

    test::run_tests(test_impl, entity_count, make_csl(), make_ssl());

    return 0;
}
