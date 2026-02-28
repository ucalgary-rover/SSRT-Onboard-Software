#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>

namespace utils {

/**
 * Find the config.env file by searching common locations:
 * 1. Current working directory
 * 2. Executable directory (where the binary is located)
 * 3. Parent of executable directory (MQTTServer/)
 *
 * @param argc The argc from main()
 * @param argv The argv from main()
 * @param filename The name of the env file to search for (default: "config.env")
 * @return Path to the config file, or just the filename if not found
 */
std::filesystem::path find_env_file(int argc, char **argv,
                                    const std::string &filename = "config.env");

}  // namespace utils

#endif  // UTILS_H
