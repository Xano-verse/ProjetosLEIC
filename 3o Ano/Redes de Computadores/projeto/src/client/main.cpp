
#include "../common/constants.h"
#include "../common/operations.h"

#include <stdio.h>
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
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>

Option parseOption(const std::string &s)
{
    if (s == "login")
        return LOGIN;
    if (s == "changePass")
        return CHANGE_PASS;
    if (s == "unregister")
        return UNREGISTERED;
    if (s == "logout")
        return LOGOUT;
    if (s == "exit")
        return EXIT;
    if (s == "create")
        return CREATE;
    if (s == "close")
        return CLOSE;
    if (s == "myevents" || s == "mye")
        return MY_EVENTS;
    if (s == "list")
        return LIST;
    if (s == "show")
        return SHOW;
    if (s == "reserve")
        return RESERVE;
    if (s == "myreservations" || s == "myr")
        return MY_RESERVATION;
    return INVALID;
}


// Needs to be passed by reference because streams have no copy constructor
void printReply(std::string cmd, std::stringstream& server_response) {
    std::string discard, reply_cmd, reply_status, fname, fsize;
    server_response >> reply_cmd >> reply_status;

    std::stringstream reply;
    std::string reply_code;
    reply_code = "(" + reply_cmd + " " + reply_status + ")";

    switch(parseOption(cmd)) {
        case LOGIN:
            if (reply_status == "REG") {
                reply << "New user registered | Login successful " << reply_code;
            } else if (reply_status == "OK") {
                reply << "Login successful " << reply_code;
            } else if (reply_status == "NOK") {
                reply << "Invalid Login " << reply_code;
            }
            break;

        case CHANGE_PASS:
            if (reply_status == "OK") {
                reply << "Password changed successfully " << reply_code;
            } else if (reply_status == "NLG") {
                reply << "User is not logged in " << reply_code;
            } else if (reply_status == "NOK") {
                reply << "Password is incorrect " << reply_code;
            } else if (reply_status == "NID") {
                reply << "User does not exist " << reply_code;
            } 
            break;

        case UNREGISTERED:
            if (reply_status == "OK") {
                reply << " User unregistered successfully " << reply_code;
            } else if (reply_status == "NOK") {
                reply << "User is not logged in " << reply_code;
            } else if (reply_status == "UNR") {
                reply << "User is not registered " << reply_code;
            } else if (reply_status == "WRP") {
                reply << "Password is incorrect " << reply_code;
            }
            break;

        case LOGOUT:
            if (reply_status == "OK") {
                reply << "Log out successful " << reply_code;
            } else if (reply_status == "NOK") {
                reply << "User is not logged in " << reply_code;
            } else if (reply_status == "UNR") {
                reply << "User is not registered " << reply_code;
            } else if (reply_status == "WRP") {
                reply << "Password is incorrect " << reply_code;
            }
            break;

        case CREATE:
            if (reply_status == "OK") {
                std::string EID;
                server_response >> EID;
                reply << "Event " << EID << " created successfully " << reply_code;
            } else if (reply_status == "NOK") {
                reply << "Event could not be created " << reply_code;
            } else if (reply_status == "WRP") {
                reply << "Password is incorrect " << reply_code;
            } else if (reply_status == "NLG") {
                reply << "User is not logged in " << reply_code;
            }
            break;

            
        case CLOSE:
            if (reply_status == "OK") {
                reply << "Event closed successfully " << reply_code;
            } else if (reply_status == "NOK") {
                reply << "Login is invalid: user doesn't exist or password is wrong " << reply_code;
            } else if (reply_status == "NLG") {
                reply << "User is not logged in " << reply_code;
            } else if (reply_status == "NOE") {
                reply << "Event ID does not exist " << reply_code;
            } else if (reply_status == "EOW") {
                reply << "Event was not created by logged in user " << reply_code;
            } else if (reply_status == "SLD") {
                reply << "Event is already sold out " << reply_code;
            } else if (reply_status == "PST") {
                reply << "The event has already passed " << reply_code;
            } else if (reply_status == "CLO") {
                reply << "Event has already been closed by current user " << reply_code;
            }
            break;

        case MY_EVENTS:
            if (reply_status == "OK") {
                reply << "Displaying list of events created by the current user " << reply_code << ":";
                std::string EID, state, state_text;
                while (server_response >> EID >> state) {
                    if (state == "0") {
                        state_text = "Event has passed";
                    } else if (state == "1") {
                        state_text = "Accepting reservations";
                    } else if (state == "2") {
                        state_text = "Sold out";
                    } else if (state == "3") {
                        state_text = "Closed by current user";
                    }

                    reply << "\nEvent ID: " << EID << " | State: " << state_text << " (" << state << ")";
                }
            } else if (reply_status == "NOK") {
                reply << "User has created any events " << reply_code;
            } else if (reply_status == "NLG") {
                reply << "User is not logged in " << reply_code;
            } else if (reply_status == "WRP") {
                reply << "Password is incorrect " << reply_code;
            }
            break;
        
        case LIST:
            if (reply_status == "OK") {
                reply << "Displaying all events " << reply_code << ":\n";
                reply << "EID\t|\tNAME\t|\tSTATE\t\t\t|\tDATE"; 
                std::string EID, name, state, state_text, date, time;
                while (server_response >> EID >> name >> state >> date >> time) {
                    if (state == "0") {
                        state_text = "Event has passed";
                    } else if (state == "1") {
                        state_text = "Accepting reservations";
                    } else if (state == "2") {
                        state_text = "Sold out\t";              // tab necessário neste para ajustar alinhamentos
                    } else if (state == "3") {
                        state_text = "Closed by current user";
                    }

                    reply << "\n" << EID << "\t|\t" << name << "\t|\t" << state_text << "\t|\t" << date << " " << time;
                }
            } else if (reply_status == "NOK") {
                reply << "No events have been created " << reply_code;
            }
            break;

        /*case SHOW:*/
        // done at the end of main

        case RESERVE:
            if (reply_status == "ACC") {
                reply << "Reservation completed " << reply_code;
            } else if (reply_status == "REJ") {
                std::string n_seats;
                server_response >> n_seats;
                reply << "Event doesn't have enough free seats " << reply_code;
                reply << "\nOnly " << n_seats << " are currently available";
            } else if (reply_status == "NOK") {
                reply << "Requested event is not active " << reply_code;
            } else if (reply_status == "NLG") {
                reply << "User not logged in " << reply_code;
            } else if (reply_status == "CLS") {
                reply << "Requested event is closed " << reply_code;
            } else if (reply_status == "SLD") {
                reply << "Event is sold out " << reply_code;
            } else if (reply_status == "PST") {
                reply << "Event has alreadyed passed " << reply_code;
            } else if (reply_status == "WRP") {
                reply << "Password is incorrect " << reply_code;
            }
            break;

        case MY_RESERVATION:
            if (reply_status == "OK") {
                reply << "Displaying 50 most recent reservations by the current user " << reply_code << ":\n";
                reply << "EID\t|\tDATE\t\t\t|\tSeats reserved by current user"; 
                std::string EID, date, time, value;
                while (server_response >> EID >> date >> time >> value) {

                    reply << "\n" << EID << "\t|\t" << date << " " << time << "\t|\t" << value;
                }
            } else if (reply_status == "NOK") {
                reply << "Current user hasn't made any reservations " << reply_code;
            } else if (reply_status == "NLG") {
                reply << "User not logged in " << reply_code;
            } else if (reply_status == "WRP") {
                reply << "Password is incorrect " << reply_code;
            }
            break;

        default:
            if (reply_status  == "ERR") {
                reply << "Error in inputs " << reply_code;
            }
            break;

    }

    reply << "\n";
    std::cout << reply.str(); 
}


