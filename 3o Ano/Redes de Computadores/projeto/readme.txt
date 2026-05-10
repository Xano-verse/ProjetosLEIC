Redes de Computadores Project
2025/26

109441 Alexandre Carapeto Delgado
109834 Ricardo Duarte Rosa da Fonseca


Overview

This project implements an event reservation platform, according to the client-server paradigm, where users can create events, make reservations and check ongoing or closed events, among other things.
The program uses both UDP and TCP protocols to exchange information between client and server, thus allowing for each of them to be running on seperate machines. This project also supports concurrent user sessions to the same server.

The user application has the following commands available:
- login, which establishes a session for an existing user or registers a new user. Each user is identified with a unique UID;
- create, which creates a new event with a short description, its date and time, the available seats, among other details;
- close, which closes an ongoing event owned by the logged in user;
- myevents, which lists the events owned by the logged in user, along with their booking status;
- list, which lists all existing events and some of their details;
- show, which shows the details of a specific event;
- reserve, which makes a reservation for a certain amount of people in a given event;
- myreservations, which lists the reservations made by the logged in user;
- changePass, which allows for the logged in user's password to be changed;
- unregister, which unregisters the logged in user from the server;
- logout, which logs out from the logged in user's session;
- exit, which terminates the user application.


Compiling

All of the code is present in the src/ directory, divided into three subdirectories: user/, server/ and common/.
The project can be compiled by running the make command in this directory. As specified by the project guidelines, this will place both executables, ES and user, in the current directory.


Running the User Application

The user application can be run by doing
./user [-n ESIP] [-p ESport]
where -n and -p are optional arguments specifying the server's IP address and port, respectively.


Running the Event-reservation Server

The server can be started by doing
./ES [-p ESport] [-v]
where -p and -v are optional arguments. The -p argument specifies the port where the server will accept requests and the -v argument enables verbose mode, where a short description of the received requests is output to the screen.

 
Technical Details

This project was made using C++17 and tested with GCC/G++ 12.2.0.
The project uses both UDP and TCP sockets.
UDP: used for managing users and listing users' activities.
TCP: used to transfer files with more information to the user application, and managing events and reservations.
The maximum file size is 10MB.


File List

readme.txt - this file
Makefile - compiling instructions
Autoavaliação.xlsx - self-assessment excel spreadsheet
src/client/main.cpp - file implementing the user application
src/common/operations.cpp - auxiliary functions used by both client and server applications
src/common/operations.h - header file for operations.cpp
src/server/main.cpp - file implementing the core of the server application
src/server/server_support.cpp - auxiliary functions exclusive to server functionality
src/server/server_support.h - header file for server_support.cpp


