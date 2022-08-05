#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>

std::string orderType[]{"pizza", "soup", "steak", "salad", "sushi"};
std::vector<std::string> readyOrders, orders;
bool kitchenIsFree = true;
bool finish = false;
std::mutex finish_access;
std::mutex kitchen_access;

void waiter() {
    bool close = false;
    bool busy = false;
    do {
        int delay = std::rand() % 6 + 5;
        int type = std::rand() % 5;
        std::this_thread::sleep_for(std::chrono::seconds(delay));
        kitchen_access.lock();
        if (kitchenIsFree) {
            busy = false;
            std::cout << "New order: " << orderType[type] << "." << std::endl;
            orders.push_back(orderType[type]);
        } else {
            if (!busy) {
                std::cout << "The kitchen is busy." << std::endl;
                busy = true;
            }
        }
        kitchen_access.unlock();
        finish_access.lock();
        if (finish) {
            close = true;
        }
        finish_access.unlock();
    } while (!close);
}

void kitchen() {
    bool close = false;
    do {
        kitchen_access.lock();
        if (!orders.empty()) {
            kitchenIsFree = false;
            kitchen_access.unlock();
            int delay = std::rand() % 11 + 5;
            std::cout << "Order is being prepared." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(delay));
            kitchen_access.lock();
            readyOrders.push_back(orders.back());
            std::cout << orders.back() << " is ready." << std::endl;
            orders.pop_back();
            kitchenIsFree = true;
        }
        kitchen_access.unlock();
        finish_access.lock();
        if (finish) {
            close = true;
        }
        finish_access.unlock();
    } while (!close);

}

void courier() {
    int ordersCount = 0;
    while (ordersCount < 10) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        kitchen_access.lock();
        if (!readyOrders.empty()) {
            std::cout << "Courier took orders: ";
            for (int i = 0; i < readyOrders.size(); i++) {
                std::cout << readyOrders[i] << " ";
                ordersCount++;
            }
            std::cout << std::endl;
            readyOrders.clear();
        }
        kitchen_access.unlock();
    }
    if (ordersCount >= 10) {
        finish_access.lock();
        finish = true;
        std::cout << ordersCount << " orders delivered." << std::endl;
        finish_access.unlock();
    }
}

int main() {
    std::thread waiterNew(waiter);
    std::thread kitchenNew(kitchen);
    std::thread courierNew(courier);
    waiterNew.join();
    kitchenNew.join();
    courierNew.join();
    return 0;
}
