#include <iostream>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include <gtest/gtest.h>
#include "async.h"

TEST(BulkServerTest, CommandBulkTest) {
    CommandBulk b;
    EXPECT_TRUE(b.empty());
    EXPECT_FALSE(b.recyclable());

    b.push_command("command1");
    b.push_command("command2");
    EXPECT_FALSE(b.empty());
    EXPECT_EQ(b.size(), 2);
    EXPECT_FALSE(b.recyclable());

    testing::internal::CaptureStdout();
    std::cout << b;
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "bulk: command1, command2\n");

    b.clear();
    EXPECT_TRUE(b.empty());
    EXPECT_EQ(b.size(), 0);
    EXPECT_TRUE(b.recyclable());
}

TEST(BulkServerTest, BulkHandlerTest) {
    BulkHandler bh(3);
    CommandProcessor proc;

    bh.subscribe(&proc);
    while(!proc.ready());

    testing::internal::CaptureStdout();
    bh.add_command("command1");
    bh.add_command("command2");
    bh.add_command("command3");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "bulk: command1, command2, command3\n");

    proc.stop();
}

TEST(BulkServerTest, BulkExclusiveTest) {
    BulkHandler bh(3);
    CommandProcessor proc;

    bh.subscribe(&proc);
    while(!proc.ready());

    CommandBulk* b = bh.exclusive_bulk();
    b->push_command(std::string("c1"));
    b->push_command(std::string("c2"));
    b->push_command(std::string("c3"));
    b->push_command(std::string("c4"));

    testing::internal::CaptureStdout();
    bh.submit_exclusive(b);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "bulk: c1, c2, c3, c4\n");

    proc.stop();
}

TEST(BulkServerTest, ReaderTest) {
    BulkHandler bh(3);
    CommandReader reader(bh);
    CommandProcessor proc;

    bh.subscribe(&proc);
    while(!proc.ready());

    testing::internal::CaptureStdout();
    reader.scan_input("c1");
    reader.scan_input("c2");
    reader.scan_input("c3");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "bulk: c1, c2, c3\n");

    proc.stop();
}

TEST(BulkServerTest, DoubleReaderTest) {
    BulkHandler bh(3);
    CommandReader reader1(bh);
    CommandReader reader2(bh);
    CommandProcessor proc;

    bh.subscribe(&proc);
    while(!proc.ready());

    testing::internal::CaptureStdout();
    reader1.scan_input("c1");
    reader2.scan_input("c2");
    reader1.scan_input("c3");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "bulk: c1, c2, c3\n");

    proc.stop();
}

TEST(BulkServerTest, DoubleReaderBraceTest) {
    BulkHandler bh(5);
    CommandReader reader1(bh);
    CommandReader reader2(bh);
    CommandProcessor proc;

    bh.subscribe(&proc);
    while(!proc.ready());

    testing::internal::CaptureStdout();
    reader1.scan_input("c1");
    reader2.scan_input("c2");
    reader1.scan_input("c3");
    reader2.scan_input("{");
    reader1.scan_input("c4");
    reader2.scan_input("ex1");
    reader2.scan_input("ex2");
    reader1.scan_input("c5");
    reader2.scan_input("ex3");
    reader2.scan_input("}");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output, "bulk: c1, c2, c3\nbulk: ex1, ex2, ex3\n");

    proc.stop();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

