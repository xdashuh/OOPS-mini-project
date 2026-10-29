#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <utility>

// Library Management System: demonstrates encapsulation, inheritance,
// abstraction, runtime polymorphism, composition, and file handling.
class LibraryItem {
private:
    int id_;
    std::string title_;
    bool available_;
public:
    LibraryItem(int id, std::string title, bool available = true)
        : id_(id), title_(std::move(title)), available_(available) {}
    virtual ~LibraryItem() = default;
    int id() const { return id_; }
    const std::string& title() const { return title_; }
    bool available() const { return available_; }
    void setAvailable(bool value) { available_ = value; }
    virtual std::string type() const = 0; // abstract interface
    virtual std::string extra() const = 0;
    virtual std::string serialize() const = 0;
    virtual void display() const {
        std::cout << std::left << std::setw(6) << id_ << std::setw(10) << type()
                  << std::setw(32) << title_ << (available_ ? "Available" : "Issued")
                  << "  " << extra() << '\n';
    }
};

class Book : public LibraryItem {
    std::string author_;
public:
    Book(int id, std::string title, std::string author, bool available = true)
        : LibraryItem(id, std::move(title), available), author_(std::move(author)) {}
    std::string type() const override { return "Book"; }
    std::string extra() const override { return "Author: " + author_; }
    std::string serialize() const override {
        return "B|" + std::to_string(id()) + "|" + title() + "|" + author_ + "|" + (available() ? "1" : "0");
    }
};

class Magazine : public LibraryItem {
    int issue_;
public:
    Magazine(int id, std::string title, int issue, bool available = true)
        : LibraryItem(id, std::move(title), available), issue_(issue) {}
    std::string type() const override { return "Magazine"; }
    std::string extra() const override { return "Issue: " + std::to_string(issue_); }
    std::string serialize() const override {
        return "M|" + std::to_string(id()) + "|" + title() + "|" + std::to_string(issue_) + "|" + (available() ? "1" : "0");
    }
};

class Library {
    std::vector<std::unique_ptr<LibraryItem>> items_; // composition
    const std::string dataFile_ = "library_data.txt";

    LibraryItem* find(int id) const {
        for (const auto& item : items_) if (item->id() == id) return item.get();
        return nullptr;
    }
    static std::vector<std::string> split(const std::string& line) {
        std::vector<std::string> fields; std::stringstream ss(line); std::string part;
        while (std::getline(ss, part, '|')) fields.push_back(part);
        return fields;
    }
public:
    Library() { load(); }
    void load() {
        std::ifstream in(dataFile_); if (!in) return;
        std::string line;
        while (std::getline(in, line)) {
            try {
                auto f = split(line); if (f.size() != 5) continue;
                int id = std::stoi(f[1]); bool available = (f[4] == "1");
                if (f[0] == "B") items_.push_back(std::make_unique<Book>(id, f[2], f[3], available));
                else if (f[0] == "M") items_.push_back(std::make_unique<Magazine>(id, f[2], std::stoi(f[3]), available));
            } catch (...) { /* Ignore malformed records and continue loading. */ }
        }
    }
    void save() const {
        std::ofstream out(dataFile_);
        if (!out) throw std::runtime_error("Could not write library_data.txt");
        for (const auto& item : items_) out << item->serialize() << '\n';
    }
    void addBook(int id, const std::string& title, const std::string& author) {
        if (id <= 0 || title.empty() || author.empty()) throw std::invalid_argument("ID, title and author are required; ID must be positive.");
        if (find(id)) throw std::invalid_argument("An item with that ID already exists.");
        items_.push_back(std::make_unique<Book>(id, title, author)); save();
    }
    void addMagazine(int id, const std::string& title, int issue) {
        if (id <= 0 || title.empty() || issue <= 0) throw std::invalid_argument("Positive ID and issue number and a title are required.");
        if (find(id)) throw std::invalid_argument("An item with that ID already exists.");
        items_.push_back(std::make_unique<Magazine>(id, title, issue)); save();
    }
    void list() const {
        if (items_.empty()) { std::cout << "No items in the library yet.\n"; return; }
        std::cout << std::left << std::setw(6) << "ID" << std::setw(10) << "Type" << std::setw(32) << "Title" << "Status      Details\n";
        std::cout << std::string(80, '-') << '\n';
        for (const auto& item : items_) item->display(); // virtual call
    }
    void issue(int id) {
        auto* item = find(id); if (!item) throw std::invalid_argument("Item ID not found.");
        if (!item->available()) throw std::invalid_argument("That item is already issued.");
        item->setAvailable(false); save(); std::cout << "Issued: " << item->title() << '\n';
    }
    void returnItem(int id) {
        auto* item = find(id); if (!item) throw std::invalid_argument("Item ID not found.");
        if (item->available()) throw std::invalid_argument("That item is already available.");
        item->setAvailable(true); save(); std::cout << "Returned: " << item->title() << '\n';
    }
};

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) { std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); return value; }
        std::cout << "Please enter a whole number.\n";
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
std::string readText(const std::string& prompt) {
    std::string text; std::cout << prompt; std::getline(std::cin, text); return text;
}

int main() {
    Library library;
    std::cout << "=== CAMPUS LIBRARY MANAGEMENT SYSTEM ===\n";
    while (true) {
        std::cout << "\n1. List items\n2. Add book\n3. Add magazine\n4. Issue item\n5. Return item\n0. Save and exit\n";
        int choice = readInt("Choose: ");
        try {
            if (choice == 0) { library.save(); std::cout << "Data saved. Goodbye!\n"; break; }
            else if (choice == 1) library.list();
            else if (choice == 2) {
                int id = readInt("Book ID: ");
                std::string title = readText("Title: ");
                std::string author = readText("Author: ");
                library.addBook(id, title, author);
            } else if (choice == 3) {
                int id = readInt("Magazine ID: ");
                std::string title = readText("Title: ");
                int issue = readInt("Issue number: ");
                library.addMagazine(id, title, issue);
            }
            else if (choice == 4) library.issue(readInt("Item ID to issue: "));
            else if (choice == 5) library.returnItem(readInt("Item ID to return: "));
            else std::cout << "Choose one of the listed options.\n";
        } catch (const std::exception& e) { std::cout << "Error: " << e.what() << '\n'; }
    }
    return 0;
}
