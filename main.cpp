#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Time {
    int hour, minute;

    Time(int h = 0, int m = 0) : hour(h), minute(m) {}

    static Time parse(const string& s) {
        int h = stoi(s.substr(0, 2));
        int m = stoi(s.substr(3, 2));
        return Time(h, m);
    }

    string toString() const {
        ostringstream oss;
        oss << setw(2) << setfill('0') << hour << ":"
            << setw(2) << setfill('0') << minute;
        return oss.str();
    }

    bool operator<(const Time& other) const {
        return hour < other.hour || (hour == other.hour && minute < other.minute);
    }

    bool operator==(const Time& other) const {
        return hour == other.hour && minute == other.minute;
    }

    int toMinutes() const {
        return hour * 60 + minute;
    }

    int diffMinutes(const Time& other) const {
        return other.toMinutes() - toMinutes();
    }
};

enum EventType {
    ARRIVAL = 1, SIT = 2, WAIT = 3, LEAVE = 4,
    FORCE_LEAVE = 11, AUTO_SIT = 12, ERROR = 13
};

struct Event {
    Time time;
    int type;
    string clientName;
    int table = -1;
    string errorText;

    static Event parse(const string& line) {
        istringstream iss(line);
        string timeStr;
        int type;
        string name;
        int table = -1;

        iss >> timeStr >> type >> name;
        if (type == 2 || type == 12) iss >> table;

        Event e;
        e.time = Time::parse(timeStr);
        e.type = type;
        e.clientName = name;
        e.table = table;
        return e;
    }

    string toString() const {
        ostringstream oss;
        oss << time.toString() << " " << type << " " << clientName;
        if (type == 2 || type == 12) oss << " " << table;
        if (type == ERROR) oss << " " << errorText;
        return oss.str();
    }
};

struct Client {
    string name;
    bool inClub = false;
    int table = -1;
    Time sitTime;
};

struct Table {
    int id;
    string occupiedBy;
    int profit = 0;
    int busyMinutes = 0;

    Table(int i) : id(i) {}
};

class Club {
    int tableCount;
    Time openTime, closeTime;
    int pricePerHour;

    map<string, Client> clients;
    vector<Table> tables;
    queue<string> waitQueue;
    vector<Event> outputEvents;

public:
    Club(int n, Time open, Time close, int price)
        : tableCount(n), openTime(open), closeTime(close), pricePerHour(price) {
        for (int i = 1; i <= n; ++i)
            tables.emplace_back(i);
    }

    void processEvent(const Event& e) {
        switch (e.type) {
            case ARRIVAL: handleArrival(e); break;
            case SIT: handleSit(e); break;
            case WAIT: handleWait(e); break;
            case LEAVE: handleLeave(e); break;
            default: break;
        }
    }

    void finishDay() {
        // Обработать оставшихся клиентов
    }

    void printResults() {
        cout << openTime.toString() << endl;
        for (const auto& e : outputEvents)
            cout << e.toString() << endl;
        cout << closeTime.toString() << endl;
        for (const auto& t : tables) {
            Time busy(0, t.busyMinutes);
            printf("%d %d %02d:%02d\n", t.id, t.profit, t.busyMinutes / 60, t.busyMinutes % 60);
        }
    }

private:
    void handleArrival(const Event& e) {
        // TODO
    }

    void handleSit(const Event& e) {
        // TODO
    }

    void handleWait(const Event& e) {
        // TODO
    }

    void handleLeave(const Event& e) {
        // TODO
    }

    void addError(const Event& cause, const string& msg) {
        outputEvents.push_back(cause);
        Event err;
        err.time = cause.time;
        err.type = ERROR;
        err.errorText = msg;
        outputEvents.push_back(err);
    }
};
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Failed to open file: " << argv[1] << endl;
        return 1;
    }

    string line;
    int tableCount;
    string startTimeStr, endTimeStr;
    int price;

    // Считываем конфигурацию
    try {
        getline(infile, line);
        tableCount = stoi(line);

        getline(infile, line);
        istringstream timeStream(line);
        timeStream >> startTimeStr >> endTimeStr;
        Time open = Time::parse(startTimeStr);
        Time close = Time::parse(endTimeStr);

        getline(infile, line);
        price = stoi(line);

        Club club(tableCount, open, close, price);

        // Обработка событий
        while (getline(infile, line)) {
            if (line.empty()) continue;
            try {
                Event e = Event::parse(line);
                club.processEvent(e);
            } catch (...) {
                cout << line << endl;
                return 0;
            }
        }

        club.finishDay();
        club.printResults();

    } catch (...) {
        cout << line << endl;
        return 0;
    }

    return 0;
}

