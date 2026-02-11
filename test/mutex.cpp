#include "../relacy/relacy_cli.hpp"

#include "test_runner.hpp"

namespace shared_mutex_tests {

struct test_shared_mutex_ops : rl::test_suite<test_shared_mutex_ops, 1>
{
    rl::shared_mutex mtx;

    void thread(unsigned)
    {
          while (!mtx.try_lock($))
            ; // empty
          mtx.unlock($);
          while (!mtx.try_lock_shared($))
            ; // empty
          mtx.unlock_shared($);
    }
};


struct test_shared_mutex_exclusive_basic : rl::test_suite<test_shared_mutex_exclusive_basic, 2>
{
    rl::shared_mutex mtx;
    rl::var<int> data;

    void before()
    {
        data($) = 0;
    }

    void after()
    {
        RL_ASSERT(data($) == 2);
    }

    void thread(unsigned)
    {
        mtx.lock($);
        data($) += 1;
        mtx.unlock($);
    }
};

struct test_shared_mutex_mixed_usage : rl::test_suite<test_shared_mutex_mixed_usage, 4>
{
    rl::shared_mutex mtx;
    rl::var<int> data;

    void before()
    {
        data($) = 0;
    }

    void after()
    {
        RL_ASSERT(data($) == 5);
    }

    void thread(unsigned index)
    {
        if (index == 0) {
            for (int i = 0; i != 5; ++i) {
                mtx.lock($);
                data($) += 1;
                mtx.unlock($);
            }
        } else {
          mtx.lock_shared($);
          int val = data($);
          RL_ASSERT(val >= 0 && val <= 5);
          mtx.unlock_shared($);
        }
    }
};


struct test_shared_mutex_unlock_shared_error : rl::test_suite<test_shared_mutex_unlock_shared_error, 1, rl::test_result_unlocking_mutex_wo_ownership>
{
    void thread(unsigned)
    {
        rl::shared_mutex m;
        m.lock_shared($);
        m.unlock_shared($);
        m.unlock_shared($);
    }
};


struct test_shared_mutex_unlock_exclusive_error : rl::test_suite<test_shared_mutex_unlock_exclusive_error, 1, rl::test_result_unlocking_mutex_wo_ownership>
{
    void thread(unsigned)
    {
        rl::shared_mutex m;
        m.lock($);
        m.unlock($);
        m.unlock($);
    }
};


struct test_shared_mutex_upgrade_error : rl::test_suite<test_shared_mutex_upgrade_error, 1, rl::test_result_mutex_read_to_write_upgrade>
{
    void thread(unsigned)
    {
        rl::shared_mutex m;
        m.lock_shared($);
        m.lock($);
    }
};


struct test_shared_mutex_downgrade_error : rl::test_suite<test_shared_mutex_downgrade_error, 1, rl::test_result_mutex_write_to_read_upgrade>
{
    void thread(unsigned)
    {
        rl::shared_mutex m;
        m.lock($);
        m.lock_shared($);
    }
};

} // namespace shared_mutex_tests

int main()
{
    rl::test_params p;
    p.iteration_count = 100;

    rl::simulate_f tests[] = {
      &rl::simulate<shared_mutex_tests::test_shared_mutex_ops>,
      &rl::simulate<shared_mutex_tests::test_shared_mutex_exclusive_basic>,
      &rl::simulate<shared_mutex_tests::test_shared_mutex_unlock_shared_error>,
      &rl::simulate<shared_mutex_tests::test_shared_mutex_unlock_exclusive_error>,
      &rl::simulate<shared_mutex_tests::test_shared_mutex_upgrade_error>,
      &rl::simulate<shared_mutex_tests::test_shared_mutex_downgrade_error>
    };
    run_tests_with_scheduler(tests, {rl::sched_random, rl::sched_bound, rl::sched_full});

    return 0;
}
