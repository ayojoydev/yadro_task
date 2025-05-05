#include "Time.h"
#include <sstream>
#include <iomanip>

Time::Time(int h, int m) : hour(h), minute(m) {}

Time Time::parse(const std::string& str) {
    int h = std::stoi(str.substr(0, 2));
    int m = std::stoi(str.substr(3, 2));
    return Time(h, m);
}

std::string Time::toString() const {
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hour << ":"
        << std::setw(2) << std::setfill('0') << minute;
    return oss.str();
}

bool Time::operator<(const Time& other) const {
    return (hour < other.hour) || (hour == other.hour && minute < other.minute);
}

bool Time::operator==(const Time& other) const {
    return hour == other.hour && minute == other.minute;
}

int Time::toMinutes() const {
    return hour * 60 + minute;
}

int Time::diffMinutes(const Time& other) const {
    return other.toMinutes() - this->toMinutes();
}
