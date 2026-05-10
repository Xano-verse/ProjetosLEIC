#include "./server_support.h"

namespace fs = std::filesystem;


Option parseOption(const std::string& s) {
    if (s == "LIN") return LOGIN;
    if (s == "CPS")  return CHANGE_PASS;
    if (s == "UNR")  return UNREGISTERED;
    if (s == "LOU")  return LOGOUT;
    if (s == "CRE")  return CREATE;
    if (s == "CLS")  return CLOSE;
    if (s == "LME")  return MY_EVENTS;
    if (s == "LST")  return LIST;
    if (s == "SED")  return SHOW;
    if (s == "RID")  return RESERVE;
    if (s == "LMR")  return MY_RESERVATION;
    return INVALID;
}


// Funções estão pela ordem seguinte


// --------------------------------------------------------------------------------------------------------------
// Funções principais - correspondem aos comandos
int createLogin(const std::string &uid, const std::string &password);
int changePassword(const std::string &uid, const std::string &old_password, const std::string &new_password);
int unregister(const std::string &uid, const std::string &password);
int logout(const std::string &uid, const std::string &password);
int createEvent(const std::string &uid, const std::string &password, const std::string &name, const std::string &event_date, const std::string &event_time, const std::string &attendance_size, const std::string &Fname, const std::string &Fsize, const std::string &Fdata, std::string *eid);
int closeEvent(const std::string &uid, const std::string &password, const std::string &eid);
int getUserEvents(const std::string &uid, const std::string &password, std::vector<std::string> *event_list);       // my events command
int getAllEvents(std::vector<std::string> *event_list);                         // list command
int getEventDetails(std::string eid, std::vector<std::string> *file_details);   // show command
int reserveEvent(const std::string &uid, const std::string &password, const std::string &eid, const std::string &people, std::string *seats_left); //reserve command
int getUserReservations(const std::string &uid, const std::string &password, std::vector<std::string> *reservation_list); // my reservations command



// --------------------------------------------------------------------------------------------------------------
// Auxiliares de filesystem
bool userExists(const std::string &uid);

void createDirectory(const std::string &path);

bool checkFileContent(const std::string &path, const std::string &text);

std::vector<std::string> readFile(const std::string &path);

std::string getLoginPath(const std::string &uid);
std::string getPassPath(const std::string &uid);
std::string getCrePath(const std::string &uid, const std::string &eid);
std::string getResPath(const std::string &uid, const std::string &date, const std::string &time);


// --------------------------------------------------------------------------------------------------------------
// Auxiliares de autenticação
bool isLoggedIn(const std::string &uid);
bool checkPassword(const std::string &uid, const std::string &password);


// --------------------------------------------------------------------------------------------------------------
// Auxiliares de eventos e reservas
std::string createEID();

bool eidExists(const std::string &eid);
bool eidbyUser(const std::string &uid, const std::string &eid);
bool eidSoldOut(const std::string &eid);
bool eventPast(const std::string &eid);
void eventEnd(const std::string &eid, const std::string &date);
bool eventIsClosed(const std::string& eid);
void makeReserves(const std::string &uid, const std::string &eid, const std::string &people);
std::string seatsLeft(const std::string &eid); 

std::vector<std::string> getEventList(const std::string dirname);
std::vector<std::string> getEventContents(const std::string &eid);
std::string getEidStart(const std::string &eid);
std::string getEidRes(const std::string &eid);
std::string getEidEnd(const std::string &eid);
std::string getEventUser(const std::string &eid);
std::string getEventName(const std::string &eid);
std::string getEventFname(const std::string &eid);
std::string getEventAttendees(const std::string &eid);
std::string getEventDate(const std::string &eid);
std::string getEventReservedSeats(const std::string &eid);
int getEventState(const std::string &eid);

std::vector<std::string> getReservationContents(const std::string reservation);
std::string getReservationEID(const std::string reservation);
std::string getReservationSeats(const std::string reservation);
std::string getReservationDate(const std::string reservation);

// --------------------------------------------------------------------------------------------------------------
// Misc
std::string to3(int n);
std::string extractFromStream(std::stringstream& buffer, int n);
std::string getCurrentDateTime();
std::string get_datetime();





// --------------------------------------------------------------------------------------------------------------
// Funções principais - correspondem aos comandos


