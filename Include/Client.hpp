// Copyright 2023  Eraj eradzh@2017@mail.ru

#ifndef INCLUDE_CLIENT_HPP
#define INCLUDE_CLIENT_HPP

#include <algorithm>
#include <iostream>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include <Network.hpp>

class DataProcessor {
public:
    DataProcessor() : sum_(0) {}

    void processInput();                 // Метод для пользовательского ввода
    void processData(Network &client);  // Метод обработки ввода

private:
    bool isConnected_ = false; // Флаг состояния подключения
    bool running_ = true;      // Флаг для цикла работы методов обработки ввода
    int sum_;                  // Переменная для хранения суммы
    std::string buffer_;       // Общий буфер данных
    std::mutex bufferMutex_;   // Мьютекс для синхронизации доступа к буферу данных

    // Метод для проверки состояния подключения к серверу
    bool checkConnection(Network &client);

    // Метод для отправки суммы данных в Программу №2
    bool sendDataToProgram2(Network &client);
};

// Метод для проверки состояния подключения к серверу
bool DataProcessor::checkConnection(Network &client) {
    int optval;
    socklen_t optlen = sizeof(optval);
    if (getsockopt(client.getSocket(), SOL_SOCKET, SO_ERROR, &optval,
                   &optlen) == 0) {
        if (optval == 0) {
            isConnected_ = true;
            return true;
        }
    }
    return false;
}

void DataProcessor::processInput() {
    while (running_) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));  // Ожидание перед следующей итерацией
        std::string input;
        std::cout << "Введите строку из цифр (не более 64 символов): ";
        std::cin >> input;
        if (std::all_of(input.begin(), input.end(), isdigit) &&
            input.length() <= 64) {
            std::string sortedInput = input;
            std::sort(sortedInput.rbegin(), sortedInput.rend());

            // регулярное выражение для обработки ввода пользователя
            std::regex r{R"~~([02468])~~"};
            sortedInput = std::regex_replace(sortedInput, r, "KB");

            std::lock_guard<std::mutex> lock(bufferMutex_);
            buffer_ = sortedInput;  // Запись обработанных данных в буфер
        } else {
            std::cout << "Ошибка ввода. Попробуйте еще раз." << std::endl;
        }
    }
}

void DataProcessor::processData(Network &client) {
    while (running_) {
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));  
        std::lock_guard<std::mutex> lock(bufferMutex_);
        if (!buffer_.empty()) {
            std::cout << "Данные буфера: " << buffer_ << std::endl;
            for (char c: buffer_) {
                if (std::isdigit(c)) {
                    sum_ += c - '0';
                }
            }
            std::cout << "Сумма: = " << sum_ << std::endl;
            buffer_ = "";  // Очистка буфера после обработки
            sendDataToProgram2(client);  // Отправка суммы в Программу №2
            sum_ = 0;
        }
    }
}

// Метод для отправки суммы данных в Программу №2
bool DataProcessor::sendDataToProgram2(Network &client) {
    if (!isConnected_) {
        if (!client.connectToServer()) {
            std::cerr << "Ошибка при подключении к серверу" << std::endl;
            return false;
        }
        isConnected_ = true;
    }

    if (client.sendData(sum_) && checkConnection(client)) {
        std::cout << "Сумма успешно отправлена" << std::endl;
    } else {
        std::cerr << "Ошибка при отправке суммы" << std::endl;
        client.closeSocket();
        std::this_thread::sleep_for(
                std::chrono::milliseconds(100));  
        client.createSocket();
        isConnected_ = false;
        return false;
    }
    return true;
}

#endif //INCLUDE_CLIENT_HPP
