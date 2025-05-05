#pragma once
#include <string>

struct Time {
    int hour;
    int minute;

    Time(int h = 0, int m = 0);

    static Time parse(const std::string& str);
    std::string toString() const;

    bool operator<(const Time& other) const;
    bool operator==(const Time& other) const;

    int toMinutes() const;
    int diffMinutes(const Time& other) const;
};
