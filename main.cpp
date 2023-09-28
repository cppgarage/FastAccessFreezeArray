#include <iostream>
#include <vector>
#include <ranges>
#include <coroutine>
#include <atomic>
#include <chrono>

// Simplified concept for demonstration.
template <typename T>
concept ElementType = std::is_trivial_v<T>;

// Fast-access and freeze array.
template <ElementType T>
class FastAccessFreezeArray {
public:
    FastAccessFreezeArray(size_t capacity)
        : capacity_(capacity), data_(new T[capacity]), count_(0) {}

    ~FastAccessFreezeArray() {
        delete[] data_;
    }

    // Add an element to the array.
    void addElement(T element) {
        if (count_ < capacity_)
            data_[count_++] = element;
    }

    // Operator to access elements by index.
    T& operator[](size_t index) {
        return data_[index];
    }

    // Function to process elements using ranges.
void processElements() {
        for (auto element : std::ranges::subrange(data_, data_ + count_)) {
            // Process each element.
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

private:
    size_t capacity_;
    T* data_;
    size_t count_;
};

int main() {
    {
        FastAccessFreezeArray<int> array(10);

        // Add elements
        for (int i = 0; i < 10; ++i) {
            array.addElement(i);
        }

        for (size_t i = 0; i < 10; ++i) {
            std::cout << array[i] << " ";
        }
        std::cout << std::endl;

        // Process elements using ranges.
        array.processElements();
    } // Array goes out of scope and is destroyed here.

    const int elementsCount = 1000000;

    auto start = std::chrono::high_resolution_clock::now();

    {
        FastAccessFreezeArray<int> array(elementsCount);

        for (int i = 0; i < elementsCount; ++i) {
            array.addElement(i);
        }
    } // Array goes out of scope and is destroyed here.

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution time for FastAccessFreezeArray<int>: " << duration.count() << " seconds" << std::endl;

    // Approximate memory usage for FastAccessFreezeArray in bytes.
    size_t memoryUsage = elementsCount * sizeof(int) + sizeof(FastAccessFreezeArray<int>);
    std::cout << "Approximate memory usage for FastAccessFreezeArray<int>: " << memoryUsage << " bytes" << std::endl;

    // Comparison with std::vector<int>.
    {
        std::vector<int> vec;

        auto vec_start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < elementsCount; ++i) {
            vec.push_back(i);
        }

        auto vec_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> vec_duration = vec_end - vec_start;

        // Approximate memory usage for vector in bytes.
        size_t vec_memoryUsage = elementsCount * sizeof(int) + sizeof(std::vector<int>);

        std::cout << "\nComparison with std::vector<int>:" << std::endl;
        std::cout << "Execution time for std::vector<int>: " << vec_duration.count() << " seconds" << std::endl;
        std::cout << "Approximate memory usage for std::vector<int>: " << vec_memoryUsage << " bytes" << std::endl;
    }

    return 0;
}