int main(int argc, char *argv[])
{

    char default_port[] = "58059"; // 58059 = 58000 + 59 (group number)

    char *server_ip = NULL;
    char *server_port = default_port;

    bool exit_program = 0;

    std::stringstream msg, ss;
    std::string message;
    std::vector<char> message_bytes;
    std::vector<char> response_buffer;

    // Only needed for SHOW
    std::vector<std::string> response_contents;
    std::string new_file_path;
    std::vector<char> new_file_text;

    // Save credencials
    std::string UID, password;
    std::string line, cmd, arg1, arg2, arg3, arg4, arg5, extra_arg;

    // Command argument parsing
    // ./user -n ESIP -p ESport
    // argv[0] argv[1] argv[2] argv[3] argv[4]
    if (argc > 1 && strcmp(argv[1], "-n") == 0)
    {
        server_ip = argv[2];

        if (argc > 3 && strcmp(argv[3], "-p") == 0)
        {
            server_port = argv[4];
        }
    }
    else if (argc > 1 && strcmp(argv[1], "-p"))
    {
        server_port = argv[4];
    }

    // Start loop
    while (1)
    {
        // Clear the response buffer
        response_buffer = {};
        message_bytes.clear(); // empties the vector completely

        msg.str(""); // clear content
        msg.clear(); // reset flags

        ss.str("");
        ss.clear();

        std::cout << "-------------\nEnter command: ";
        std::getline(std::cin, line);

        ss.str(line);
        ss >> cmd; // até 5 palavras

        switch (parseOption(cmd))
        {
        case LOGIN: // UDP - UID password -> LIN UID password
            ss >> arg1 >> arg2 >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << extra_arg << "h\n";
                std::cout << "Wrong input format\n";
                break;
            }
            
            // If there's a user still logged in
            if (UID != "" && password != "")
            {
                std::cout << "A user is already logged in. Please logout first" << "\n";
                break;
            }

            if (!isUid(arg1))
            {
                std::cout << "Invalid argument. UID must be 6 digits" << "\n";
                break;
            }
            if (!isPass(arg2))
            {
                std::cout << "Invalid argument. Password must be 8 characters" << "\n";
                break;
            }

            msg << "LIN" << " " << arg1 << " " << arg2 << "\n";
            message = msg.str();

            std::cout << "Sending UDP: " << message;

            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_UDP(message_bytes, server_ip, server_port); //, &response_buffer);

            // Only register internal credentials if the login was successful
            if (strcmp(response_buffer.data(), "RLI REG\n") == 0 || strcmp(response_buffer.data(), "RLI OK\n") == 0)
            {
                UID = arg1;
                password = arg2;
            }
            break;

        case CHANGE_PASS: // TCP - oldPassword newPassword -> CPS UID oldPassword newPassword
            ss >> arg1 >> arg2 >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }
            if (!isPass(arg1) || !isPass(arg2))
            {
                std::cout << "Invalid argument. Passwords must be 8 characters" << "\n";
                break;
            }

            msg << "CPS" << " " << UID << " " << arg1 << " " << arg2 << "\n";
            message = msg.str();
            std::cout << "Sending UDP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_TCP(message_bytes, server_ip, server_port); //, &response_buffer);

            // Only change internal password if the change was successful
            if (strcmp(response_buffer.data(), "RCP OK\n") == 0)
            {
                password = arg2;
            }
            break;

        case UNREGISTERED: // UDP - [] -> UNR UID password
            ss >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            msg << "UNR" << " " << UID << " " << password << "\n";
            message = msg.str();
            std::cout << "Sending UDP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_UDP(message_bytes, server_ip, server_port); //, &response_buffer);

            // Only reset internal credentials if unregister was successful
            if (strcmp(response_buffer.data(), "RUR OK\n") == 0)
            {
                UID = "";
                password = "";
            }
            break;

        case LOGOUT: // UDP - [] -> LOU UID password
            ss >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            msg << "LOU" << " " << UID << " " << password << "\n";
            message = msg.str();
            std::cout << "Sending UDP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_UDP(message_bytes, server_ip, server_port); //, &response_buffer);

            // Only reset internal credentials if logout was successful
            if (strcmp(response_buffer.data(), "RLO OK\n") == 0)
            {
                UID = "";
                password = "";
            }
            break;

        case EXIT:
            ss >> extra_arg;
            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }
        
            std::cout << "EXIT chosen\n"; //
            // If user still logged in
            if (UID != "" && password != "")
            {
                std::cout << "Please logout first" << "\n";
            }
            else
            {
                exit_program = 1;
            }
            break;

        case CREATE:
        { // TCP - name event_fname event_date event_time num_attendees -> CRE UID password name event_date attendance_size Fname Fsize Fdata

            ss >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            std::string path;
            struct stat info;
            std::ifstream fp;
            std::vector<char> fdata;
            size_t fsize;

            path = "./" + arg2;

            if (!fileExists(path))
            {
                std::cout << "No such file or directory \'" << path << "\'\n";
                break;
            }

            if (!isValidDateTime(arg3, arg4))
            {
                std::cout << "Invalid date and time\n";
                break;
            }

            if (!isNumberXto999(arg5, 10))
            {
                std::cout << "Invalid argument, attendance size must be between 10 and 999\n";
                break;
            }

            fp.open(path);
            if (!fp.is_open())
            {
                return false;
            }

            fdata.assign((std::istreambuf_iterator<char>(fp)), std::istreambuf_iterator<char>());

            fp.close();

            fsize = fdata.size();

            // sends with an extra \n at the end that isn't part of Fdata
            msg << "CRE" << " " << UID << " " << password << " " << arg1 << " " << arg3 << " " << arg4 << " " << arg5 << " " << arg2 << " " << fsize << " ";
            message = msg.str();
            std::cout << "Sending TCP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            message_bytes.insert(message_bytes.end(), fdata.begin(), fdata.end());
            message_bytes.push_back('\n');
            response_buffer = send_TCP(message_bytes, server_ip, server_port);

            break;
        }

        case CLOSE: //  TCP - EID -> CLS UID password EID

            ss >> arg1 >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            if (!isEid(arg1))
            {
                std::cout << "Invalid argument, EID must be 3 digits\n";
                break;
            }

            msg << "CLS" << " " << UID << " " << password << " " << arg1 << "\n";
            message = msg.str();
            std::cout << "Sending TCP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_TCP(message_bytes, server_ip, server_port); //, &response_buffer);
            break;

        case MY_EVENTS: // UDP - [] -> LME UID password
            ss >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            msg << "LME" << " " << UID << " " << password << "\n";
            message = msg.str();
            std::cout << "Sending UDP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_UDP(message_bytes, server_ip, server_port); //, &response_buffer);
            break;

        case LIST: // TCP - [] -> LST
            ss >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            // Em TCP é importante acabar a mensagem com \n
            msg << "LST\n";
            message = msg.str();
            std::cout << "Sending TCP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_TCP(message_bytes, server_ip, server_port); //, &response_buffer);
            break;

        case SHOW: // TCP - EID -> SED EID

            ss >> arg1 >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (!isEid(arg1))
            {
                std::cout << "Invalid argument, EID must be 3 digits\n";
                break;
            }

            msg << "SED" << " " << arg1 << "\n";
            message = msg.str();
            std::cout << "Sending TCP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_TCP(message_bytes, server_ip, server_port); //, &response_buffer);
            break;

        case RESERVE: // TCP - EID value -> RID UID password EID people

            ss >> arg1 >> arg2 >> extra_arg;

            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            if (!isEid(arg1))
            {
                std::cout << "Invalid argument, EID must be 3 digits\n";
                break;
            }

            msg << "RID" << " " << UID << " " << password << " " << arg1 << " " << arg2 << "\n";
            message = msg.str();
            std::cout << "Sending TCP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_TCP(message_bytes, server_ip, server_port); //, &response_buffer);
            break;

        case MY_RESERVATION: // UDP - [] -> LMR UID password

            ss >> extra_arg;
            // Confirm command ends with \n
            if (extra_arg != "") {
                std::cout << "Wrong input format\n";
                break;
            }

            if (UID == "")
            {
                std::cout << "No user session active, please log in first" << "\n";
                break;
            }

            msg << "LMR" << " " << UID << " " << password << "\n";
            message = msg.str();
            std::cout << "Sending UDP: " << message;
            message_bytes.insert(message_bytes.end(), message.begin(), message.end());
            response_buffer = send_UDP(message_bytes, server_ip, server_port);//, &response_buffer);
            break;

        default:
            std::cout << "Unknown\n"; //
        }

        // Will only enter this in case EXIT, which breaks out of the while loop
        if (exit_program)
        {
            break;
        }

        if (response_buffer.size() == 0) {
            continue;
        }


        // Transform string into vector<string> (response_buffer into response_contents)
        std::string response_str(response_buffer.begin(), response_buffer.end());
        std::stringstream ss2(response_str);


        // If we're in a response show event (RSE) we print additional information about the stored file, in another line
        if (cmd == "show") {

            std::string reply_cmd, reply_status, UID, name, date, time, att_size, seats_reserved, fname, fsize;
            ss2 >> reply_cmd >> reply_status;
            if (reply_status == "OK") {
                // RSE OK uid name dd-mm-yyyy hh:mm total_attendees reserved_seats fname fsize fdata
                // fname is index 8, fsize index 9, fdata index 10 onwards
                ss2 >> UID >> name >> date >> time >> att_size >> seats_reserved >> fname >> fsize; // guardar só index 8 e 9
                std::vector<char> fdata;
                std::streampos pos = ss2.tellg(); // position after arg8

                if (pos != -1) {
                    size_t start = pos;
                    // skip one leading space
                    ++start;

                    // create fdata
                    fdata.insert(fdata.end(), response_buffer.begin() + start, response_buffer.end());

                    if (!fdata.empty() && fdata.back() == '\n')
                        fdata.pop_back();
                }
            

                // RSE OK uid name dd-mm-yyyy hh:mm total_attendees reserved_seats fname fsize fdata
                // fname is index 8, fsize index 9, fdata index 10 onwards
                new_file_path = "./" + fname; 

                createRawFile(new_file_path, fdata);

                std::cout << "Displaying information about requested event (" << reply_cmd << " " << reply_status << "):\n";
                std::cout << "Owner UID: " << UID << " | Name: " << name << " | Date: " << date << " " << time << "\n";
                std::cout << "Total number of seats: " << att_size << " | Number of reserved seats: " << seats_reserved << "\n";
                std::cout << "File name: " << fname << " | File size: " << fsize << "B\n";
                std::cout << "Downloaded " << getFileSize(new_file_path) << "B and stored in file " << new_file_path << "\n"; // <<  event_state_message << "\n";


            // se for erro entao dar skip
            } else if (reply_status == "ERR" || reply_status == "NOK") {
                std::cout << "Remote server error: event does not exist, no file available, wrong inputs (" << reply_cmd << " " << reply_status << ")\n";

            }

        } else {
            printReply(cmd, ss2);
        }

    }
}

