#include <iostream>
#include <ctime>

using namespace std;

int main() {

    time_t currentTime = time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localTime);

    std::cout << "当前时间是：" << timeStr << std::endl;
}