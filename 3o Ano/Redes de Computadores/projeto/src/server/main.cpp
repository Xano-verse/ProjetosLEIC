#include "./server_support.h"

// Headers teus já incluídos em server_support.h, então não repitas

#include <cstdlib>   // para std::exit(), etc.
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>   // para memset, memcpy
#include <cmath>


std::vector<char> server_command(const char *buffer, bool verbose){
    std::stringstream ss, msg;
    std::vector<char> message, fdata;
    std::string cmd, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, msg_str;
    ss.str(buffer);
    ss >> cmd;

    switch (parseOption(cmd)) {
        case LOGIN:{ if (verbose == 1){std::cout << "LOGIN chosen   UID:" << arg1 <<"\n";}                                    // UDP - LIN UID password -> RLI status

            int answer;
            std::string state;
            ss >> arg1 >> arg2;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = createLogin(arg1, arg2);
                switch (answer){
                    // Error in inputs
                    case -2:
                        state = "ERR";
                        break;
                    // There was a problem
                    case -1:
                        state = "NOK";
                        break;
                    // Logged in
                    case 0:
                        state = "OK";
                        break;
                    // Registers new user
                    case 1:
                        state = "REG";
                        break;
                }
            }

            msg << "RLI" << " " << state << "\n";
            std::cout << "Sending UDP: RLI " << state << "\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));

            break;
        }

        case CHANGE_PASS:{ if (verbose == 1){std::cout << "CHANGE_PASS chosen   UID:" << arg1 <<"\n";}                  // TCP - CPS UID oldPassword newPassword -> RCP status

            int answer;
            std::string state;
            ss >> arg1 >> arg2 >> arg3;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = changePassword(arg1, arg2, arg3);
                switch (answer){
                    // Error in inputs
                    case -4:
                        state = "ERR";
                        break;
                    // Password is wrong
                    case -3:
                        state = "NOK";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NLG";
                        break;
                    // User doesn't exist
                    case -1:
                        state = "NID";
                        break;
                    case 0:
                    // Password is correct
                        state = "OK";
                        break;
                }
            }

            msg << "RCP" << " " << state << "\n";
            std::cout << "Sending TCP: RCP " << state << "\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case UNREGISTERED:{ if (verbose == 1){std::cout << "UNREGISTERED chosen   UID:" << arg1 <<"\n";}                     // UDP - UNR UID password -> RUR status

            int answer;
            std::string state;
            ss >> arg1 >> arg2;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = unregister(arg1, arg2);
                switch (answer){
                    // Error in inputs
                    case -4:
                        state = "ERR";
                        break;
                    // Password is wrong
                    case -3:
                        state = "WRP";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NOK";
                        break;
                    // User doesn't exist
                    case -1:
                        state = "UNR";
                        break;
                    case 0:
                    // Password is correct
                        state = "OK";
                        break;
                }
            }

            msg << "RUR" << " " << state << "\n";
            std::cout << "Sending UDP: RUR " << state << "\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case LOGOUT:{ if (verbose == 1){std::cout << "LOGOUT chosen   UID:" << arg1 <<"\n";}                                 // UDP - LOU UID password -> RLO status

            int answer;
            std::string state;
            ss >> arg1 >> arg2;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = logout(arg1, arg2);
                switch(answer) {
                    // Error in inputs
                    case -4:
                        state = "ERR";
                        break;
                    // Password is wrong
                    case -3:
                        state = "WRP";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NOK";
                        break;
                    // User doesn't exist
                    case -1:
                        state = "UNR";
                        break;
                    case 0:
                    // Password is correct
                        state = "OK";
                        break;
                }
            }

            msg << "RLO" << " " << state << "\n";
            std::cout << "Sending UDP: RLO " << state << "\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case CREATE:{ if (verbose == 1){std::cout << "CREATE chosen   UID:" << arg1 <<"\n";}                              // TCP - CRE UID password name event_date attendance_size Fname Fsize Fdata -> RCE status [EID]

            int answer;
            std::string state, eid;
            ss >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> arg6 >> arg7 >> arg8;

            // get the position in the buffer where ss stopped
            std::streampos stream_pos = ss.tellg(); // position after arg8
            size_t fdata_pos = static_cast<size_t>(stream_pos);

            std::string fsize_string = get_last_word(buffer, fdata_pos);
            size_t fsize = std::stoul(fsize_string);
            // skip 1 space
            fdata_pos++;
            fdata.insert(fdata.end(), buffer + fdata_pos, buffer + fdata_pos + fsize);

            // buffer is now a char* and not vector
            if (buffer[fdata_pos + fsize] != '\n'){
                state = "ERR";
            } else{

                answer = createEvent(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, fdata, &eid);
                switch(answer) {
                    // Error in inputs
                    case -4:
                        state = "ERR";
                        break;
                    // Password is wrong
                    case -3:
                        state = "WRP";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NLG";
                        break;
                    // Couldn't create file
                    case -1:
                        state = "NOK";
                        break;
                    // Created file
                    case 0:
                        state = "OK";
                        break;
                }
            }

            if(state == "OK") {
                msg << "RCE" << " " << state << " " << eid << "\n";
                std::cout << "Sending TCP: RCE " << state << " [EID: " << eid << "]\n";
            } else {
                msg << "RCE" << " " << state << "\n";
                std::cout << "Sending TCP: RCE " << state << "\n";
            }
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case CLOSE:{ if (verbose == 1){std::cout << "CLOSE chosen   UID:" << arg1 <<"\n";}                           //  TCP - CLS UID password EID -> RCL status

            int answer;
            std::string state;
            ss >> arg1 >> arg2 >> arg3;
            
            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = closeEvent(arg1, arg2, arg3);

                switch(answer) {
                    // Error in inputs
                    case -8:
                        state = "ERR";
                        break;
                    // Was already closed by user
                    case -7:
                        state = "CLO";
                        break;
                    // Event in the past
                    case -6:
                        state = "PST";
                        break;
                    // Created by user but already sold out
                    case -5:
                        state = "SLD";
                        break;
                    // Event not created by user
                    case -4:
                        state = "EOW";
                        break;
                    // Event EID doest't exist
                    case -3:
                        state = "NOE";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NLG";
                        break;
                    // User doesn't exist or password wrong
                    case -1:
                        state = "NOK";
                        break;
                    // Was closed
                    case 0:
                        state = "OK";
                        break;
                }
            }

            msg << "RCL" << " " << state << "\n";
            std::cout << "Sending TCP: RCL " << state << "\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case MY_EVENTS:{ if (verbose == 1){std::cout << "MY_EVENTS chosen   UID:" << arg1 <<"\n";}                        // UDP - LME UID password -> RME status[ EID state]*

            int answer;
            std::string state;

            std::vector<std::string> event_list;
            ss >> arg1 >> arg2;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = getUserEvents(arg1, arg2, &event_list);

                switch(answer) {
                    // Error in inputs
                    case -4:
                        state = "ERR";
                        break;
                    // Password is incorrect
                    case -3:
                        state = "WRP";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NLG";
                        break;
                    // User has not created any events
                    case -1:
                        state = "NOK";
                        break;
                    // User has created events
                    case 0:
                        state = "OK";
                        break;
                }
            }

            msg << "RME" << " " << state;
            std::cout << "Sending UDP: RME " << state << "\n[\n";

            for(size_t  i = 0; i < event_list.size(); i++) {
                msg << " " << event_list[i] << " " << getEventState(event_list[i]);
                std::cout << "EID: " << event_list[i] << "  State: " << event_list[i] << "\n";
            }
            msg << "\n";
            std::cout << "]\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case LIST:{ if (verbose == 1){std::cout << "LIST chosen\n";}                               // TCP - LST -> RLS status[ EID name state event_date ]*

            int answer;
            std::string state;

            std::vector<std::string> event_list;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                answer = getAllEvents(&event_list);

                switch(answer) {
                    // No event was created yet
                    case -1:
                        state = "NOK";
                        break;
                    // There are events 
                    case 0:
                        state = "OK";
                        break;
                }
            }

            msg << "RLS" << " " << state;
            std::cout << "Sending TCP: RLS " << state << "\n[\n";

            for(size_t  i = 0; i < event_list.size(); i++) {
                msg << " " << event_list[i] << " " << getEventName(event_list[i]) << " " << getEventState(event_list[i]) << " " << getEventDate(event_list[i]);
                std::cout << "EID: " << event_list[i] << "  Event name: " << getEventName(event_list[i]) << "  Event state: " << getEventState(event_list[i]) << "  Event date: " << getEventDate(event_list[i]) << "\n";
            }
            msg << "\n";
            std::cout << "]\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case SHOW:{ if (verbose == 1){std::cout << "SHOW chosen\n";}                                 // TCP - SED EID -> RSE status [UID name event_date attendance_size Seats_reserved Fname Fsize Fdata]
            int answer;
            std::string state;

            std::vector<std::string> file_details;
            std::string text_to_insert = "";
            std::string path = "./";
            std::string event_state_message;
            ss >> arg1;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else{
                // arg1 is EID
                answer = getEventDetails(arg1, &file_details);

                switch(answer) {
                    // Error in inputs
                    case -2:
                        state = "ERR";
                        msg << "RSE " << state << "\n";
                        std::cout << "Sending TCP: RSE " << state << "\n";
                        break;
                    // No event was created yet
                    case -1:
                        state = "NOK";
                        msg << "RSE " << state << "\n";
                        std::cout << "Sending TCP: RSE " << state << "\n";
                        break;
                    // There are events 
                    case 0:
                        state = "OK";
                        msg << "RSE " << state << " " << getEventUser(arg1) << " " << getEventName(arg1) << " " << getEventDate(arg1) << " " << getEventAttendees(arg1) << " " << getEventReservedSeats(arg1);
                        std::cout << "Sending TCP: RSE " << state << "  [Event user: " << getEventUser(arg1) << "  Event name: " << getEventName(arg1) << "  Event date:" << getEventDate(arg1) << "  Max attendees: " << getEventAttendees(arg1) << "  Seats reserved: " << getEventReservedSeats(arg1) << "]\n";

                        // Percorrer file_details e adicionar todos à msg
                        for(size_t  i = 0; i < file_details.size(); i++) {
                            msg << " " << file_details[i];

                        }
                        msg << "\n";
                        break;
                }
            }

            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case RESERVE:{ if (verbose == 1){std::cout << "RESERVE chosen   UID:" << arg1 <<"\n";}                       // TCP - RID UID password EID people -> RRI status [ n_seats ]*

            int answer;
            std::string state, seats_left;
            ss >> arg1 >> arg2 >> arg3 >> arg4;

            // Confirm command ends with \n
            if (ss.peek() != '\n') {
                state = "ERR";
            }
            else {
                answer = reserveEvent(arg1, arg2, arg3, arg4, &seats_left);
                switch (answer){
                    // Error in inputs
                    case -8:
                        state = "ERR";
                        break;
                    // Number of reservation higher than available spots
                    case -7:
                        state = "REJ";
                        break;
                    // Event is sold out
                    case -6:
                        state = "SLD";
                        break;
                    // Event is closed
                    case -5:
                        state = "CLS";
                        break;
                    // Event date passed
                    case -4:
                        state = "PST";
                        break;
                    // Password is wrong
                    case -3:
                        state = "WRP";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NLG";
                        break;
                    // Event not active
                    case -1:
                        state = "NOK";
                        break;
                    case 0:
                    // Password is correct
                        state = "ACC";
                        break;
                }
            }

            if (state == "REJ") {
                msg << "RRI" << " " << state << " " << seats_left << "\n";
            } else {
                msg << "RRI" << " " << state << "\n";
            }
            std::cout << "Sending TCP: RRI " << state << " " << seats_left << "\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        case MY_RESERVATION:{ if (verbose == 1){std::cout << "MY_RESERVATION chosen   UID:" << arg1 <<"\n";}               // UDP - LMR UID password -> RMR status[ EID date value]*
            int answer;
            std::string state;

            std::vector<std::string> reservation_list;
            ss >> arg1 >> arg2;

            // Confirm command ends with \n
            if (ss.peek() != '\n' || arg3 != "") {
                state = "ERR";
            }
            else{
                answer = getUserReservations(arg1, arg2, &reservation_list);
                switch (answer){
                    // Error in inputs
                    case -4:
                        state = "ERR";
                        break;
                    // Password is incorrect
                    case -3:
                        state = "WRP";
                        break;
                    // User is not loged in
                    case -2:
                        state = "NLG";
                        break;
                    // User has not made any reservation 
                    case -1:
                        state = "NOK";
                        break;
                    // User has created events
                    case 0:
                        state = "OK";
                        break;
                }
            }
 

            msg << "RMR" << " " << state;
            std::cout << "Sending TCP: RMR " << state << "\n[\n";
            // Por default, os diretorios sao lidos por ordem alfabetica logo os eventos reservados primeiro/as reservas mais antigas estão no início
            // Queremos as 50 reservas mais recentes
            // Comecar no 0, parar quando se excede o vetor ou então quando se chega a 50 
            for(size_t  i = 0; i < reservation_list.size() && i < 50; i++) {
                // size - 50 é o primeiro, o endereco base, e depois vamos aumentando com o i
                // seria size - 50 + i, mas se size for menor que 50 então seria 50 - size + 1. Logo usa-se absolute value std::fabs
                int j = std::fabs(reservation_list.size() - 50) + i;
                msg << " " << getReservationEID(reservation_list[i]) << " " << getReservationDate(reservation_list[i]) << " " << getReservationSeats(reservation_list[i]);
                std::cout << "EID: " << getReservationEID(reservation_list[i]) << "  Reservation date: " << getReservationDate(reservation_list[i]) << "  Reserved seats: " << getReservationSeats(reservation_list[i]) << "\n";
            }
            msg << "\n";
            std::cout << "]\n";
            msg_str = msg.str();
            std::copy(msg_str.begin(), msg_str.end(), std::back_inserter(message));


            break;
        }

        default:    std::cout << "Unknown\n"; break;  // 

    }

    return message;
}


