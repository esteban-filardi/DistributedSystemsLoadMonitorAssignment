#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <random>

int getPercentageCpu() {
    // @todo: add a real implementation

    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);

    // Generate a random number
    int randomNumber = dist(gen);
    return randomNumber;
}
