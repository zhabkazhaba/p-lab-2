#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <random>
#include <map>
#include "resident.hpp"
#include "linear.hpp"
#include "binary_tree.hpp"
#include "rb_tree.hpp"
#include "hash_table.hpp"
#include "util.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    #ifdef _WIN32
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    #endif

    std::string in_file = "../data/in_data.csv";
    std::string times_file = "../data/out_times.csv";

    std::cout << "Читаем данные из " << in_file << "\n";
    std::vector<Resident> original_data = util::read_csv(in_file);
    std::cout << "Успешно прочитано " << original_data.size() << "\n";

    if (original_data.empty()) {
        std::cerr << "Датасет пуст\n";
        return 1;
    }

    std::vector<int> sizes = {100, 500, 1000, 2500, 5000, 10000, 25000, 50000, 75000, 100000, 250000, 500000, 1000000};
    std::vector<util::TimeLog> time_results;

    std::ofstream t_file(times_file);
    if (!t_file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи времени\n";
        return 1;
    }

    t_file << "Size;Linear;BT;RBT;Hash;MultiMap;HashCollisions\n";

    std::mt19937 rng(111);

for (int size : sizes) {
        if (size > original_data.size()) {
            continue;
        }

        std::cout << "Размер: " << size << "\n";

        // Подготовка данных
        std::vector<Resident> slice(original_data.begin(), original_data.begin() + size);
        
        std::vector<Resident> sorted_slice = slice;
        std::sort(sorted_slice.begin(), sorted_slice.end());

        // Выбор улиц
        std::vector<std::string> search_keys;
        std::uniform_int_distribution<int> dist(0, size - 1);
        for (int i = 0; i < 100; ++i) {
            search_keys.push_back(slice[dist(rng)].street);
        }

        // Создание нужных структур
        // Бинарное дерево поиска
        btree::tnode* bt_root = btree::buildTree(sorted_slice, 0, size - 1);

        // Красно-черное дерево
        rbtree::RBTree rb_tree;
        for (const auto& res : slice) {
            rb_tree.RBInsert(res);
        }

        // Хэш-таблица
        unsigned long buckets = util::get_closest_prime(size);
        htable::HashTable hash_table(buckets);
        for (const auto& res : slice) {
            hash_table.insert(res);
        }

        // std::multimap
        std::multimap<std::string, Resident> mmap;
        for (const auto& res : slice) {
            mmap.insert({res.street, res});
        }

        // Замеряем время
        long long time_lin = 0, time_bt = 0, time_rbt = 0, time_hash = 0, time_map = 0;

        auto start = std::chrono::high_resolution_clock::now();
        for (const auto& key : search_keys) {
            auto res = linear_search(slice, key); 
        }
        auto end = std::chrono::high_resolution_clock::now();
        time_lin = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (const auto& key : search_keys) {
            std::vector<Resident> res;
            btree::search(bt_root, key, res); 
        }
        end = std::chrono::high_resolution_clock::now();
        time_bt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (const auto& key : search_keys) {
            auto res = rb_tree.search(key);
        }
        end = std::chrono::high_resolution_clock::now();
        time_rbt = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (const auto& key : search_keys) {
            auto res = hash_table.search(key);
        }
        end = std::chrono::high_resolution_clock::now();
        time_hash = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        for (const auto& key : search_keys) {
            auto range = mmap.equal_range(key);
            std::vector<Resident> res;
            for (auto it = range.first; it != range.second; ++it) {
                res.push_back(it->second);
            }
        }
        end = std::chrono::high_resolution_clock::now();
        time_map = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        int colls = hash_table.getCollisions();
        
        time_results.push_back({size, time_lin, time_bt, time_rbt, time_hash, time_map, colls});
    }

    util::write_times_csv(times_file, time_results);

    std::cout << "Конец замеров\n";
    return 0;
}