int createLogin(const std::string &uid, const std::string &password) {
    std::string login_path, pass_path, path;
    std::ofstream fp;
    
    // Cheks inputs
    if (!isUid(uid) || !isPass(password)){
        return -2;
    }

    path = "./USERS/" + uid;

    login_path = getLoginPath(uid);
    pass_path = getPassPath(uid);

    // If new user
    if (!directoryExists(path)){
        createDirectory(path);
        createDirectory(path + "/CREATED");
        createDirectory(path + "/RESERVED");
        createTextFile(login_path, "Logged in\n");
        createTextFile(pass_path, password);
        return 1;
    }

    // If old user registers again (the directories already exist)
    else if (!(fileExists(login_path)) && !(fileExists(pass_path))) {
        createDirectory(path);
        createTextFile(login_path, "Logged in\n");
        createTextFile(pass_path, password);
        return 1;
    }

    // Already registered
    else{
        // Check password correct
        if (checkFileContent(pass_path, password)){
            createTextFile(login_path, "Logged in\n");
            return 0;
        }
    }
    
    return -1;
}


int changePassword(const std::string &uid, const std::string &old_password, const std::string &new_password){

    // Cheks inputs
    if (!isUid(uid) || !isPass(old_password) || !isPass(new_password)){
        return -4;
    }

    // User doesn't exist
    if (!userExists(uid)){
        return -1;
    }

    // User is not loged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    // Password is correct
    if (checkPassword(uid, old_password)){

        createTextFile(getPassPath(uid), new_password);
        return 0;
    }
    // Password wrong
    return -3;
}


int unregister(const std::string &uid, const std::string &password) {

    // Cheks inputs
    if (!isUid(uid) || !isPass(password)){
        return -4;
    }
    // User doesn't exist
    if (!userExists(uid)){
        return -1;
    }
    
    // User is not loged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -3;
    }
    // Password correct

    std::filesystem::remove(getLoginPath(uid));
    std::filesystem::remove(getPassPath(uid));
    return 0;
}

int logout(const std::string &uid, const std::string &password) {

    // Cheks inputs
    if (!isUid(uid) || !isPass(password)){
        return -4;
    }
    // User doesn't exist
    if (!userExists(uid)){
        return -1;
    }
    
    // User is not logged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -3;
    }

    std::filesystem::remove(getLoginPath(uid));
    return 0;
}

int createEvent(const std::string &uid, const std::string &password, const std::string &name, const std::string &event_date, const std::string &event_time, const std::string &attendance_size, const std::string &Fname, const std::string &Fsize, const std::vector<char> &Fdata, std::string *eid) {

    std::string path, path_desc, path_res;

    // Cheks inputs
    if (!isUid(uid) || !isPass(password) || !validateFname(Fname) || !isValidDateTime(event_date, event_time) || !isNumberXto999(attendance_size, 10) || std::stoi(Fsize) > 10000000){
        return -4;
    }

    // Check if its a past date
    if (isPastDate(event_date + " " + event_time)){
        return -1;
    }

    // User is not logged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -3;
    }
    
    *eid = createEID();

    path = "EVENTS/" + *eid;
    path_desc = "EVENTS/" + *eid + "/DESCRIPTION";
    path_res = "EVENTS/" + *eid + "/RESERVATIONS";

    createDirectory(path);
    createDirectory(path_desc);
    createDirectory(path_res);

    createTextFile(getEidStart(*eid), (uid + " " + name + " " + Fname + " " + attendance_size + " " + event_date + " " + event_time + ""));
    createTextFile(getEidRes(*eid), "0");
    createRawFile((path_desc + "/" + Fname + ""), Fdata);

    // Create file on User CREATED
    createTextFile(getCrePath(uid, *eid), "Event created");

    return 0;
}

int closeEvent(const std::string &uid, const std::string &password, const std::string &eid) {

    // Cheks inputs
    if (!isUid(uid) || !isPass(password) || !isEid(eid)){
        return -8;
    }

    // User doesn't exist
    if (!userExists(uid)){
        return -1;
    }
    
    // User is not logged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -1;
    }

    // Event doesn´t exist
    if (!eidExists(eid)){
        return -3;
    }

    // Event is not from the user
    if (!eidbyUser(uid, eid)){
        return -4;
    }

    // Was already sold out
    if (eidSoldOut(eid)){
        return -5;
    }

    // Event date is Past
    if (eventPast(eid)){
        return -6;
    }

    // Is already closed
    if (eventIsClosed(eid)){
        return -6;
    }

    eventEnd(eid, getCurrentDateTime());

    return 0;
}


