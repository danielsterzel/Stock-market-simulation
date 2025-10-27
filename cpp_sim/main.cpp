#include "Logger.h"
#include "Order.h"
int main() {

  Order o{OrderType::LIMITORDER, 1, true, 2.0, 5, std::chrono::steady_clock::now(), std::chrono::milliseconds(5)};

  Logger logger;
  logger.openFile("../logs/test.csv", true);
  logger.logToCsvFormat<double, std::string, int>(4.54, "something", 5);

  return 0;
}