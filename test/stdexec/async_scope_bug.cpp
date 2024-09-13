#include "../../relacy/relacy_std.hpp"
#include "../../relacy/relacy_cli.hpp"

#include <stdexec/execution.hpp>
#include <exec/async_scope.hpp>
#include <exec/static_thread_pool.hpp>
#include <test_common/schedulers.hpp>

#include <chrono>
#include <random>
#include <iostream>

using rl::nvar;
using rl::nvolatile;
using rl::mutex;

namespace ex = stdexec;
using exec::async_scope;

struct async_scope_bug : rl::test_suite<async_scope_bug, 1>
{
    static size_t const dynamic_thread_count = 2;

    void thread(unsigned)
    {
        async_scope scope;

        bool cancelled1{false};
        bool cancelled2{false};

        exec::static_thread_pool pool{2};
        auto s = ex::schedule(pool.get_scheduler());

        {
        ex::sender auto snd1 = scope.spawn_future(ex::on(
            pool.get_scheduler(),
            ex::just() //
            | ex::let_stopped([&] {
                cancelled1 = true;
                return ex::just();
                })));
        (void) snd1;
#if 0
        ex::sender auto snd2 = scope.spawn_future(ex::on(
            pool.get_scheduler(),
            ex::just() //
            | ex::let_stopped([&] {
                cancelled2 = true;
                return ex::just();
                })));
        (void) snd2;
#endif
        }

        ex::sync_wait(scope.on_empty());
    }
};

int main()
{
    rl::test_params p;
    p.iteration_count = 50000;
    p.execution_depth_limit = 10000;
    rl::simulate<async_scope_bug>(p);
    return 0;
}
