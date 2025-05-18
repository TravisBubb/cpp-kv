#include <gtest/gtest.h>
#include "../include/channel.h"

#include <thread>

// Basic send/receive
TEST(ChannelBasic, SendAndReceive) {
    Channel<int> ch;
    ch.send(10);
    auto v = ch.receive();
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(v.value(), 10);
}

// FIFO ordering
TEST(ChannelBasic, Order) {
    Channel<std::string> ch;
    ch.send("a");
    ch.send("b");
    EXPECT_EQ(*ch.receive(), "a");
    EXPECT_EQ(*ch.receive(), "b");
}

// close() drains then EOF
TEST(ChannelClose, DrainThenEOF) {
    Channel<int> ch;
    ch.send(1);
    ch.send(2);
    ch.close();
    EXPECT_EQ(*ch.receive(), 1);
    EXPECT_EQ(*ch.receive(), 2);
    EXPECT_FALSE(ch.receive().has_value());
}

// Multi-threaded producer/consumer
TEST(ChannelThreaded, Concurrent) {
    Channel<int> ch;
    std::thread prod([&](){
        for(int i=0;i<1000;i++) ch.send(i);
        ch.close();
    });
    int sum = 0;
    std::thread cons([&](){
        while(auto v = ch.receive())
            sum += *v;
    });
    prod.join();
    cons.join();
    EXPECT_EQ(sum, 1000*999/2);
}

