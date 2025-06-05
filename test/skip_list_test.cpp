#include <gtest/gtest.h>
#include "../include/skip_list.hpp"

class SkipListTest : public ::testing::Test {
protected:
    SkipList<int> list;

    void SetUp() override {
        // optional: pre-fill
    }

    void TearDown() override {
        list.clear();
    }
};

TEST_F(SkipListTest, InsertAndContains) {
    EXPECT_TRUE(list.insert(10));
    EXPECT_TRUE(list.contains(10));
    EXPECT_FALSE(list.insert(10));  // дубликат
    EXPECT_TRUE(list.insert(20));
    EXPECT_TRUE(list.insert(5));
    EXPECT_TRUE(list.contains(5));
    EXPECT_TRUE(list.contains(20));
    EXPECT_FALSE(list.contains(999));
}

TEST_F(SkipListTest, SizeAndEmpty) {
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    list.insert(1);
    list.insert(2);
    list.insert(3);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.size(), 3);
}

TEST_F(SkipListTest, EraseWorks) {
    list.insert(10);
    list.insert(20);
    list.insert(30);
    EXPECT_TRUE(list.erase(20));
    EXPECT_FALSE(list.contains(20));
    EXPECT_FALSE(list.erase(20));  // повторное удаление
    EXPECT_EQ(list.size(), 2);
    EXPECT_TRUE(list.erase(10));
    EXPECT_TRUE(list.erase(30));
    EXPECT_TRUE(list.empty());
}

TEST_F(SkipListTest, InsertInSortedOrder) {
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(list.insert(i));
    }

    int expected = 0;
    for (auto val : list) {
        EXPECT_EQ(val, expected++);
    }
    EXPECT_EQ(expected, 100);
}

TEST_F(SkipListTest, ClearEmptiesList) {
    list.insert(1);
    list.insert(2);
    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_FALSE(list.contains(1));
    EXPECT_FALSE(list.contains(2));
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SkipListTest, ForwardIteratorTraversal) {
    std::vector<int> values{1, 3, 5, 7, 9};
    for (int v : values) list.insert(v);

    std::vector<int> output;
    for (auto it = list.begin(); it != list.end(); ++it) {
        output.push_back(*it);
    }

    EXPECT_EQ(output, values);
}

TEST_F(SkipListTest, RangeBasedForTraversal) {
    std::vector<int> input{4, 2, 8, 6};
    for (int x : input) list.insert(x);

    std::vector<int> expected{2, 4, 6, 8};
    std::vector<int> result;

    for (int x : list)
        result.push_back(x);

    EXPECT_EQ(result, expected);
}

TEST_F(SkipListTest, InsertDescendingOrder) {
    for (int i = 10; i >= 1; --i) list.insert(i);
    int expected = 1;
    for (int x : list)
        EXPECT_EQ(x, expected++);
}

TEST_F(SkipListTest, EraseAll) {
    for (int i = 0; i < 10; ++i) list.insert(i);
    for (int i = 0; i < 10; ++i) EXPECT_TRUE(list.erase(i));
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(SkipListTest, EraseNonExistent) {
    list.insert(1);
    list.insert(2);
    EXPECT_FALSE(list.erase(99));
    EXPECT_EQ(list.size(), 2);
}


int main(int argc, char* argv[]) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
