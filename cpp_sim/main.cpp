#include "Logger.h"


int main() {

  Logger logger;
  logger.openFile("../logs/test.csv", true);
  logger.logToCsvFormat<double, std::string, int>(4.54, "something", 5);

  return 0;
}