#pragma once
#include <array>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

constexpr int HEIGHT = 20;
constexpr int WIDTH = 10;
constexpr int BLOCK_SIZE = 4;
constexpr int TILE_SIZE = 50;
constexpr int WAIT_TIME = 500;

