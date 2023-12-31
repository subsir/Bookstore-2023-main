#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "book_info.hpp"
#include "log.hpp"
#include "user.hpp"

std::vector<std::shared_ptr<user>> users;
Book book_db;
Log log_db("_log_file");
Log faculty_log_db("_faculty_log_file");
Log finance_log_db("_finance_log_file");
UserfileDatabase user_db("_user_file");

bool valid_check(int type, std::string &input) {
  if (input.size() > 30 and type < 3) {
    return false;
  }

  if (type ==
      0) {  // 0代表 [UserID], [Password], [CurrentPassword], [NewPassword]
    for (int i = 0; i < input.size(); i++) {
      if ((input[i] < '0' or input[i] > '9') and
          (input[i] < 'a' or input[i] > 'z') and
          (input[i] < 'A' or input[i] > 'Z') and input[i] != '_') {
        return false;
      }
    }
    return true;
  }
  if (type == 1) {  // 1代表[Username]
    for (int i = 0; i < input.size(); i++) {
      if (input[i] < 32 or input[i] > 126) {
        return false;
      }
    }
    return true;
  }
  if (type == 2) {  // 2代表[Privilege]
    if (input.size() != 1) {
      return false;
    }
    if (input[0] != '0' and input[0] != '1' and input[0] != '3' and
        input[0] != '7') {
      return false;
    }
    return true;
  }

  if (type == 3) {  // 3代表[ISBN]
    if (input == "") {
      return false;
    }
    if (input.size() > 20) {
      return false;
    }
    for (int i = 0; i < input.size(); i++) {
      if (input[i] < 32 or input[i] > 126) {
        return false;
      }
    }
    return true;
  }
  if (type == 4) {  // 4代表[BookName], [Author]
    if (input == "") {
      return false;
    }
    if (input.size() > 62) {
      return false;
    }
    if (input[0] != '\"' or input[input.size() - 1] != '\"') {
      return false;
    }
    for (int i = 1; i < input.size() - 1; i++) {
      if (input[i] < 32 or input[i] > 126 or input[i] == '\"') {
        return false;
      }
    }
    return true;
  }
  if (type == 5) {  // 5代表[Keyword]
    if (input == "\"\"") {
      return false;
    }
    if (input.size() > 62) {
      return false;
    }
    if (input[0] != '\"' or input[input.size() - 1] != '\"') {
      return false;
    }
    for (int i = 1; i < input.size() - 1; i++) {
      if (input[i] < 32 or input[i] > 126 or input[i] == '\"') {
        return false;
      }
      if (input[i] == '|' and (i == input.size() - 2 or input[i + 1] == '|')) {
        return false;
      }
      if (input[i] == '|' and (i == 1 or input[i - 1] == '|')) {
        return false;
      }
    }
    return true;
  }
  if (type == 6) {  // 6代表[Quantity]，[Count]
    if (input == "") {
      return false;
    }
    if (input.size() > 10) {
      return false;
    }
    if (input.size() == 10) {
      if (input > "2147483647") {
        return false;
      }
    }
    for (int i = 0; i < input.size(); i++) {
      if (input[i] < '0' or input[i] > '9') {
        return false;
      }
    }
    return true;
  }
  if (type == 7) {  // 7代表[Price], [TotalCost]
    if (input == "") {
      return false;
    }
    if (input.size() > 13) {
      return false;
    }
    int dot = -1;
    for (int i = 0; i < input.size(); i++) {
      if ((input[i] < '0' or input[i] > '9') and input[i] != '.') {
        return false;
      }
      if (input[i] == '.') {
        if (dot != -1 or i == 0 or i == input.size() - 1) {
          return false;
        }
        dot = i;
      }
    }
    return true;
  }
  return false;
}

