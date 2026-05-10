#include "../common/constants.h"

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>                                                                   
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <iomanip>
#include <chrono>
#include <sys/stat.h>
#include <fstream>
#include <vector>



/// Ensure that all data is written to the file descriptor.
/// @param fd File descriptor to write to.
/// @param buffer Buffer with the data to write.
/// @return 0 if all data was written, 1 otherwise.
int safe_write (int fd, const std::vector<char> &msg) {
    size_t len = msg.size();
    size_t done = 0;

	while (len > done) {
        ssize_t chars_written = write(fd, reinterpret_cast<const char*>(msg.data() + done), len - done);

        if (chars_written < 0) {
            fprintf(stderr, "Write error\n");
            return -1;
        }

        done += (size_t) chars_written;
    }

    return EXIT_SUCCESS;
}

std::string get_last_word(const char* buffer, size_t len)
{
    if (len == 0) return {};

    // 1. Skip trailing spaces
    ssize_t i = static_cast<ssize_t>(len) - 1;
    while (i >= 0 && buffer[i] == ' ')
        i--;

    if (i < 0)
        return {};  // buffer only had spaces

    // 2. Mark end of last word
    ssize_t end = i;

    // 3. Scan backwards to find start of last word
    while (i >= 0 && buffer[i] != ' ')
        i--;

    ssize_t start = i + 1;

    // 4. Construct string from raw memory
    return std::string(buffer + start, buffer + end + 1);
}

int readFdata(int fd, char *buffer, size_t fsize, size_t i)
{

    size_t bytes_read = 0;
        while (bytes_read < fsize) {
            ssize_t result = read(fd, buffer + bytes_read, fsize - bytes_read);
            if (result == -1) {
                perror("Failed to read file from socket");
                return -1;
            }
            bytes_read += (size_t)result;
    }

    return (int)i;
}

int read_string(int fd, std::vector<char> *buffer, size_t max) {
    ssize_t bytes_read;
    char ch;
    size_t i = 0;
    int is_create = 1, is_show = 1, space_counter = 0;
    char ch1, ch2, ch3;

    while (i < max) { // leave space for '\0'
        bytes_read = read(fd, &ch, 1);

        if (bytes_read <= 0) {
            return -1; // error or connection closed
        }

        buffer->push_back(ch);

        switch(i) {
            case 0:
                ch1 = ch;
            case 1:
                ch2 = ch;
            case 2:
                ch3 = ch;
        }

        if (ch == '\n') {
            break; // end of message
        }

        if (ch == ' ') {
            space_counter++;

            // Estes ifs são para print da file data toda mesmo que haja \n lá

            // is_create será relevante no servidor ao ler o pedido CRE
            if (is_create == 2 && space_counter == 9){
                std::string fsize_string = get_last_word(buffer->data(), i);
                size_t fsize = std::stoul(fsize_string);
                i++;

                char fdata_buffer[fsize];
                readFdata(fd, fdata_buffer, fsize, 0);
                
                bytes_read = read(fd, &ch, 1);
                if (ch != ('\n')){return -1;}
                buffer->insert(buffer->end(), fdata_buffer, fdata_buffer + fsize);
                buffer->push_back(ch);
                return 0;
            }

            // is_show será relevante no cliente ao ler a resposta RSE
            if (is_show == 2 && space_counter == 10) {
                std::string fsize_string = get_last_word(buffer->data(), i);
                size_t fsize = std::stoul(fsize_string);
                i++;

                char fdata_buffer[fsize];
                readFdata(fd, fdata_buffer, fsize, 0);
                buffer->insert(buffer->end(), fdata_buffer, fdata_buffer + fsize);
                return 0;
            }
        }


       // Check if its a Create command or Show
        if (i == 2) {
            if ((ch1 == 'C' && ch2 == 'R' && ch3 == 'E')){
            is_create = 2;

            } else if ((ch1 == 'R' && ch2 == 'S' && ch3 == 'E')){
                is_show = 2;
            }
        }

        i++;
    }

    return 0; // success
}

bool isAlphanumerical(const std::string &string) {
    return std::all_of(string.begin(), string.end(), [](unsigned char c){ return std::isalnum(c); });
}

bool isDigit(const std::string &string) {
    return std::all_of(string.begin(), string.end(), [](unsigned char c){ return std::isdigit(c); });
}

bool numBetween(const std::string &s, int min, int max){
    try {
        int value = std::stoi(s);
        return value >= min && value <= max;
    } catch (const std::out_of_range&) {
        return false; // number too big for int
    } catch (const std::invalid_argument&) {
        return false; // not a number (shouldn't happen due to isdigit check)
    }
}

bool isNumberXto999(const std::string &s, int min) {
    // check if string is not empty and contains only digits
    if (s.empty() || !std::all_of(s.begin(), s.end(),[](unsigned char c){ return std::isdigit(c); }))
        return false;

    // convert string to integer
    return numBetween(s, min, 999);
}

bool isPastDate(const std::string& date) {
    using namespace std::chrono;

    std::tm tm{};
    std::istringstream ss(date);
    ss >> std::get_time(&tm, "%d-%m-%Y %H:%M");

    if (ss.fail()) {
        throw std::runtime_error("Invalid date/time format");
    }

    // Convert std::tm → time_t → system_clock::time_point
    std::time_t t = std::mktime(&tm);
    auto inputTime = system_clock::from_time_t(t);

    // Get current time
    auto now = system_clock::now();

    return inputTime < now;
}

