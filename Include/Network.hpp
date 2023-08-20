// Copyright 2023 Eraj eradzh@2017@mail.ru

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <string>

class Network {
public:
    // Конструктор по умолчанию. Создает сокет и инициализирует адрес по умолчанию.
    Network();

    // Конструктор с параметрами. Создает сокет и инициализирует адрес с заданными параметрами.
    Network(const std::string &ip, int port);

    // Деструктор. Закрывает сокет при уничтожении объекта.
    ~Network();

    // Метод для закрытия сокета.
    void closeSocket() const;

    // Метод для создания сокета.
    void createSocket();
    
    // Метод для подключения к серверу. Возвращает true при успешном подключении, иначе false.
    bool connectToServer();

    // Метод для привязки сокета к заданному порту. Возвращает true при успешной привязке, иначе false.
    bool bindSocket();

    // Метод для начала прослушивания сокета. Возвращает true при успешном начале прослушивания, иначе false.
    bool startListening();
    
    // Метод для принятия подключения. Возвращает true при успешном принятии, иначе false.
    bool acceptConnection();

    // Метод для чтения данных. Возвращает true при успешном чтении, иначе false.
    bool readData(int &data);

    // Метод для отправки данных. Возвращает true при успешной отправке, иначе false.
    bool sendData(int sum);

    // Получение идентификатора сокета.
    int getSocket() const { return sockfd_; }

private:
    int sockfd_ = -1;  // Идентификатор сокета
    int listenSockfd_ = -1;  // Идентификатор сокета для прослушивания
    sockaddr_in serv_addr_;  // Структура для хранения адреса сервера

    // Метод для инициализации адреса сервера.
    void initializeServerAddress(const std::string &ip, int port);
};

void Network::initializeServerAddress(const std::string &ip, int port) {
    serv_addr_.sin_family = AF_INET;  
    serv_addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &serv_addr_.sin_addr);
}

Network::Network() {
    createSocket();  // Создание сокета
    initializeServerAddress("127.0.0.1", 8080);  // Инициализация адреса
}

Network::Network(const std::string &ip, int port) {
    createSocket();  // Создание сокета
    initializeServerAddress(ip, port);  // Инициализация адреса
}

Network::~Network() { closeSocket(); }  // Деструктор, закрывает сокет при уничтожении объекта

void Network::closeSocket() const {
    if (sockfd_ != -1) {
        close(sockfd_);  // Закрытие сокета
    }
}

void Network::createSocket() {
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);  // Создание сокета
    if (sockfd_ < 0) {
        throw std::runtime_error("Ошибка при создании сокета");
    }
}

bool Network::connectToServer() {
    if (connect(sockfd_, reinterpret_cast<struct sockaddr *>(&serv_addr_), sizeof(serv_addr_)) < 0) {
        return false;  
    }
    return true; 
}

bool Network::bindSocket() {
    listenSockfd_ = socket(AF_INET, SOCK_STREAM, 0);  // Создание сокета для прослушивания
    if (listenSockfd_ < 0) {
        return false; 
    }
    return bind(listenSockfd_, reinterpret_cast<struct sockaddr *>(&serv_addr_), sizeof(serv_addr_)) == 0;
}

bool Network::startListening() {
    return listen(listenSockfd_, 5) == 0;
}

bool Network::acceptConnection() {
    socklen_t addrLen = sizeof(serv_addr_);
    sockfd_ = accept(listenSockfd_, reinterpret_cast<struct sockaddr *>(&serv_addr_),
                     &addrLen);  // Принятие подключения

    if (sockfd_ < 0) {
        return false;  
    }

    return true; 
}

bool Network::readData(int &data) {
    if (sockfd_ == -1) {
        return false;
    }

    int bytesReceived = recv(sockfd_, &data, sizeof(data), 0);  // Чтение данных
    if (bytesReceived == -1) {
        std::cerr << "Ошибка чтения данных из сокета" << std::endl;
        return false;
    } else if (bytesReceived == 0) {
        std::cerr << "Соединение закрыто клиентом" << std::endl;
        return false;
    }

    return true; 
}

bool Network::sendData(int sum) {
    if (sockfd_ == -1) {
        return false;
    }
    int bytesReceived = send(sockfd_, &sum, sizeof(sum), 0);  // Отправка данных
    if (bytesReceived == -1) {
        std::cerr << "Ошибка чтения данных из сокета" << std::endl;
        return false;
    } else if (bytesReceived == 0) {
        std::cerr << "Соединение закрыто клиентом" << std::endl;
        return false;
    }

    return true;  // Возвращаем true в случае успешной отправки
}

#endif  // NETWORK_HPP