void handleInput(std::string &input) {
  if (input == "exit" or input == "quit") {
    getline(std::cin, input);
    std::istringstream stream(input);
    std::string left;
    stream >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    std::cout << "\n";
    exit(0);
  } else if (input == "su") {
    getline(std::cin, input);
    std::istringstream stream(input);
    std::string id, password, left;
    stream >> id >> password >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    int pos = user_db.index_find(id);
    bool login = false;
    int temp_priority = 0;
    if (pos != -1) {
      temp_priority = user_db.get_priority(pos);
      if (users.empty() != true and temp_priority < users.back()->rank) {
        login = true;
      }
      if (password == user_db.check(pos)) {
        login = true;
      }
      if (password != user_db.check(pos) and password != "") {
        login = false;
      }
    }
    if (login) {
      if (temp_priority == 1) {
        std::shared_ptr<user> ptr;
        ptr = std::make_shared<customer>(id, password);
        users.push_back(ptr);
        std::cout << "\n";
      } else if (temp_priority == 3) {
        std::shared_ptr<user> ptr;
        ptr = std::make_shared<faculty>(id, password);
        users.push_back(ptr);
        std::cout << "\n";
      } else if (temp_priority == 7) {
        std::shared_ptr<user> ptr;
        ptr = std::make_shared<manager>(id, password);
        users.push_back(ptr);
        std::cout << "\n";
      }
    } else {
      std::cout << "Invalid" << std::endl;
    }
  } else if (input == "logout") {
    getline(std::cin, input);
    std::istringstream stream(input);
    std::string left;
    stream >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    if (users.empty()) {
      std::cout << "Invalid" << std::endl;
    } else {
      users.pop_back();
      std::cout << "\n";
    }
  } else if (input == "register") {
    getline(std::cin, input);
    std::istringstream stream(input);
    std::string id, password, username, left;
    stream >> id >> password >> username >> left;
    if (left != "" or id == "" or password == "" or username == "") {
      std::cout << "Invalid\n";
      return;
    }
    if (valid_check(0, id) and valid_check(0, password) and
        valid_check(1, username)) {
      if (user_db.index_find(id) == -1) {
        user_db.insert(id, username, password, 1);
        log_db.write("register id: " + id + " username: " + username +
                     " password: " + password);
        std::cout << "\n";
      } else {
        std::cout << "Invalid\n";
      }
    } else {
      std::cout << "Invalid\n";
    }
  } else if (input == "delete") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank != 7) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string id, left;
    stream >> id >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    bool login = false;
    for (auto it : users) {
      if (it->id == id) {
        login = true;
        break;
      }
    }
    if (login == false) {
      int pos = user_db.index_find(id);
      if (pos != -1) {
        user_db.remove(id);
        log_db.write(users.back()->id + " delete id: " + id);
        faculty_log_db.write(users.back()->id + " delete id: " + id);
        std::cout << "\n";
      } else {
        std::cout << "Invalid\n";
      }
    } else {
      std::cout << "Invalid\n";
    }
  } else if (input == "useradd") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 3) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string id, password, username, privilege, left;
    stream >> id >> password >> privilege >> username >> left;
    if (left != "" or id == "" or password == "" or username == "" or
        privilege == "") {
      std::cout << "Invalid\n";
      return;
    }
    if (valid_check(0, id) and valid_check(0, password) and
        valid_check(1, username) and valid_check(2, privilege)) {
      if (users.empty() or users.back()->rank <= (privilege[0] - '0')) {
        std::cout << "Invalid\n";
        return;
      }
      if (user_db.index_find(id) == -1) {
        user_db.insert(id, username, password, (privilege[0] - '0'));
        log_db.write(users.back()->id + " add id: " + id +
                     " username: " + username + " password: " + password);
        faculty_log_db.write(users.back()->id + " add id: " + id +
                             " username: " + username +
                             " password: " + password);
        std::cout << "\n";
      } else {
        std::cout << "Invalid\n";
      }
    } else {
      std::cout << "Invalid\n";
    }
  } else if (input == "passwd") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 1) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string id, current_password, new_password, left;
    stream >> id >> current_password >> new_password >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    if (new_password == "") {
      if (users.empty() == false and users.back()->rank == 7 and
          new_password == "") {
        new_password = current_password;
        current_password = "";
      } else {
        std::cout << "Invalid\n";
        return;
      }
    }
    if (valid_check(0, new_password)) {
      int pos = user_db.index_find(id);
      if (pos != -1) {
        if (current_password == "" or current_password == user_db.check(pos)) {
          int priority = user_db.get_priority(pos);
          user_db.remove(id);
          user_db.insert(id, "", new_password, priority);
          log_db.write(users.back()->id + " change password id: " + id +
                       " password: " + new_password);
          faculty_log_db.write(users.back()->id + " change password id: " + id +
                               " password: " + new_password);
          // suppose that the username will not be used
          std::cout << "\n";
        } else {
          std::cout << "Invalid\n";
        }
      } else {
        std::cout << "Invalid\n";
      }
    } else {
      std::cout << "Invalid\n";
    }
  } else if (input == "show") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 1) {
      std::cout << "Invalid\n\n";
      return;
    }
    std::istringstream stream(input);
    std::string info, left;
    stream >> info;
    if (info == "") {
      book_db.find(info, 4);
    } else if (info == "finance") {
      if (users.empty() or users.back()->rank < 7) {
        std::cout << "Invalid\n\n";
        return;
      }
      std::string count;
      stream >> count >> left;
      if (left != "") {
        std::cout << "Invalid\n\n";
        return;
      }
      if (count == "") {
        book_db.finance(-1);
      } else {
        if (valid_check(6, count) == false) {
          std::cout << "Invalid\n\n";
          return;
        }
        int count_int = -1;
        try {
          count_int = std::stoi(count);
        } catch (std::invalid_argument) {
          std::cout << "Invalid\n\n";
          return;
        }
        if (count_int < 0) {
          std::cout << "Invalid\n\n";
          return;
        }
        book_db.finance(count_int);
      }
    } else {
      stream >> left;
      if (left != "") {
        std::cout << "Invalid\n\n";
        return;
      }
      int split = -1;
      for (int i = 1; i < info.size(); i++) {
        if (info[i] == '=') {
          split = i;
          break;
        }
      }
      if (split == -1 or info[0] != '-') {
        std::cout << "Invalid\n\n";
        return;
      }
      std::string type = info.substr(1, split - 1);
      std::string index = info.substr(split + 1);
      if (index == "") {
        std::cout << "Invalid\n";
        return;
      }
      if (type == "ISBN") {
        if (valid_check(3, index) == false) {
          std::cout << "Invalid\n\n";
          return;
        }
        book_db.find(index, 0);
      } else if (type == "name") {
        if (valid_check(4, index) == false) {
          std::cout << "Invalid\n\n";
          return;
        }
        book_db.find(index, 1);
      } else if (type == "author") {
        if (valid_check(4, index) == false) {
          std::cout << "Invalid\n\n";
          return;
        }
        book_db.find(index, 2);
      } else if (type == "keyword") {
        if (valid_check(5, index) == false) {
          std::cout << "Invalid\n\n";
          return;
        }
        if (index.find('|') != -1) {
          std::cout << "Invalid\n\n";
          return;
        }
        book_db.find(index, 3);
      } else {
        std::cout << "Invalid\n\n";
      }
    }
  } else if (input == "buy") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 1) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string ISBN, quantity, left;
    stream >> ISBN >> quantity >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    long long quantity_int = -1;
    if (valid_check(3, ISBN) == false) {
      std::cout << "Invalid\n";
      return;
    }
    if (valid_check(6, quantity) == false) {
      std::cout << "Invalid\n";
      return;
    }
    try {
      quantity_int = std::stoll(quantity);
    } catch (std::invalid_argument) {
      std::cout << "Invalid\n";
      return;
    }
    if (quantity_int <= 0) {
      std::cout << "Invalid\n";
      return;
    }
    book_db.buy(ISBN, quantity_int, log_db, finance_log_db, faculty_log_db,
                users.back()->id, users.back()->rank);
  } else if (input == "select") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 3) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string ISBN, left;
    stream >> ISBN >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    if (valid_check(3, ISBN) == false) {
      std::cout << "Invalid\n";
      return;
    }
    if (ISBN == "") {
      std::cout << "Invalid\n";
      return;
    }
    if (book_db.return_index(ISBN) == -1) {
      book_db.new_book(ISBN);
    }
    users.back()->select = book_db.return_index(ISBN);
    std::cout << "\n";
  } else if (input == "modify") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 3 or users.back()->select == -1) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string info[5];
    std::string ISBN, name, author, keyword, price, left;
    stream >> info[0] >> info[1] >> info[2] >> info[3] >> info[4] >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    if (info[0] == "") {
      std::cout << "Invalid\n";
      return;
    }
    for (int i = 0; i < 5; i++) {
      if (info[i] == "") {
        break;
      }
      int split = -1;
      for (int j = 1; j < info[i].size(); j++) {
        if (info[i][j] == '=') {
          split = j;
          break;
        }
      }
      if (split == -1 or info[i][0] != '-') {
        std::cout << "Invalid\n";
        return;
      }
      std::string type = info[i].substr(1, split - 1);
      std::string index = info[i].substr(split + 1);
      if (index == "") {
        std::cout << "Invalid\n";
        return;
      }
      if (type == "ISBN" and ISBN == "") {
        ISBN = index;
        if (valid_check(3, ISBN) == false) {
          std::cout << "Invalid\n";
          return;
        }
      } else if (type == "name" and name == "") {
        name = index;
        if (valid_check(4, name) == false) {
          std::cout << "Invalid\n";
          return;
        }
      } else if (type == "author" and author == "") {
        author = index;
        if (valid_check(4, author) == false) {
          std::cout << "Invalid\n";
          return;
        }
      } else if (type == "keyword" and keyword == "") {
        keyword = index;
        if (valid_check(5, keyword) == false) {
          std::cout << "Invalid\n";
          return;
        }
      } else if (type == "price" and price == "") {
        price = index;
        if (valid_check(7, price) == false) {
          std::cout << "Invalid\n";
          return;
        }
      } else {
        std::cout << "Invalid\n";
        return;
      }
    }
    book_db.modify(users.back()->select, ISBN, name, author, keyword, price,
                   log_db, faculty_log_db, users.back()->id);
  } else if (input == "import") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 3 or users.back()->select == -1) {
      std::cout << "Invalid\n";
      return;
    }
    std::istringstream stream(input);
    std::string quantity, totalcost, left;
    stream >> quantity >> totalcost >> left;
    if (left != "") {
      std::cout << "Invalid\n";
      return;
    }
    if (quantity == "" or totalcost == "") {
      std::cout << "Invalid\n";
      return;
    }
    if (valid_check(6, quantity) == false or
        valid_check(7, totalcost) == false) {
      std::cout << "Invalid\n";
      return;
    }
    long long quantity_int = -1;
    long double totalcost_double = -1;
    try {
      quantity_int = std::stoll(quantity);
      totalcost_double = std::stold(totalcost);
    } catch (std::invalid_argument) {
      std::cout << "Invalid\n";
      return;
    }
    if (quantity_int <= 0 or totalcost_double <= 0) {
      std::cout << "Invalid\n";
      return;
    }
    book_db.import(users.back()->select, quantity_int, totalcost_double, log_db,
                   faculty_log_db, finance_log_db, users.back()->id);
  } else if (input == "report") {
    std::cin >> input;
    if (input == "finance") {
      getline(std::cin, input);
      if (users.empty() or users.back()->rank < 7) {
        std::cout << "Invalid\n\n";
        return;
      }
      std::istringstream stream(input);
      std::string left;
      stream >> left;
      if (left != "") {
        std::cout << "Invalid\n\n";
        return;
      }
      finance_log_db.read();
    } else if (input == "employee") {
      getline(std::cin, input);
      if (users.empty() or users.back()->rank < 7) {
        std::cout << "Invalid\n\n";
        return;
      }
      std::istringstream stream(input);
      std::string left;
      stream >> left;
      if (left != "") {
        std::cout << "Invalid\n\n";
        return;
      }
      faculty_log_db.read();
    }
  } else if (input == "log") {
    getline(std::cin, input);
    if (users.empty() or users.back()->rank < 7) {
      std::cout << "Invalid\n\n";
      return;
    }
    std::istringstream stream(input);
    std::string left;
    stream >> left;
    if (left != "") {
      std::cout << "Invalid\n\n";
      return;
    }
    log_db.read();
  } else {
    std::cout << "Invalid\n\n";
    getline(std::cin, input);
  }
}

int main() {
  std::string input;
  while (!std::cin.eof()) {
    if (!(std::cin >> input)) {
      break;
    }
    handleInput(input);
  }
  return 0;
}
