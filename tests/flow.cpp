#include "flow.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>


namespace test {


static std::vector<const Runner*>& getRunners()
{
    static std::vector<const Runner*> runners;
    return runners;
}


int Runner::getCount()
{
    return getRunners().size();
}


const Runner& Runner::get(int idx)
{
    return *getRunners()[idx];
}


Runner::Runner(std::string_view name, void (&fn)())
    : name{name}
    , fn{fn}
{
    auto& r = getRunners();

    const auto iter = std::lower_bound(r.begin(), r.end(), name,
        [&](const Runner* runner, std::string_view name)
        {
            return runner->name < name;
        });
    if (iter == r.end() || (*iter)->name != name)
        r.insert(iter, this);
    else {
        std::cerr
            << "Runner \"" << name << "\" is already registered"
            << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


std::string_view Runner::getName() const
{
    return name;
}


void Runner::run() const
{
    fn();
}


static int numFailures;


void failure(std::string_view message)
{
    ++numFailures;
    std::cerr << message << std::endl;
}


int getNumFailures()
{
    return numFailures;
}


}