bool isValidDateTime(const std::string& date, const std::string& time) {
    std::tm t = {};
    std::string datetime = date + " " + time;
    std::istringstream ss(datetime);

    // Expect format "dd-mm-yyyy hh:mm"
    ss >> std::get_time(&t, "%d-%m-%Y %H:%M");
    if (ss.fail()) {return false;}

    // Normalize with mktime
    t.tm_isdst = -1;
    std::tm t_copy = t;
    std::time_t normalized = std::mktime(&t_copy);
    if (normalized == -1) {return false;}

    // Compare fields to catch invalid dates like 31-02-2024
    return t.tm_mday == t_copy.tm_mday &&
           t.tm_mon  == t_copy.tm_mon &&
           t.tm_year == t_copy.tm_year &&
           t.tm_hour == t_copy.tm_hour &&
           t.tm_min  == t_copy.tm_min;
}

std::vector<char> send_UDP(const std::vector<char> &msg, const char* ip, const char* port){
    int fd_UDP, errcode;
    ssize_t n;
    struct addrinfo hints, *res_UDP;
    struct sockaddr_storage addr;
    socklen_t addrlen;
    char buffer[BUFSIZ];
    std::vector<char> ret_buffer;

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0);    // UDP socket
    if (fd_UDP == -1) {
		perror("send_UDP socket");
		return {};
	}

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;              // IPv4
    hints.ai_socktype = SOCK_DGRAM;         // UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(ip, port, &hints, &res_UDP);
    if (errcode != 0) {
		perror("send_UDP getaddrinfo");
		return {};
	}

    n = sendto(fd_UDP, msg.data(), msg.size(), 0, res_UDP->ai_addr, res_UDP->ai_addrlen);
    if (n == -1) {
		perror("send_UDP sendto");
		return {};
	}

    addrlen = sizeof(addr);
    n = recvfrom(fd_UDP, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) {
		perror("send_UDP recvfrom");
		return {};
	}

    buffer[n] = '\0';
    n++;
    ret_buffer.insert(ret_buffer.end(), buffer, buffer + n);

    freeaddrinfo(res_UDP);
    close(fd_UDP);

    return ret_buffer;
}

std::vector<char> send_TCP(const std::vector<char> &msg, const char* ip, const char* port){
    int fd_TCP, errcode;
    ssize_t n;
    struct addrinfo hints, *res_TCP;
    std::vector<char> ret_buffer;

    fd_TCP = socket(AF_INET, SOCK_STREAM, 0);        // TCP socket
    if (fd_TCP == -1) {
		perror("send_TCP socket");
		return {};
	}

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;                   // IPv4
    hints.ai_socktype = SOCK_STREAM;             // TCP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(ip, port, &hints, &res_TCP);
    if (errcode != 0) {
		perror("send_TCP getaddrinfo");
		return {};
	}

    n = connect(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    if (n == -1) { 
		perror("send_TCP connect");
        return {};
	}

    n = safe_write(fd_TCP, msg);
    if (n == -1) {
		perror("send_TCP safe_write");
		return {};
	}

    n = read_string(fd_TCP, &ret_buffer, BUFSIZ);
    if (n == -1) {
		perror("send_TCP read_string");
		return {};
	}

    freeaddrinfo(res_TCP);
    close(fd_TCP);

    return ret_buffer;
}

bool validateFname(const std::string &fname){

    bool begining, dot, end;

    if ((4 > fname.size() || fname.size() > 24)){return false;}

    begining = std::all_of(fname.begin(), fname.end() - 4, [](char c) {
        return std::isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.';
    });

    dot = (*(fname.end() - 4) == '.');

    end = std::all_of(fname.end() - 3, fname.end(), [](char c) {
        return std::isalnum((unsigned char)c);
    });

    return begining && dot && end;
}



void createTextFile(const std::string &path, const std::string &text) {
    std::ofstream fp;

    // Abre o ficheiro para escrita
    fp.open(path);
    if (!fp.is_open()) {
        std::cerr << "failed create file" << path << "\n";
        perror("createFile");
        return;
    }

    fp << text;
    fp.close();
}

void createRawFile(const std::string &path, const std::vector<char> &text) {
    std::ofstream fp;

    // Abre o ficheiro para escrita
    fp.open(path);
    if (!fp.is_open()) {
        std::cerr << "failed create file" << path << "\n";
        perror("createFile");
        return;
    }

    fp.write(text.data(), static_cast<std::streamsize>(text.size()));
    fp.close();
}


std::string getFileSize(std::string path) {
    struct stat filestat;
    int ret_stat;
    std::string size;

    ret_stat = stat(path.c_str(), &filestat);
    
    if (ret_stat == -1 || filestat.st_size == 0) {
        perror("getFileSize ret_stat");
        return "";
    }

    // Convert from int to string by assigning to string
    size = std::to_string(filestat.st_size);
    
    // Return the file's size in bytes
    return(size);
}


bool isUid(const std::string& uid) {

    if ((uid.size() != 6) || !isDigit(uid)) {
        return false;
    }
    return true;
}

// Cheks if its a valid password
bool isPass(const std::string& passWord){

    if ((passWord.size() != 8) || !isAlphanumerical(passWord)) {
        return false;
    }
    return true;
}

// Cheks if its a valid eid
bool isEid(const std::string& eid){

    if ((eid.size() != 3 || !isDigit(eid))) {
        return false;
    }
    return true;
}


// technically o file/dir podem existir mas ter outro erros com o stat, mas less likely

bool directoryExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode));
}

bool fileExists(const std::string& path) {
    struct stat info;
    return (stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode));
}