int main(int argc, char* argv[]){
    char default_port[] = "58059";   // 58059 = 58000 + 59 (group number)

    char* server_port = default_port;
    bool verbose = 0;

    int fd_UDP, fd_TCP, errcode;
    ssize_t n;
    struct addrinfo hints, *res_UDP, *res_TCP;

    // For loop
    fd_set readfds;
    int maxfd;
    struct sockaddr_storage client_addr;
    socklen_t addrlen;
    char buffer_UDP[BUFSIZ];//, buffer_TCP[BUFSIZ];
    std::vector<char> buffer_TCP;
    std::vector<char> buffer_UDP_response, buffer_TCP_response;       // std::strings nao precisam de tamanho já definido, são dynamically allocated


    // Command argument parsing
    // ./ES -p ESport -v
    // argv[0] argv[1] argv[2] argv[3]
    if(argc > 1 && strcmp(argv[1], "-p") == 0) {
        server_port = argv[2];

        if(argc > 3 && strcmp(argv[3], "-v") == 0) {
            verbose = 1;
        }

    } else if(argc > 1 && strcmp(argv[1], "-v") == 0) {
        verbose = 1;

        if(argc > 2 && strcmp(argv[2], "-p") == 0) {
            server_port = argv[3];
        }
    }


    std::cout << "Listening on port " << server_port << "...\n";

    // UDP

    fd_UDP = socket(AF_INET, SOCK_DGRAM, 0);    // UDP socket
    if (fd_UDP == -1) {
		perror("main UDP socket");
		return -1;
	}

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;              // IPv4
    hints.ai_socktype = SOCK_DGRAM;         // UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, server_port, &hints, &res_UDP);
    if (errcode != 0) {
		perror("main UDP getaddrinfo");
		return -1;
	}

    n = bind(fd_UDP, res_UDP->ai_addr, res_UDP->ai_addrlen);
    if (n == -1) {
		perror("main UDP bind");
		return -1;
	}


    //TCP

    fd_TCP = socket(AF_INET, SOCK_STREAM, 0);        // TCP socket
    if (fd_TCP == -1) {
		perror("main TCP socket");
		return -1; 
	}

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;                   // IPv4
    hints.ai_socktype = SOCK_STREAM;             // TCP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, server_port, &hints, &res_TCP);
    if (errcode != 0) {
		perror("main TCP getaddrinfo");
        return -1;
	}

    // Quando server fecha abrutamente, n fica com socket aberto
    int opt = 1;
    setsockopt(fd_TCP, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    n = bind(fd_TCP, res_TCP->ai_addr, res_TCP->ai_addrlen);
    if (n == -1) {
		perror("main TCP bind");
        return -1;
	}

    if (listen(fd_TCP, 5) == -1) {
		perror("main TCP listen");
		return -1;
	}


    // LOOP

    while (1) {
        FD_ZERO(&readfds);           // clear the set
        FD_SET(fd_UDP, &readfds);    // watch UDP socket
        FD_SET(fd_TCP, &readfds);    // watch TCP socket

        // select needs the highest fd + 1
        maxfd = (fd_UDP > fd_TCP ? fd_UDP : fd_TCP) + 1;

        int activity = select(maxfd, &readfds, NULL, NULL, NULL);
        if (activity == -1) {
			perror("main select");
            return -1;
		}

        buffer_UDP[0] = '\0';
        buffer_TCP = {};
        buffer_UDP_response = {};
        buffer_TCP_response = {};
         
        // --- UDP ready? ---
        if (FD_ISSET(fd_UDP, &readfds)) {
            addrlen = sizeof(client_addr);
            n = recvfrom(fd_UDP, buffer_UDP, sizeof(buffer_UDP)-1, 0, (struct sockaddr*)&client_addr, &addrlen);
            if (n == -1) {
				perror("main recvfrom");
                return -1;
			}
            buffer_UDP[n] = '\0';

            if (verbose == 1){
                char ip[INET6_ADDRSTRLEN];
                int port = 0;

                if (client_addr.ss_family == AF_INET) {
                    auto *addr = (sockaddr_in*)&client_addr;
                    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
                    port = ntohs(addr->sin_port);
                } else if (client_addr.ss_family == AF_INET6) {
                    auto *addr = (sockaddr_in6*)&client_addr;
                    inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
                    port = ntohs(addr->sin6_port);
                }

                std::cout << "UDP from IP: " << ip << " Port: " << port << "  | "; 
            }

            buffer_UDP_response = server_command(buffer_UDP, verbose);

            // echo back to client
            n = sendto(fd_UDP, buffer_UDP_response.data(), buffer_UDP_response.size(), 0, (struct sockaddr*)&client_addr, addrlen);
            if (n == -1) {
				perror("main sendto");
                return -1;
			}
				
        }



        // --- TCP ready? ---
        if (FD_ISSET(fd_TCP, &readfds)) {
            int newfd;
            addrlen = sizeof(client_addr);
            newfd = accept(fd_TCP, (struct sockaddr*)&client_addr, &addrlen);
            if (newfd == -1) {
				perror("main accept");
                return -1;
			}

            if (verbose == 1){
                char ip[INET6_ADDRSTRLEN];
                int port = 0;

                if (client_addr.ss_family == AF_INET) {
                    auto *addr = (sockaddr_in*)&client_addr;
                    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
                    port = ntohs(addr->sin_port);
                } else if (client_addr.ss_family == AF_INET6) {
                    auto *addr = (sockaddr_in6*)&client_addr;
                    inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
                    port = ntohs(addr->sin6_port);
                }

                std::cout << "TCP client connected from " << ip << ":" << port << "  | "; 
            } 

            // read data from TCP client
			n = read_string(newfd, &buffer_TCP, BUFSIZ);
            if (n == -1) {
				perror("main TCP read");
                return -1;
			}
            buffer_TCP_response = server_command(buffer_TCP.data(), verbose);            

            // echo back to client
            n = safe_write(newfd, buffer_TCP_response);
            if (n == -1) {
				perror("main TCP write");
                return -1;
			}

            close(newfd); // close connection after handling
        }
    }



    //END
    close(fd_UDP);
    close(fd_TCP);
    freeaddrinfo(res_UDP);
    freeaddrinfo(res_TCP);
}
