#include <iostream>
#include <vector>
#include <ranges>
#include <coroutine>
#include <atomic>
#include <chrono>
#include <cstring>

template <typename T>
concept ElementType = std::is_trivial_v<T>;

template <ElementType T>
class FastAccessFreezeArray {
public:
    FastAccessFreezeArray(size_t capacity)
        : capacity_(capacity), data_(new T[capacity]), rows_(capacity / rowSize_), cols_(rowSize_) {}

    ~FastAccessFreezeArray() {
        delete[] data_;
    }

    void addElement(T element) {
        if (count_ < capacity_) {
            size_t row = count_ / rowSize_;
            size_t col = count_ % rowSize_;
            data_[row * cols_ + col] = element;
            ++count_;
        }
    }

    // Operator to access elements by index.
    T& operator[](size_t index) {
        size_t row = index / rowSize_;
        size_t col = index % rowSize_;
        return data_[row * cols_ + col];
    }

    // Function to process elements using ranges.
    void processElements() {
        for (auto element : std::ranges::subrange(data_, data_ + count_)) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }

    // Simulated shrink to fit (release unused memory).
    void shrinkToFit() {
        if (count_ < capacity_) {
            T* newData = new T[count_];
            std::memcpy(newData, data_, count_ * sizeof(T));

            delete[] data_;
            data_ = newData;

            capacity_ = count_;
            rows_ = count_ / rowSize_;
            cols_ = rowSize_;
        }
    }

private:
    const size_t rowSize_ = 1024;
    size_t rows_;
    size_t cols_;
    size_t capacity_;
    T* data_;
    size_t count_ = 0;
};

int main() {
    {
        FastAccessFreezeArray<int> array(10);

        for (int i = 0; i < 10; ++i) {
            array.addElement(i);
        }

        for (size_t i = 0; i < 10; ++i) {
            std::cout << array[i] << " ";
        }
        std::cout << std::endl;

        array.processElements();
    }

    const int elementsCount = 1000000;

    auto start = std::chrono::high_resolution_clock::now();

    {
        FastAccessFreezeArray<int> array(elementsCount);

        for (int i = 0; i < elementsCount; ++i) {
            array.addElement(i);
        }

        array.shrinkToFit();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution time for FastAccessFreezeArray<int>: " << duration.count() << " seconds" << std::endl;

    size_t memoryUsage = elementsCount * sizeof(int) + sizeof(FastAccessFreezeArray<int>);
    std::cout << "Approximate memory usage for FastAccessFreezeArray<int>: " << memoryUsage << " bytes" << std::endl;

    {
        std::vector<int> vec;

        auto vec_start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < elementsCount; ++i) {
            vec.push_back(i);
        }

        vec.shrink_to_fit();

        auto vec_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> vec_duration = vec_end - vec_start;

        size_t vec_memoryUsage = elementsCount * sizeof(int) + sizeof(std::vector<int>);

        std::cout << "\nComparison with std::vector<int>:" << std::endl;
        std::cout << "Execution time for std::vector<int>: " << vec_duration.count() << " seconds" << std::endl;
        std::cout << "Approximate memory usage for std::vector<int>: " << vec_memoryUsage << " bytes" << std::endl;
    }

    return 0;
}
