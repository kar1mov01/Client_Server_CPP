// Copyright 2023  Eraj eradzh@2017@mail.ru

#ifndef INCLUDE_SERVER_HPP
#define INCLUDE_SERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <thread>

#include <Network.hpp>

class Program2 {
public:
    void run() {
        Network server("127.0.0.1", 8080);  // Создание сервера

        if (!server.bindSocket()) {
            std::cerr << "Ошибка при привязке сокета" << std::endl;
            return;
        }

        // Начало прослушивания
        if (!server.startListening()) {
            std::cerr << "Ошибка при запуске прослушивания" << std::endl;
            return;
        }

        while (running_) {
            if (server.acceptConnection()) {  // Принятие подключения
                while (running_) {
                    int receivedSum = 0;
                    if (server.readData(receivedSum)) {  // Чтение данных
                        processDataFromProgram1(receivedSum);  // Обработка данных от Программы №1
                    } else {
                        std::cerr
                                << "Соединение закрыто или ошибка чтения данных. Повторное подключение..."
                                << std::endl;
                        break;  // Выход из внутреннего цикла для повторного принятия подключения
                    }
                            std::this_thread::sleep_for(
                                            std::chrono::milliseconds(100)); 
                }
            } else {
                std::cerr << "Ошибка при принятии подключения" << std::endl;
            }
        }
    }

    void processDataFromProgram1(int receivedSum) {
        if (receivedSum > 99 && receivedSum % 32 == 0) {
            std::cout << "Сумма соответствует условию: " << receivedSum << std::endl;
        } else {
            std::cout << "Сумма не соответствует условию, попробуйте ещё раз!" << std::endl;
        }
    }

private:
    bool running_ = true;
};

#endif //INCLUDE_SERVER_HPP
