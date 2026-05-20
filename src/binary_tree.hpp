#pragma once
#include <vector>
#include <string>
#include "resident.hpp"

namespace btree {
    struct tnode {
        Resident data;
        tnode* left = nullptr;
        tnode* right = nullptr;

        tnode(Resident res) : data(res) {}
    };

    /**
     * @brief Построение бинарного дерева
     */
    tnode* buildTree(const std::vector<Resident>& input, int start, int end) {
        if (start > end) {
            return nullptr;
        }

        int mid = start + (end - start) / 2;
        tnode* root = new tnode(input[mid]);

        root->left = buildTree(input, start, mid - 1);
        root->right = buildTree(input, mid + 1, end);

        return root;
    }

    /**
     * @brief Поиск по бинарному дереву
     */
    void search(btree::tnode* root, const std::string& key, std::vector<Resident>& output) {
        if (root == nullptr) {
            return;
        }

        if (root->data.street == key) {
            output.push_back(root->data);
            search(root->left, key, output);
            search(root->right, key, output);
        } else if (key < root->data.street) {
            search(root->left, key, output);
        } else {
            search(root->right, key, output);
        }
    }
}
