#include <gtest/gtest.h>
#include "Time.h"

TEST(TimeTest, ParseAndFormat) {
    Time t = Time::parse("08:45");
    EXPECT_EQ(t.hour, 8);
    EXPECT_EQ(t.minute, 45);
    EXPECT_EQ(t.toString(), "08:45");
}

TEST(TimeTest, Comparison) {
    Time a(9, 0), b(10, 15);
    EXPECT_LT(a, b);
}

TEST(TimeTest, DifferenceInMinutes) {
    Time a(8, 30), b(10, 0);
    EXPECT_EQ(a.diffMinutes(b), 90);
}
