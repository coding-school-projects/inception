#pragma once

#include <string>
#include <vector>
#include <map>
#include <queue>
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <csignal>

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string RESET = "\033[0m";
const std::string YELLOW = "\033[33m";
const std::string GREEN_B = "\033[92m";
const std::string MAGENDA_B = "\033[95m";
const std::string CYAN_BU = "\033[1;4;96m";
const std::string YELLOW_B ="\033[93m";
const std::string BLUE_B = "\033[94m";