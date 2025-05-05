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
        // Обработать оставшихся клиентов ну тут пока пустеньково...
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

	    outputEvents.push_back(e);

	    auto& name = e.clientName;
	    auto it = clients.find(name);

	    if (e.time < openTime) {
		addError(e, "NotOpenYet");
	        return;
    	    }	

		if (it != clients.end() && it->second.inClub) {
        		addError(e, "YouShallNotPass");
        	return;
    	}

	    // Добавляем клиента
    	clients[name] = Client{name, true, -1, Time()};
    }

    

    void handleSit(const Event& e) {
        outputEvents.push_back(e);
        auto& name = e.clientName;
        int tableIdx = e.table - 1;

        if (clients.find(name) == clients.end() || !clients[name].inClub) {
            addError(e, "ClientUnknown");
            return;
        }

        if (tableIdx < 0 || tableIdx >= tableCount) {
            addError(e, "PlaceIsBusy");
            return;
        }

        if (!tables[tableIdx].occupiedBy.empty()) {
            if (tables[tableIdx].occupiedBy == name) {
                addError(e, "PlaceIsBusy");
                return;
            }
            addError(e, "PlaceIsBusy");
            return;
        }

        // Освобождаем текущий стол, если сидит
        Client& client = clients[name];
        if (client.table != -1) {
            int oldIdx = client.table;
            int minutes = tables[oldIdx].id >= 1 ? tables[oldIdx].id : 0;
            int sessionTime = client.sitTime.diffMinutes(e.time);
            tables[oldIdx].busyMinutes += sessionTime;
            tables[oldIdx].profit += ((sessionTime + 59) / 60) * pricePerHour;
            tables[oldIdx].occupiedBy.clear();
        }

        // Занимаем новый стол
        client.table = tableIdx;
        client.sitTime = e.time;
        tables[tableIdx].occupiedBy = name;

        // Если клиент был в очереди — убираем
        queue<string> newQueue;
        while (!waitQueue.empty()) {
            string qname = waitQueue.front(); waitQueue.pop();
            if (qname != name) newQueue.push(qname);
        }
        waitQueue = newQueue;
    }


    void handleWait(const Event& e) {
          outputEvents.push_back(e);
        auto& name = e.clientName;

        if (clients.find(name) == clients.end() || !clients[name].inClub) {
            addError(e, "ClientUnknown");
            return;
        }

    // Если есть свободный стол
        bool hasFree = false;
        for (auto& t : tables) {
            if (t.occupiedBy.empty()) {
                hasFree = true;
                break;
            }
        }
        if (hasFree) {
            addError(e, "ICanWaitNoLonger!");
            return;
        }

    // Если очередь переполнена
        if ((int)waitQueue.size() >= tableCount) {
            Event forcedLeave;
            forcedLeave.time = e.time;
            forcedLeave.type = FORCE_LEAVE;
            forcedLeave.clientName = name;
            outputEvents.push_back(forcedLeave);

            clients[name].inClub = false;
            return;
        }

    // Ставим в очередь
        waitQueue.push(name);
    }

    void handleLeave(const Event& e) {
        outputEvents.push_back(e);
        auto& name = e.clientName;

        if (clients.find(name) == clients.end() || !clients[name].inClub) {
            addError(e, "ClientUnknown");
            return;
        }

        Client& client = clients[name];
        if (client.table != -1) {
            int tableIdx = client.table;
            int sessionTime = client.sitTime.diffMinutes(e.time);
            tables[tableIdx].busyMinutes += sessionTime;
            tables[tableIdx].profit += ((sessionTime + 59) / 60) * pricePerHour;
            tables[tableIdx].occupiedBy.clear();

            // Освободили стол — посадим из очереди
            if (!waitQueue.empty()) {
                string next = waitQueue.front();
                waitQueue.pop();

                Event autoSit;
                autoSit.time = e.time;
                autoSit.type = AUTO_SIT;
                autoSit.clientName = next;
                autoSit.table = tableIdx + 1;
                outputEvents.push_back(autoSit);

                clients[next].table = tableIdx;
                clients[next].sitTime = e.time;
                tables[tableIdx].occupiedBy = next;
            }
        }

        client.table = -1;
        client.inClub = false;
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

