#include "../relacy/context.hpp"
#include "../relacy/test_params.hpp"

#include <initializer_list>
#include <iostream>

namespace rl {

template <size_t N>
int run_tests_with_scheduler(rl::simulate_f (&tests)[N], std::initializer_list<scheduler_type_e> schedulers) {
    for (auto sched: schedulers)
    {
        std::cout << format((rl::scheduler_type_e)sched) << " tests:" << std::endl;

        for (size_t i = 0; i != N; ++i)
        {
            rl::ostringstream stream;
            rl::test_params params;
            params.search_type = (rl::scheduler_type_e)sched;
            params.iteration_count =
                (params.test_result == rl::test_result_success ? 100000 : 500);
            params.output_stream = &stream;
            params.progress_stream = &stream;
            params.context_bound = 2;
            params.execution_depth_limit = 500;

            if (false == tests[i](params))
            {
                std::cout << std::endl;
                std::cout << "FAILED" << std::endl;
                std::cout << stream.str();
                std::cout << std::endl;
                return 1;
            }
            else
            {
                std::cout << params.test_name << "...OK" << std::endl;
            }
        }
        std::cout << std::endl;
    }
    return 0;
}

} // namespace rl
