#include <cstdlib>
#include <iomanip>
#include <ios>
#include <iostream>

#include "flow.h"


int main()
{
    for (int i{}; i < test::Runner::getCount(); ++i) {
        const auto& runner = test::Runner::get(i);
        std::cout
            << std::right << i + 1 << '/'
            << test::Runner::getCount() << ' '
            << runner.getName()
            // Flush to make sure that failure() messages (written to
            // stderr) are nested under the test name.
            << std::endl;

        try {
            runner.run();
        } catch (const test::FatalError& e) {
            std::cerr << "FATAL ERROR\n";
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "===\n";
    const auto numFailures = test::getNumFailures();
    if (numFailures == 0)
        std::cout << "Everything is OK\n";
    else
        std::cout
            << numFailures
            << " failure" << (numFailures > 1 ? "s" : "")
            << '\n';

    return numFailures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
