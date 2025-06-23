#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <mutex>

using namespace std;

const int WIDTH = 100;
const int HEIGHT = 100;
const int N = 3;           // зміна напрямку N секунд
const int V = 1;           // швидкість
const int DURATION = 30;   // тривалість 

mutex mtx;

struct Position {
    int x, y;
};

class Rabbit {
public:
    Position pos;
    string name;

    Rabbit(string name, int x, int y) : name(name) {
        pos = {x, y};
    }

    virtual void move() = 0;
};

class NormalRabbit : public Rabbit {
public:
    NormalRabbit(string name, int x, int y) : Rabbit(name, x, y) {}

    void move() override {
        srand(time(0) + rand());

        int dx = 1, dy = 1;
        for (int t = 0; t < DURATION; ++t) {
            if (t % N == 0) {
                dx = rand() % 3 - 1; // -1, 0 або 1
                dy = rand() % 3 - 1;
            }

            pos.x += V * dx;
            pos.y += V * dy;

            // Обмеження межами 
            pos.x = max(0, min(WIDTH, pos.x));
            pos.y = max(0, min(HEIGHT, pos.y));

            mtx.lock();
            cout << "[standart] " << name << " -> (" << pos.x << ", " << pos.y << ")" << endl;
            mtx.unlock();

            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

class AlbinoRabbit : public Rabbit {
public:
    AlbinoRabbit(string name, int x, int y) : Rabbit(name, x, y) {}

    void move() override {
        for (int t = 0; t < DURATION && pos.x < WIDTH; ++t) {
            pos.x += V;
            mtx.lock();
            cout << "[albinos] " << name << " -> (" << pos.x << ", " << pos.y << ")" << endl;
            mtx.unlock();

            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

int main() {
    vector<thread> threads;

    // створення кроликів
    NormalRabbit r1("krolyk1", 10, 10);
    NormalRabbit r2("krolyk2", 20, 30);
    AlbinoRabbit a1("albinos1", 0, 50);
    AlbinoRabbit a2("albinos2", 10, 70);

    // запуск потоків
    threads.emplace_back(&NormalRabbit::move, &r1);
    threads.emplace_back(&NormalRabbit::move, &r2);
    threads.emplace_back(&AlbinoRabbit::move, &a1);
    threads.emplace_back(&AlbinoRabbit::move, &a2);

    for (auto& t : threads) {
        t.join();
    }

    cout << "simulation end." << endl;
    return 0;
}
