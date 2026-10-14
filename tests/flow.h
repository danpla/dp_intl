#pragma once

#include <stdexcept>
#include <string_view>


namespace test {


class Runner {
public:
    static int getCount();
    static const Runner& get(int idx);

    Runner(std::string_view name, void (&fn)());

    std::string_view getName() const;
    void run() const;
private:
    std::string_view name;
    void (*fn)();
};


#define TEST_REGISTER(FN) \
static test::Runner FN ## TestRunner(#FN, FN)


// Report a test case failure, increment the failure counter, and
// continue.
void failure(std::string_view message);


// Return the number of failure() calls.
int getNumFailures();


// An exception for an abnormal error. The exception is intended for
// errors that are not directly related to test cases, e.g. an IO
// error when setting up a test. It only expected to be thrown from
// within a function registered with TEST_REGISTER().
class FatalError : public std::runtime_error {
    using runtime_error::runtime_error;
};


}
