#include <iostream>
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <vector>
#include <typeinfo>
#include <cstdint>
#include <memory>

template <typename T>
class Vector {
public:
    Vector<T>();
    Vector<T>(size_t range);
    Vector<T>(std::initializer_list<T> initialization);
    Vector<T>(const Vector<T>& other);
    Vector<T>(Vector<T>&& other);

    // Дополнительный конструктор от вектора
    Vector<T>(const std::vector<T>& other);
    Vector<T>(std::vector<T>&& other);

    ~Vector<T>();

    void push_back(const T& element);
    void push_back(T&& element);

    Vector<T>& operator=(const Vector<T>& other);
    Vector<T>& operator=(Vector<T>&& other);

    void assign(size_t count, const T& value);
    void assign(std::initializer_list<T> inizialization);

    // Acces to the elements of container type <T>
    T& at(size_t index) const;
    T& operator[](size_t n) const;
    T& front() const;
    T& back() const;
    T* data() const;

    // Iterators
    struct Iterator {
        Iterator(T* pointer);
        Iterator& operator=(const Iterator& other);
        
        T& operator*();
        Iterator operator+(size_t);
        Iterator operator++(int);
        Iterator& operator++();

        bool operator==(const Iterator& other);
        bool operator!=(const Iterator& other);

    private:
        T* pointer_;
    };
    
    struct ReverseIterator {
        ReverseIterator(T* pointer);
        ReverseIterator& operator=(const ReverseIterator& other);

        T& operator*();
        ReverseIterator operator+(size_t);
        ReverseIterator operator++(int);
        ReverseIterator& operator++();

        bool operator==(const ReverseIterator& other);
        bool operator!=(const ReverseIterator& other);
    private:
        T* pointer_;
    };

    Iterator begin() const;
    Iterator end() const;
    ReverseIterator rbegin() const;
    ReverseIterator rend() const;

    // Capacity
    bool empty() const;
    size_t size() const;
    size_t max_size() const;
    void reserve(size_t sz);
    size_t capacity() const;
    void shrink_to_fit();

    // Modifiers
    void clear();
    void insert(Iterator pos, const T& value);
    void insert(Iterator pos, T&& value);
    void insert(Iterator pos, std::initializer_list<T> initializer);

    template< class... Args >
    void emplace( Iterator pos, Args&&... args);

    template< class... Args>
    void emplace_back(Args&&... args);

    void erase(Iterator pos);
    void erase(Iterator start, Iterator finish);
    void pop_back();
    void resize(size_t new_size);
    void resize(size_t new_size, const T& value);

    // Other
    void print() {
        for (size_t i = 0; i < size_; ++i) {
            std::cout << container[i] << " ";
        }
        std::cout << "\n";
    }
    void swap(Vector<T>& other);
private:
    size_t size_;
    size_t capacity_;
    T* container = nullptr;
    static constexpr size_t single_object_size = sizeof(T);

    void rellocate();
    void reduce();
    void clean(size_t index);
};
