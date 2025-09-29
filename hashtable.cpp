#include <iostream>
#include <string>
#include <vector>

// Собака
struct Dog {
    std::string name;  
    int weight;        

    Dog(const std::string& n = "", int w = 0) {
        name = n;
        weight = w;
    }
};

// Именнованные константы для состояний слота в хэш-таблице
enum SlotState {
    EMPTY = 0,
    OCCUPIED = 1,
    DELETED = 2
};

// Слот для открытой адресации
struct Slot {
    Dog dog;
    SlotState state; 

    Slot() {
        state = EMPTY;
    }
};

// Хэш-таблица с открытой адресацией и квадратичным пробированием
class HashTable {
private:
    std::vector<Slot> table;  
    size_t capacity;          // Текущая ёмкость
    size_t numElements;       // Текущее количество элементов

    // Хэш-функция
    size_t hash(const std::string& key) const {
        size_t h = 0;
        const int base = 17;
        for (char c : key) {
            h = h * base + static_cast<size_t>(c);
        }
        return h % capacity;
    }

    // Квадратичное пробирование(h + i^2)
    size_t probe(size_t hashValue, size_t i) const {
        return (hashValue + i * i) % capacity;
    }

    // Рехэширование
    void rehash() {
        size_t oldCapacity = capacity;
        std::vector<Slot> oldTable = table;

        capacity *= 2;
        table.assign(capacity, Slot());
        numElements = 0;

        for (size_t i = 0; i < oldCapacity; ++i) {
            if (oldTable[i].state == OCCUPIED) {
                insert(oldTable[i].dog.name, oldTable[i].dog.weight);
            }
        }
    }

public:
    // Конструктор
    HashTable(size_t initialCapacity = 10) {
        capacity = initialCapacity;
        numElements = 0;
        table.assign(capacity, Slot());
    }

    // Деструктор
    ~HashTable() {
        clear();
    }

    // Вставка элемента с квадратичным пробированием
    void insert(const std::string& key, const int& value) {
        if (numElements >= capacity * 0.75) {
            rehash();
        }

        size_t hashValue = hash(key);
        size_t i = 0;  // Счетчик проб
        while (true) {
            size_t id = probe(hashValue, i);
            if (table[id].state == EMPTY || table[id].state == DELETED) {
                table[id].dog = Dog(key, value);
                table[id].state = OCCUPIED;
                ++numElements;
                return;
            }
            if (table[id].state == OCCUPIED && table[id].dog.name == key) {
                table[id].dog.weight = value;
                return;
            }
            ++i;
            if (i >= capacity) return; // Обработка бесконечного цикла
        }
    }

    // Поиск(возвращаем true, если найден, и заполняем value)
    bool find(const std::string& key, int& value) const {
        size_t hashValue = hash(key);
        size_t i = 0;  // Счетчик проб
        while (true) {
            size_t id = probe(hashValue, i);
            if (table[id].state == EMPTY) return false;
            if (table[id].state == OCCUPIED && table[id].dog.name == key) {
                value = table[id].dog.weight;
                return true;
            }
            ++i;
            if (i >= capacity) return false;  // Не нашли
        }
    }

    // Удаление(помечаем как DELETED)
    bool remove(const std::string& key) {
        size_t hashValue = hash(key);
        size_t i = 0;  // Счетчик проб
        while (true) {
            size_t id = probe(hashValue, i);
            if (table[id].state == EMPTY) return false;
            if (table[id].state == OCCUPIED && table[id].dog.name == key) {
                table[id].state = DELETED;
                --numElements;
                return true;
            }
            ++i;
            if (i >= capacity) return false;  // Не нашли
        }
    }

    // Вывод в консоль
    void print() const {
        for (size_t id = 0; id < capacity; ++id) {
            std::cout << "Slot " << id << ": ";
            if (table[id].state == OCCUPIED) {
                std::cout << "[" << table[id].dog.name << ", " << table[id].dog.weight << "]";
            }
            else if (table[id].state == DELETED) {
                std::cout << "[DELETED]";
            }
            else {
                std::cout << "[EMPTY]";
            }
            std::cout << std::endl;
        }
    }

    // Очистка таблицы
    void clear() {
        table.assign(capacity, Slot());
        numElements = 0;
    }

    // Количество элементов
    size_t size() const {
        return numElements;
    }

    // Проверка на пустоту
    bool isEmpty() const {
        return numElements == 0;
    }
};

// Демо
int main() {
    HashTable ht;

    ht.insert("Dars", 25);
    ht.insert("Gretta", 10);
    ht.insert("Belka", 5);
    ht.insert("Strelka", 6);
    ht.insert("Gaston", 1);

    // Вывод таблицы
    std::cout << "Hash table after adding elements:" << std::endl;
    ht.print();
    std::cout << "Number of items in the table: " << ht.size() << ", Is Empty: " << (ht.isEmpty() ? "Yes" : "No") << std::endl;

    // Поиск элемента
    int weight;
    if (ht.find("Dars", weight)) {
        std::cout << "Found a Dars with a weight of: " << weight << std::endl;
    }
    else {
        std::cout << "Dars was not found" << std::endl;
    }

    // Удаляем элемент
    if (ht.remove("Gretta")) {
        std::cout << "Gretta has been deleted" << std::endl;
    }
    else {
        std::cout << "Gretta was not found to be deleted" << std::endl;
    }

    // Вывод таблицы после удаления
    std::cout << "Hash table after deleting elements:" << std::endl;
    ht.print();
    std::cout << "Number of items in the table: " << ht.size() << ", Is Empty: " << (ht.isEmpty() ? "Yes" : "No") << std::endl;

    // Очистка таблицы
    ht.clear();
    std::cout << "Hash table after cleaning:" << std::endl;
    ht.print();
    std::cout << "Number of items in the table: " << ht.size() << ", Is Empty: " << (ht.isEmpty() ? "Yes" : "No") << std::endl;

    return 0;
}