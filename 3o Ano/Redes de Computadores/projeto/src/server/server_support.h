#ifndef SERVER_SUPPORT_H
#define SERVER_SUPPORT_H

// Headers meus
#include "../common/constants.h"
#include "../common/operations.h"

// Headers C++ padrão
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>  // para funções C como memset, memcpy
#include <filesystem>

// Headers POSIX / Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>   // close(), read(), write()
#include <netdb.h>
#include <dirent.h>
#include <sys/stat.h>

// time
#include <chrono>
#include <ctime>

Option parseOption(const std::string& s); 


// --------------------------------------------------------------------------------------------------------------
// Funções principais externas - correspondem aos comandos
int createLogin(const std::string &uid, const std::string &password);
int changePassword(const std::string &uid, const std::string &old_password, const std::string &new_password);
int unregister(const std::string &uid, const std::string &password);
int logout(const std::string &uid, const std::string &password);
int createEvent(const std::string &uid, const std::string &password, const std::string &name, const std::string &event_date, const std::string &event_time, const std::string &attendance_size, const std::string &Fname, const std::string &Fsize, const std::vector<char> &Fdata, std::string *eid);
int closeEvent(const std::string &uid, const std::string &password, const std::string &eid);
int getUserEvents(const std::string &uid, const std::string &password, std::vector<std::string> *event_list);
int getAllEvents(std::vector<std::string> *event_list);
int getEventDetails(std::string eid, std::vector<std::string> *file_details);
int reserveEvent(const std::string &uid, const std::string &password, const std::string &eid, const std::string &people, std::string *seats_left); //reserve command
int getUserReservations(const std::string &uid, const std::string &password, std::vector<std::string> *reservation_list);
bool eventPast(const std::string &eid);



// --------------------------------------------------------------------------------------------------------------
// Auxiliares externas de eventos e reservas 

std::string getEventUser(const std::string &eid);
std::string getEventName(const std::string &eid);
std::string getEventFname(const std::string &eid);
std::string getEventAttendees(const std::string &eid);
std::string getEventDate(const std::string &eid);
std::string getEventReservedSeats(const std::string &eid);
int getEventState(const std::string &eid);

std::string getReservationEID(const std::string reservation);
std::string getReservationSeats(const std::string reservation);
std::string getReservationDate(const std::string reservation);




#endif	// SERVER_SUPPORT_H