int getUserEvents(const std::string &uid, const std::string &password, std::vector<std::string> *event_list) {
    std::string dirname = "USERS/" + uid + "/CREATED/";
    
    // Cheks inputs
    if (!isUid(uid) || !isPass(password)){
        return -4;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -3;
    }

    // User is not logged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    *event_list = getEventList(dirname);

    if (event_list->size() == 0) {
        return -1;
    }


    return 0;
}

int getAllEvents(std::vector<std::string> *event_list) {
    std::string dirname = "EVENTS/";

    *event_list = getEventList(dirname);

    if (event_list->size() == 0) {
        return -1;
    }

    return 0;
}

int getEventDetails(std::string eid, std::vector<std::string> *file_details) {

    // Cheks inputs
    if (!isEid(eid)) {  
        return -2;
    }

    if (!eidExists(eid)) {
        return -1;
    }
    std::string path = "./EVENTS/" + eid + "/DESCRIPTION/" + getEventFname(eid);

    if(!fileExists(path)) {
        return -1;
    }

    std::vector<std::string> file_data = readFile(path);

    file_details->push_back(getEventFname(eid));
    file_details->push_back(getFileSize(path));
    
    // Percorrer todas as strings do file_data e adicionar aos file_details
    // file_details será igual a file_data com os 2 primeiros elementos sendo o file name e file size
    for(int i = 0; i < file_data.size(); i++) {
        file_details->push_back(file_data[i]);
    }

    return 0; 


}

int reserveEvent(const std::string &uid, const std::string &password, const std::string &eid, const std::string &people, std::string *seats_left) {

    // Cheks inputs
    if (!isUid(uid) || !isPass(password) || !isEid(eid) || !isNumberXto999(people, 1)){
        return -8;
    }

    // Event not active
    if (!eidExists(eid)){
        return -1;
    }
    
    // User is not logged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -3;
    }

    // Event date passed
    if (eventPast(eid)){
        return -4;
    }

    //  Event is closed
    if (eventIsClosed(eid)){
        return -5;
    }
   
    // Event is sold out
    if (eidSoldOut(eid)){
        return -6;
    }

    // Number of reservation higher than available spots
    if (std::stoi(seatsLeft(eid)) < std::stoi(people)) {
        (*seats_left) = seatsLeft(eid);
        return -7;
    }

    makeReserves(uid, eid, people);

    return 0;
}

int getUserReservations(const std::string &uid, const std::string &password, std::vector<std::string> *reservation_list) {
    std::string dirname = "USERS/" + uid + "/RESERVED/";
    
    // Cheks inputs
    if (!isUid(uid) || !isPass(password)){
        return -4;
    }

    // Password is wrong
    if (!checkPassword(uid, password)){
        return -3;
    }

    // User is not logged in
    if (!isLoggedIn(uid)){
        return -2;
    }

    *reservation_list = getEventList(dirname);

    if (reservation_list->size() == 0) {
        return -1;
    }


    return 0;
}

// --------------------------------------------------------------------------------------------------------------
// Auxiliares de filesystem

// Cheks if user exists in data base
bool userExists(const std::string &uid){
    std::string path;

    path = "USERS/" + uid;

    return directoryExists(path);
}


void createDirectory(const std::string &path) {
    int ret = mkdir(path.c_str(), 0700);

    if (ret == -1) {
        if (errno == EEXIST) {
            // Diretório já existe → não é erro
            return;
        }
    }
}

// Checks if the file is empty or not
bool checkFileContent(const std::string &path, const std::string &text){
    std::ifstream fp;
    std::stringstream buffer;

    fp.open(path);
    if (!fp.is_open()) {
        return false;
    }

    buffer << fp.rdbuf();

    fp.close();

    if (buffer.str() == text){
        return true;
    }

    return false;
}

std::vector<std::string> readFile(const std::string &path) {
    std::ifstream fp;
    std::stringstream buffer;

    std::vector<std::string> contents;

    fp.open(path);
    if(!fp.is_open()) {
        perror("readFile open");
        return {};
    }

    buffer << fp.rdbuf();

    fp.close();

    // Com cada chamada vamos retirando da std::stringstream (vamos separar por espaços)
    for(std::string line; std::getline(buffer, line, ' '); ) {
        contents.push_back(line);
    }

    return contents;

}

