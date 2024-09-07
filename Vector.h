#include <iostream>
#include <cassert>
#include <initializer_list>
#include <iterator>

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

    // Capacity
    bool empty() const;
    size_t size() const;
    size_t max_size() const;
    void reserve(size_t sz);
    size_t capacity() const;
    void shrink_to_fit();

    // Modifiers
    void clear();
    void insert(size_t pos, const T& element); // NR
    void erase(); // NR

    template<class... Args>
    void emplace_back( Args&&... args );

    void pop_back();  // update
    void resize(size_t new_size);
    void resize(size_t new_size, const T& value);

    // Other
    void print() {
        for (size_t i = 0; i < size_; ++i) {
            std::cout << container[i] << " ";
        }
    }
    bool operator==( const Vector<T>& rhs);
    bool operator==( Vector<T>&& rhs);

private:
    size_t size_;
    size_t capacity_;
    T* container = nullptr;
    static constexpr size_t single_object_size = sizeof(T);

    void rellocate();
    void clean(size_t index);
};

