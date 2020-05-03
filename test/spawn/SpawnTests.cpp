#include <boost/asio/io_service.hpp>

#include <iostream>

#include <spawn/spawn.hpp>

#include <gtest/gtest.h>

TEST(SpawnTests, givenSpawnedStackfulCoroutine_whenItAllocatesMoreThan64KbOfStaticMemory_shouldCrash) {
    ASSERT_DEATH(
        {
            boost::asio::io_service io_service;
            spawn::spawn(io_service, [&]([[maybe_unused]] spawn::yield_context yield) {
                [[maybe_unused]] volatile char maxStackSize[64 * 1024];
            });
            io_service.run();
            exit(0);
        },
        "");
}

TEST(SpawnTests, givenSpawnedStackfulCoroutine_whenItAllocatesLessThan64KbOfStaticMemory_shouldNotCrash) {
    ASSERT_EXIT(
        {
            boost::asio::io_service io_service;
            spawn::spawn(io_service, [&]([[maybe_unused]] spawn::yield_context yield) {
                [[maybe_unused]] volatile char maxStackSize[63 * 1024];
            });
            io_service.run();
            exit(0);
        },
        ::testing::ExitedWithCode(0), "");
}
