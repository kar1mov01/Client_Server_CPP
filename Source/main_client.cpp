// Copyright 2023  Eraj eradzh@2017@mail.ru

#include <mutex>
#include <thread>

#include <Client.hpp>

class Program1 {
public:
    void run() {
        Network client("127.0.0.1", 8080);
        std::thread t1(&DataProcessor::processInput, &dataProcessor_);
        std::thread t2(&DataProcessor::processData, &dataProcessor_, std::ref(client));
        t1.join();
        t2.join();
    }

private:
    DataProcessor dataProcessor_;
};

int main() {
    Program1 program;
    program.run();
    return 0;
}
