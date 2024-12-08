#include <iostream>
#include <memory>
#include <chrono>

template <typename T>
class Tree {
    struct Node {
        T value;
        std::shared_ptr<Node> left, right;

        Node(T val) : value(val), left(nullptr), right(nullptr) {}

        Node(T val, std::shared_ptr<Node> l, std::shared_ptr<Node> r)
            : value(val), left(std::move(l)), right(std::move(r)) {}
    };

    std::shared_ptr<Node> root;

    size_t calculateMemoryUsage(std::shared_ptr<Node> node) const {
        if (!node) return 0;

        size_t nodeSize = sizeof(Node);
        size_t sharedPtrOverhead = 2 * sizeof(std::shared_ptr<Node>);
        size_t valueSize = sizeof(T);

        return nodeSize + sharedPtrOverhead + valueSize +
               calculateMemoryUsage(node->left) +
               calculateMemoryUsage(node->right);
    }

    bool lookupHelper(std::shared_ptr<Node> node, T value) const {
        if (!node) return false;
        if (value == node->value) return true;
        if (value < node->value) return lookupHelper(node->left, value);
        return lookupHelper(node->right, value);
    }

public:
    Tree() : root(nullptr) {}

    Tree insert(T value) const {
        if (!root) return Tree(std::make_shared<Node>(value));

        std::shared_ptr<Node> current = root;
        std::shared_ptr<Node> newRoot = root;

        while (current) {
            if (value < current->value) {
                if (!current->left) {
                    current->left = std::make_shared<Node>(value);
                    break;
                }
                current = current->left;
            } else if (value > current->value) {
                if (!current->right) {
                    current->right = std::make_shared<Node>(value);
                    break;
                }
                current = current->right;
            } else {
                return *this;
            }
        }

        return Tree(newRoot);
    }

    bool lookup(T value) const {
        return lookupHelper(root, value);
    }

    size_t getMemoryUsage() const {
        return calculateMemoryUsage(root);
    }

private:
    Tree(std::shared_ptr<Node> node) : root(node) {}
};

template <typename Func>
void measureTime(const std::string& operation, Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << operation << " took " << duration.count() << " ms" << std::endl;
}

void testTreeOperations(int numElements) {
    std::cout << "\nTesting with " << numElements << " elements:" << std::endl;

    Tree<int> tree;

    measureTime("Tree insertion", [&]() {
        for (int i = 1; i <= numElements; ++i) {
            tree = tree.insert(i);
        }
    });

    size_t memoryUsage = tree.getMemoryUsage();
    std::cout << "Memory usage after inserting " << numElements << " elements: " << memoryUsage << " bytes" << std::endl;

    measureTime("Tree lookup", [&]() {
        for (int i = 1; i <= numElements; ++i) {
            tree.lookup(i);
        }
    });
}

int main() {
    testTreeOperations(1000);
    testTreeOperations(2000);
    testTreeOperations(3000);

    return 0;
}
