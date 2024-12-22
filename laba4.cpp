#include <pqxx/pqxx>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>


class DatabaseConnection {
private:
    pqxx::connection conn;

public:
    DatabaseConnection(const std::string& conn_str) : conn(conn_str) {}

    pqxx::connection& getConnection() {
        return conn;
    }
};


template <typename T1, typename T2>
class Pair {
private:
    T1 first;
    T2 second;

public:
    Pair(T1 first, T2 second) : first(first), second(second) {}

    T1 getFirst() const { return first; }
    T2 getSecond() const { return second; }
};


class Author {
private:
    pqxx::connection& conn;

public:
    Author(pqxx::connection& db_conn) : conn(db_conn) {}

    void addAuthor(const std::string& name, int birth_year) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO authors (name, birth_year) VALUES (" + txn.quote(name) + ", " + std::to_string(birth_year) + ")");
            txn.commit();
            logOperation("Author added: " + name);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void logOperation(const std::string& message) {
        std::ofstream log("log.txt", std::ios::app);
        log << "[" << currentDateTime() << "] " << message << std::endl;
    }

    static std::string currentDateTime() {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};


class Book {
private:
    pqxx::connection& conn;

public:
    Book(pqxx::connection& db_conn) : conn(db_conn) {}

    void addBook(const std::string& title, int author_id, int publication_year, const std::string& genre) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO books (title, author_id, publication_year, genre) VALUES (" 
                     + txn.quote(title) + ", " + std::to_string(author_id) + ", " 
                     + std::to_string(publication_year) + ", " + txn.quote(genre) + ")");
            txn.commit();
            logOperation("Book added: " + title);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void logOperation(const std::string& message) {
        std::ofstream log("log.txt", std::ios::app);
        log << "[" << Author::currentDateTime() << "] " << message << std::endl;
    }
};


class User {
private:
    pqxx::connection& conn;

public:
    User(pqxx::connection& db_conn) : conn(db_conn) {}

    void addUser(const std::string& name, const std::string& registration_date) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO users (name, registration_date) VALUES (" + txn.quote(name) + ", " + txn.quote(registration_date) + ")");
            txn.commit();
            logOperation("User added: " + name);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void logOperation(const std::string& message) {
        std::ofstream log("log.txt", std::ios::app);
        log << "[" << Author::currentDateTime() << "] " << message << std::endl;
    }
};


class BorrowedBook {
private:
    pqxx::connection& conn;

public:
    BorrowedBook(pqxx::connection& db_conn) : conn(db_conn) {}

    void borrowBook(int user_id, int book_id, const std::string& borrow_date) {
        try {
            pqxx::work txn(conn);
            txn.exec("INSERT INTO borrowed_books (user_id, book_id, borrow_date) VALUES (" 
                     + std::to_string(user_id) + ", " + std::to_string(book_id) + ", " + txn.quote(borrow_date) + ")");
txn.commit();
            logOperation("Book borrowed: User ID " + std::to_string(user_id) + ", Book ID " + std::to_string(book_id));
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void logOperation(const std::string& message) {
        std::ofstream log("log.txt", std::ios::app);
        log << "[" << Author::currentDateTime() << "] " << message << std::endl;
    }
};


void getTopBooks(pqxx::connection& conn) {
    try {
        pqxx::work txn(conn);
        pqxx::result res = txn.exec(
            "SELECT title, COUNT(*) AS borrow_count FROM books "
            "JOIN borrowed_books ON books.id = borrowed_books.book_id "
            "GROUP BY title ORDER BY borrow_count DESC LIMIT 3");

        std::cout << "Top 3 books by borrow count:\n";
        for (const auto& row : res) {
            std::cout << "Book: " << row["title"].c_str() << ", Borrowed: " << row["borrow_count"].as<int>() << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}


void displayMenu() {
    std::cout << "Library Management System:\n";
    std::cout << "1. Add Author\n";
    std::cout << "2. Add Book\n";
    std::cout << "3. Add User\n";
    std::cout << "4. Borrow Book\n";
    std::cout << "5. Get Top Books\n";
    std::cout << "6. Exit\n";
}

void handleMenu(DatabaseConnection& db_conn) {
    int choice;
    do {
        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string name;
                int birth_year;
                std::cout << "Enter author name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter birth year: ";
                std::cin >> birth_year;
                Author(db_conn.getConnection()).addAuthor(name, birth_year);
                break;
            }
            case 2: {
                std::string title, genre;
                int author_id, publication_year;
                std::cout << "Enter book title: ";
                std::cin.ignore();
                std::getline(std::cin, title);
                std::cout << "Enter author ID: ";
                std::cin >> author_id;
                std::cout << "Enter publication year: ";
                std::cin >> publication_year;
                std::cout << "Enter genre: ";
                std::cin.ignore();
                std::getline(std::cin, genre);
                Book(db_conn.getConnection()).addBook(title, author_id, publication_year, genre);
                break;
            }
            case 3: {
                std::string name, registration_date;
                std::cout << "Enter user name: ";
                std::cin.ignore();
                std::getline(std::cin, name);
                std::cout << "Enter registration date (YYYY-MM-DD): ";
                std::cin >> registration_date;
                User(db_conn.getConnection()).addUser(name, registration_date);
                break;
            }
            case 4: {
                int user_id, book_id;
                std::string borrow_date;
                std::cout << "Enter user ID: ";
                std::cin >> user_id;
                std::cout << "Enter book ID: ";
                std::cin >> book_id;
                std::cout << "Enter borrow date (YYYY-MM-DD): ";
                std::cin >> borrow_date;
                BorrowedBook(db_conn.getConnection()).borrowBook(user_id, book_id, borrow_date);
                break;
            }
            case 5: {
                getTopBooks(db_conn.getConnection());
                break;
            }
            case 6:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 6);
}


int main() {
    try {
        DatabaseConnection db_conn("dbname=library user=your_user password=your_password host=localhost");
        handleMenu(db_conn);
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
    }
    return 0;
}