std::string getLoginPath(const std::string &uid){
    return "USERS/" + uid + "/" + uid + "_login.txt";
}

std::string getPassPath(const std::string &uid){
    return "USERS/" + uid + "/" + uid + "_pass.txt";
}

std::string getCrePath(const std::string &uid, const std::string &eid){
    return "USERS/" + uid + "/CREATED/" + eid + ".txt";
}

std::string getResPath(const std::string &uid, const std::string &date, const std::string &time){
    return "USERS/" + uid + "/RESERVED/R-" + uid + "-" + date + " " + time + ".txt";
}

std::string getEidStart(const std::string &eid){
    return "EVENTS/" + eid + "/START_" + eid + ".txt";
}

std::string getEidRes(const std::string &eid){
    return "EVENTS/" + eid + "/RES_" + eid + ".txt";
}

std::string getEidEnd(const std::string &eid){
    return "EVENTS/" + eid + "/End_" + eid + ".txt";
}


// --------------------------------------------------------------------------------------------------------------
// Auxiliares de autenticação

// Cheks if user is logged in
bool isLoggedIn(const std::string &uid){
    std::string path;

    path = "USERS/" + uid + "/" + uid + "_login.txt";

    return fileExists(path);
}

// Cheks if password is correct
bool checkPassword(const std::string &uid, const std::string &password){
    std::string path;

    path = "USERS/" + uid + "/" + uid + "_pass.txt";

    return checkFileContent(path, password);
}


// --------------------------------------------------------------------------------------------------------------
// Auxiliares de eventos e reservas

std::string createEID(){
    
    std::string path;
    int count;

    path = "EVENTS/";

    count = 0;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            count++;
        }
    }

    return to3(count + 1);

}

bool eidExists(const std::string &eid){
    std::string path;

    path = "./EVENTS/" + eid;

    return directoryExists(path);
}

bool eidbyUser(const std::string &uid, const std::string &eid){
    std::string path;

    if (uid.size() != 6) {
        perror("userExists uid length wrong");
        return -1;
    }

    if (eid.size() != 3) {
        perror("eidExists eid length wrong");
        return -1;
    }

    path = "USERS/" + uid + "/CREATED/" + eid + ".txt";

    return fileExists(path);
}

bool eidSoldOut(const std::string &eid){

    std::string max_atendees, num_atendees;

    if (eid.size() != 3) {
        perror("eidSoldOut eid length wrong");
        return -1;
    }

    max_atendees = getEventAttendees(eid);

    num_atendees = getEventReservedSeats(eid);

    return max_atendees == num_atendees;
}

bool eventPast(const std::string &eid){
    std::string date;

    if (eid.size() != 3) {
        perror("eventPast length wrong");
        return -1;
    }

    if (isPastDate(getEventDate(eid))){

        // Closes event if not already closed
        if (!(eventIsClosed(eid))){
            createTextFile(getEidEnd(eid), getEventDate(eid));
        }
        return true;
    }
    return false;
}

void eventEnd(const std::string &eid, const std::string &date){
    createTextFile(getEidEnd(eid), date);
}

bool eventIsClosed(const std::string& eid) {
    return fileExists(getEidEnd(eid));
}

void makeReserves(const std::string &uid, const std::string &eid, const std::string &people){
    std::string path_user, path_events, content, file_name, time, new_reserved_seats;

    time = get_datetime();

    file_name = "R-" + uid + "-" + time + ".txt";

    path_user = "USERS/" + uid + "/RESERVED/" + file_name;
    path_events = "EVENTS/" + eid + "/RESERVATIONS/" + file_name;
    content = eid + " " + people + " " + getCurrentDateTime(); 

    createTextFile(path_user, content);
    createTextFile(path_events, content);

    // Write on Event Res
    new_reserved_seats = std::to_string(std::stoi(getEventReservedSeats(eid)) + std::stoi(people));
    createTextFile(getEidRes(eid), new_reserved_seats);
}

std::string seatsLeft(const std::string &eid) {//, const std::string &people){

    std::string max_atendees, num_atendees;

    max_atendees = getEventAttendees(eid);

    num_atendees = getEventReservedSeats(eid);


    return std::to_string(std::stoi(max_atendees) - std::stoi(num_atendees));
}

