#ifndef COMMON_OPERATIONS_H
#define COMMON_OPERATIONS_H

#include <string.h>
#include <string>
#include <vector>

int safe_write(int fd, const std::vector<char> &msg);
std::string get_last_word(const char* buffer, size_t len);
int readFdata(int fd, char *buffer, size_t max, size_t i);
int read_string(int fd, std::vector<char> *buffer, size_t max);

bool isAlphanumerical(const std::string &string);
bool isDigit(const std::string &string);
bool isNumberXto999(const std::string &s, int min);
bool isValidDateTime(const std::string &date, const std::string &time);
bool isPastDate(const std::string& date);

std::vector<char> send_UDP(const std::vector<char> &msg, const char* ip, const char* port);
std::vector<char> send_TCP(const std::vector<char> &msg, const char* ip, const char* port);

bool validateFname(const std::string &fname);

void createTextFile(const std::string &path, const std::string &text);
void createRawFile(const std::string &path, const std::vector<char> &text);
std::string getFileSize(std::string path);

bool isUid(const std::string& uid);
bool isPass(const std::string& passWord);
bool isEid(const std::string& eid);

bool directoryExists(const std::string& path);
bool fileExists(const std::string& path);

#endif	// COMMON_OPERATIONS_H
