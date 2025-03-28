#include <cmath>
#include <cstdio>
#include <iostream>
#include <sqlite3.h>
#include <string>

static int callback(void *taskData, int numberOfColoums, char **coloumValues, char **colomNames){
    int i;
    for (int i = 0; i < numberOfColoums; i++) {
        printf("-----------------------------------------------------------------\n");
        printf("%s: %s\n", colomNames[i], coloumValues[i] ? coloumValues[i] : "NULL");
        printf("-----------------------------------------------------------------\n");
    }
    return 0;
}

int connectDB(sqlite3** DB){
    int rc = sqlite3_open("domanic.db", DB);
       if (rc) {
           std::cerr << "DB Connection Failed: " << sqlite3_errmsg(*DB) << std::endl;
           return 1;
       } 
   
       std::string sql = "CREATE TABLE IF NOT EXISTS domanic (_id INTEGER PRIMARY KEY AUTOINCREMENT, tasks VARCHAR(255));";
       char* errMsg = nullptr;
       rc = sqlite3_exec(*DB, sql.c_str(), callback, 0, &errMsg);
   
       if (rc != SQLITE_OK) {
           std::cerr << "SQL ERROR: " << errMsg << std::endl;
           sqlite3_free(errMsg);
       } else {
           std::cout << "Database Ready" << std::endl;
       }
       return 0;
}

int showTasks(sqlite3* DB){
    if (DB == nullptr) {
        std::cout << "Database connection is null!" << std::endl;
        return 1;
    }
    std::string sql = "SELECT * FROM domanic";
    int rc = sqlite3_exec(DB, sql.c_str(), callback, 0, nullptr);
    std::cout << rc << std::endl;
    return 0;
}

int addTasks(sqlite3* DB, const std::string task){
    if (DB == nullptr) {
        std::cout << "Database connection is null!" << std::endl;
        return 1;
    }
    
    sqlite3_stmt* statement;
    const char* sql = "INSERT INTO domanic (tasks) VALUES (?);";
    
    if ( sqlite3_prepare_v2(DB, sql, -1, &statement, nullptr) != SQLITE_OK ) {
        std::cerr << "Not prepared" << std::endl;
        return 1;
    }
    
    if (sqlite3_bind_text(statement, 1, task.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        std::cerr << "Binding failed" << std::endl;
        return 1;
    }
    
    if(sqlite3_step(statement) != SQLITE_DONE){
        std::cerr << "Execution failed" << std::endl;
        return 1;
    }
    
    std::cout << "Task added" << std::endl;
    return 0;
}

int editTasks(sqlite3* DB, int id, const std::string updatedTask){
    if (DB == nullptr) {
        std::cout << "Database connection is null!" << std::endl;
        return 1;
    }
    
    sqlite3_stmt* statement;
    const char* sql = "UPDATE domanic SET tasks = ? WHERE _id = ?; ";
    
    if(sqlite3_prepare_v2(DB, sql, -1, &statement, nullptr) != SQLITE_OK){
        std::cerr << "Not prepared" << std::endl;
        return 1;
    }
    
    if (sqlite3_bind_text(statement, 1, updatedTask.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        std::cerr << "Binding failed: UpdatedTasks" << std::endl;
        sqlite3_finalize(statement);
        return 1;
    }
    
    if (sqlite3_bind_int(statement, 2, id) != SQLITE_OK) {
        std::cerr << "Binding failed: _id" << std::endl;
        sqlite3_finalize(statement);
        return 1;
    }
    
    if (sqlite3_step(statement) != SQLITE_DONE) {
        std::cerr << "Execution failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Task added successfully" << std::endl;
    sqlite3_finalize(statement);
    return 0;
}

int deleteTasks(sqlite3* DB, int id){
    if (DB == nullptr) {
        std::cout << "Database connection is null!" << std::endl;
        return 1;
    }
    
    sqlite3_stmt* statement;
    const char* sql = "DELETE FROM domanic WHERE _id = ? ;";
    
    if (sqlite3_prepare_v2(DB, sql, -1, &statement, nullptr) != SQLITE_OK) {
        std::cerr << "Not prepared" << std::endl;
        return 1;
    }
    
    if (sqlite3_bind_int(statement, 1, id) != SQLITE_OK) {
        std::cerr << "Binding failed: _id" << std::endl;
        sqlite3_finalize(statement);
        return 1;
    }
    
    if (sqlite3_step(statement) != SQLITE_DONE) {
        std::cerr << "Executation failed!" << std::endl;
        sqlite3_finalize(statement);
        return 1;
    }
    
    std::cout << "Task deleted!" << std::endl;
    return 0;
}

int main(){
    sqlite3* DB = nullptr;
    connectDB(&DB);
    
    int choice;
    std::string task;
    int id;
    std::cin.ignore();
    
    while (true){
        std::cout << "___________________________________________________" << std::endl;
        std::cout << "Select a choice :" << std::endl ;
        std::cout << "1. Show tasks" << std::endl;
        std::cout << "2. Add tasks" << std::endl;
        std::cout << "3. Edit tasks" << std::endl;
        std::cout << "4. Delete tasks" << std::endl;
        std::cout << "5. Exit" << std::endl;
        std::cout << "___________________________________________________" << std::endl;
        std::cin >> choice;

        switch (choice){
        case 1:
            showTasks(DB);
            break;
        case 2:
            std::cout << "Enter your task: " << std::endl;
            std::cin >> task;
            addTasks(DB, task);
            break;
        case 3:
            showTasks(DB);
            std::cout << "Select id: " << std::endl;
            std::cin >> id;
            std::cout << "Update your task: " << std::endl;
            std::cin >> task;         
            editTasks(DB, id, task);
            break;
        case 4:
            showTasks(DB);
            std::cout << "Select id: " << std::endl;
            std::cin >> id;
            deleteTasks(DB, id);
            break;
        case 5:
            sqlite3_close(DB);
            std::cout << "DISCONNECTED" << std::endl;
            return 0;
        default:
            break;
        }
    }
    return 0;
}