// Listagem ordenada de conteudos de directorias
std::vector<std::string> getEventList(const std::string dirname) {
    std::vector<std::string> event_list;

    struct dirent **filelist;
    int n_entries, no_events;
    int i_event = 0, i_entry = 0;

    n_entries = scandir(dirname.c_str(), &filelist, 0, alphasort);

    if (n_entries <= 0) {
        perror("GetEventList n_entries");
        exit(1);
    } else {
        // Percorrer directory entries
        while (i_entry < n_entries) {
            // Skip . and .. entries (and other entries starting with .) (doesn't increment i_event, but increments i_entry)
            if(filelist[i_entry]->d_name[0] == '.') {
                ++i_entry;
                continue;
            }

            // Copy entry name
            event_list.insert(event_list.begin(), filelist[i_entry]->d_name);

            // Se a entry nao for um diretorio, remover a extensão que será .txt
            // (entry sera diretorio no caso de listar todos os eventos em EVENTS, onde cada evento é um diretorio)
            if (filelist[i_entry]->d_type != DT_DIR) {
                event_list.front().resize(event_list.front().size() - 4);
            }
            
            free(filelist[i_entry]);
            ++i_event;
            ++i_entry;
        }
        free(filelist);
    }

    return event_list;

}


std::vector<std::string> getEventContents(const std::string &eid) {
    std::string path = "EVENTS/" + eid + "/START_" + eid + ".txt";

    std::vector<std::string> contents = readFile(path);

    // UID name file_name num_attendees dd-mm-yyyy hh:mm 
    return contents;
}


std::string getEventUser(const std::string &eid) {
    return getEventContents(eid)[0];
}

std::string getEventName(const std::string &eid) {
    return getEventContents(eid)[1];
}

std::string getEventFname(const std::string &eid) {
    return getEventContents(eid)[2];
}

std::string getEventAttendees(const std::string &eid) {
    return getEventContents(eid)[3];
}

std::string getEventDate(const std::string &eid) {
    return getEventContents(eid)[4] + " " + getEventContents(eid)[5];
}

std::string getEventReservedSeats(const std::string &eid) {
    return readFile(getEidRes(eid))[0];
}

// state is 0 if the event is in the past, 1 if is still accepting reservations, 2 if is sold out, 3 if it was closed by the user
int getEventState(const std::string &eid) {
    std::string end_file_path;
    std::string num_attendees, num_reserved_seats;

    // If event is in the past
    if (eventPast(eid)){
        return 0;
    }

    // If END_eid exists, event has been closed by the user
    if(eventIsClosed(eid)) {
        return 3;
    }

    num_attendees = getEventAttendees(eid);
    num_reserved_seats = getEventReservedSeats(eid);

    // If total seats != reserved seats state is 1, accepting reservations, else it's 2, sold out
    if(num_attendees != num_reserved_seats) {
        return 1;
    } else {
        return 2;
    }

}

std::vector<std::string> getReservationContents(const std::string reservation) {
    // R-123456-YYYYMMDD_HHMMSS.txt
    // 01234567
    std::string uid = reservation.substr(2, 6);         // extract 6 characters starting from index 2
    std::string path = "./USERS/" + uid + "/RESERVED/" + reservation + ".txt";

    std::vector<std::string> contents = readFile(path);

    // EID reserved_seats dd-mm-yyyy hh:mm:ss 
    return contents;
}


std::string getReservationEID(const std::string reservation) {
    return getReservationContents(reservation)[0];
}

std::string getReservationSeats(const std::string reservation) {
    return getReservationContents(reservation)[1];
}

std::string getReservationDate(const std::string reservation) {
    return getReservationContents(reservation)[2] + " " + getReservationContents(reservation)[3];
}

// --------------------------------------------------------------------------------------------------------------
// Misc


std::string to3(int n) {
    std::ostringstream oss;
    oss << std::setw(3) << std::setfill('0') << n;
    return oss.str();
}

std::string extractFromStream(std::stringstream& buffer, int n) {
    std::string word;
    int index = 0;

    while (buffer >> word) {
        index++;
        if (index == n) {
            break;
        }
    }
    return word;
}

std::string getCurrentDateTime() {
    using namespace std::chrono;

    // Get current time
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

    // Convert to local time
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &t);
#else
    local = *std::localtime(&t);
#endif

    // Format: DD-MM-YYYY HH:MM
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &local);

    return std::string(buffer);
}

std::string get_datetime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

