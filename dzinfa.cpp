#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <libpq-fe.h>
struct Sale {
    int sale_id;
    std::string sale_date;
    int product_id;
    int customer_id;
    int quantity;
    double amount;
};

std::vector<Sale> loadSalesData(const std::string& filename) {
    std::vector<Sale> sales;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла " << filename << std::endl;
        return sales;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        Sale sale;
        std::string temp;

        std::getline(ss, temp, ',');
        sale.sale_id = std::stoi(temp);
        std::getline(ss, sale.sale_date, ',');
        std::getline(ss, temp, ',');
        sale.product_id = std::stoi(temp);
        std::getline(ss, temp, ',');
        sale.customer_id = std::stoi(temp);
        std::getline(ss, temp, ',');
        sale.quantity = std::stoi(temp);
        std::getline(ss, temp, ',');
        sale.amount = std::stod(temp);
        sales.push_back(sale);
    }
    file.close();
    return sales;
}
void loadToPostgres(const std::vector<Sale>& sales) {
    PGconn* conn = PQconnectdb("dbname=your_db user=your_user password=your_password");
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Ошибка подключения: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return;
    }

    for (const auto& sale : sales) {
        std::ostringstream query;
        query << "INSERT INTO sales_fact (sale_id, sale_date, product_id, customer_id, quantity, amount) "
              << "VALUES (" << sale.sale_id << ", '" << sale.sale_date << "', "
              << sale.product_id << ", " << sale.customer_id << ", "
              << sale.quantity << ", " << sale.amount << ");";

        PGresult* res = PQexec(conn, query.str().c_str());
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            std::cerr << "Ошибка выполнения запроса: " << PQerrorMessage(conn) << std::endl;
        }
        PQclear(res);
    }
    PQfinish(conn);
}
