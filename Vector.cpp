#include "Vector.h"
#include <vector>
#include <typeinfo>

template <typename T>
void Vector<T>::clean(size_t index) {
    for (size_t i = 0; i < index; ++i) {
        container[i].~T();
    }
}
template <typename T>
Vector<T>::Vector() {
    size_ = 0;
    capacity_ = 0;
    container = *(new T*[0]);
}

template <typename T>
Vector<T>::Vector(size_t range) {
    // Необходимо проверить коррекность
    size_ = range;
    capacity_ = 2 * size_;
    container = reinterpret_cast<T*>(operator new[](std::max<size_t>(capacity_, 1) * single_object_size));
    for (size_t i = 0; i < size_; ++i) {
        try {
            container[i] = T();
        } catch (...) {
            std::cout << "Default constructor failed\n";
            clean(i);
            std::exit(0);
        }
    }
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> initialization) {
    size_ = initialization.size();
    capacity_ = 2 * size_;
    container = reinterpret_cast<T*>(operator new[](size_ * single_object_size));
    auto it = initialization.begin();
    size_t i = 0;
    while (it != initialization.end()) {
        container[i] = *it;
        ++it;
        ++i;
    }
}

template <typename T>
Vector<T>::Vector(const Vector<T>& other) {
    capacity_ = other.capacity_;
    container = other.container;
    size_ = other.size_;
}

template <typename T>
Vector<T>::Vector(Vector<T>&& other) {
    std::swap(container, other.container);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template <typename T>
Vector<T>::Vector(const std::vector<T>& other) {
    size_ = other.size();
    capacity_ = other.capacity();
    *this = std::move(Vector<T>(size_));
    for (size_t i = 0; i < size_; ++i) {
        container[i] = other[i];
    }
}

template <typename T>
Vector<T>::Vector(std::vector<T>&& other) {
    size_ = other.size();
    capacity_ = other.capacity();
    Vector<T>&& ptr = Vector<T>(size_);
    *this = std::move(ptr);
    for (size_t i = 0; i < size_; ++i) {
        container[i] = std::move(other[i]);
    }
}

template<typename T>
Vector<T>::~Vector<T>() {
    for (size_t i = 0; i < size_; ++i) {
        container[i].~T();
    }
    delete[] container;
}

template <typename T>
void Vector<T>::push_back(const T& element) {
    if (size_ == capacity_) {
        rellocate();
    }
    container[size_++] = element;
}

template <typename T>
void Vector<T>::push_back(T&& element) {
    if (size_ == capacity_) {
        rellocate();
    }
    container[size_++] = std::move(element);
}

template <typename T>
void Vector<T>::rellocate() {
    void* memory = operator new[](std::max<size_t>(2 * capacity_, 1) * single_object_size);
    T* new_container = reinterpret_cast<T*>(memory);
    memory = nullptr;

    for (size_t i = 0; i < capacity_; ++i) {
        // copy constructor???
        new_container[i] = std::move(container[i]);
        container[i].~T();
        //new (new_container + i) T (container[i]);
    }
    delete[] container;
    container = new_container;
    capacity_ = std::max<size_t>(2 * capacity_, 1);
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
    T* new_container = reinterpret_cast<T*>(operator new[](other.capacity_ * single_object_size));
    clean(size_);
    delete[] container;
    container = new_container;
    for (size_t i = 0; i < other.size_; ++i) {
        container[i] = other[i];
    }
    capacity_ = other.capacity_;
    size_ = other.size_;
    return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& other) {
    std::swap(container, other.container);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    return *this;
}

template <typename T>
void Vector<T>::assign(size_t count, const T& value) {
    clear();
    this = Vector<T>(count);
    for (size_t i = 0; i < size_; ++i) {
        container[i] = value;
    }
}

template <typename T>
void Vector<T>::assign(std::initializer_list<T> inizialization) {
    clear();
    this = std::move(T(inizialization));
}
// Acces to the elements of container type <T>
template <typename T>
T& Vector<T>::at(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index is out of range");
    }
    return container[index];
}

template <typename T>
T& Vector<T>::operator[](size_t index) const {
    return container[index];
}

template <typename T>
T& Vector<T>::front() const {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    return container[0];
}

template <typename T>
T& Vector<T>::back() const {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    return container[size_ - 1];
}

template <typename T>
T* Vector<T>::data() const {
    return container;
}

// Capacity
template <typename T>
bool Vector<T>::empty() const {
    return (size_ == 0);
}

template <typename T>
size_t Vector<T>::size() const {
    return size_;
}

template <typename T>
size_t Vector<T>::max_size() const {
    return 0;
}

template <typename T>
size_t Vector<T>::capacity() const {
    return capacity_;
}

// Modifiers
template <typename T>
void Vector<T>::clear() {
    clean(size_);
    delete[] container;
    container = nullptr;
    size_ = 0;
    capacity_ = 0;
    //*this = Vector<T>();    
}
template <typename T>
template <class... Args> 
void Vector<T>::emplace_back( Args&&... args ) {
    push_back(T(std::forward<Args>(args) ...));
    //push_back(std::move(T(std::forward<Args>(args))));
    //(T<Args>::run(std::forward<Args>(args)), 0)
}

template <typename T>
void Vector<T>::pop_back() {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    // last element = size_ - 1
    container[--size_].~T();
}

template <typename T>
void Vector<T>::resize(size_t new_size) {
    if (new_size == size_) {
        return;
    }
    void* memory = operator new[](new_size * single_object_size);
    T* new_container = reinterpret_cast<T*>(memory);

    for (size_t i = 0; i < new_size; ++i) {
        if (i < size_) {
            new_container[i] = container[i];
        } else {
            new_container[i] = T(); // Должно быть
        }
    }
    size_ = new_size;
    capacity_ = size_ * 2;
    delete[] container;
    container = new_container;
}

template <typename T>
void Vector<T>::resize(size_t new_size, const T& value) {
    if (new_size == size_) {
        return;
    }
    void* memory = operator new[](new_size * single_object_size);
    T* new_container = reinterpret_cast<T*>(memory);

    for (size_t i = 0; i < new_size; ++i) {
        if (i < size_) {
            new_container[i] = container[i];
        } else {
            new_container[i] = value; // Должно быть
        }
    }
    size_ = new_size;
    capacity_ = size_ * 2;
    delete[] container;
    container = new_container;
}

template <typename T>
bool Vector<T>::operator==(const Vector<T>& rhs) {
    if (rhs.size_ != size_) {
        return false;
    }

    for (size_t i = 0; i < size_; ++i) {
        if (rhs[i] != at(i)) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool Vector<T>::operator==( Vector<T>&& rhs) {
    if (rhs.size_ != size_) {
        return false;
    }

    for (size_t i = 0; i < size_; ++i) {
        if (rhs[i] != this[i]) {
            return false;
        }
    }
    return true;
}
