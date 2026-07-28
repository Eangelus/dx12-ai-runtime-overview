#include "../include/mock_device.hpp"
#include "../include/scoped_guard.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

void test_normal_scope_opens_and_closes_resource() {
    MockDevice dev;
    {
        ScopedGuard<MockDevice> guard(dev);
        assert(dev.is_open());
    }
    assert(!dev.is_open());
    assert(dev.begin_count() == 1);
    assert(dev.end_count() == 1);
}

void test_exception_inside_scope_still_closes_resource() {
    MockDevice dev;
    bool caught = false;
    try {
        ScopedGuard<MockDevice> guard(dev);
        assert(dev.is_open());
        throw std::runtime_error("simulated failure mid-scope");
    } catch (const std::runtime_error&) {
        caught = true;
    }
    assert(caught);
    // Kernaussage des Musters: trotz Exception ist die Resource sauber
    // geschlossen, nicht "haengend" offen geblieben.
    assert(!dev.is_open());
    assert(dev.end_count() == 1);
}

void test_nested_scopes_are_independent() {
    MockDevice dev1;
    MockDevice dev2;
    {
        ScopedGuard<MockDevice> outer(dev1);
        {
            ScopedGuard<MockDevice> inner(dev2);
            assert(dev1.is_open() && dev2.is_open());
        }
        assert(dev1.is_open() && !dev2.is_open());
    }
    assert(!dev1.is_open() && !dev2.is_open());
}

void test_sequential_scopes_reuse_the_same_resource() {
    MockDevice dev;
    {
        ScopedGuard<MockDevice> first(dev);
    }
    {
        ScopedGuard<MockDevice> second(dev);
    }
    assert(dev.begin_count() == 2);
    assert(dev.end_count() == 2);
}

int main() {
    test_normal_scope_opens_and_closes_resource();
    test_exception_inside_scope_still_closes_resource();
    test_nested_scopes_are_independent();
    test_sequential_scopes_reuse_the_same_resource();
    std::cout << "All tests passed.\n";
    return 0;
}
