#include "Vector.h"

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
    size_ = range;
    capacity_ = 2 * size_;
    container = reinterpret_cast<T*>(operator new[](std::max<size_t>(capacity_, 1) * single_object_size));
    for (size_t i = 0; i < size_; ++i) {
        try {
            container[i] = T();
        } catch (...) {
            std::cout << "Default constructor failed\n";
            clean(i);
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
    operator delete(container);
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
        try {
            if (std::is_move_constructible<T>::value) {
                new_container[i] = std::move(container[i]);
            } else if (std::is_copy_constructible<T>::value) {
                new_container[i] = container[i];
                //container[i].~T();
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                new_container[i].~T();
            }
            return;
        };
        //new (new_container + i) T (container[i]);
    }
    clean(size_); // size_ == capacity_
    operator delete(container);
    container = new_container;
    capacity_ = std::max<size_t>(2 * capacity_, 1);
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& other) {
    T* new_container = reinterpret_cast<T*>(operator new[](other.capacity_ * single_object_size));
    clean(size_);
    operator delete(container);
    container = new_container;
    for (size_t i = 0; i < other.size_; ++i) {
        try {
            if (std::is_copy_assignable<T>::value) {
                container[i] = other[i];
            }
        } catch (...){};
        //container[i] = other[i];
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
    *this = Vector<T>(count);
    for (size_t i = 0; i < size_; ++i) {
        try {
            if (std::is_copy_assignable<T>::value) { 
                container[i] = value;
            }
        } catch (...) {
            clean(i);
        };
    }
}

// ПРОВЕРИТЬ КОРРЕКНОСТЬ
template <typename T>
void Vector<T>::assign(std::initializer_list<T> inizialization) {
    clear();
    for (auto it : inizialization) {
        push_back(std::move(it));
    }
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

// Iterator
template <typename T>
Vector<T>::Iterator::Iterator(T* pointer): pointer_(pointer) {};

template <typename T>
typename Vector<T>::Iterator& Vector<T>::Iterator::operator=(const Vector<T>::Iterator& other) {
    pointer_ = other.pointer_;
    return *this;
}

template <typename T>
T& Vector<T>::Iterator::operator*() {
    return *pointer_;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::Iterator::operator+(size_t index) {
    return Iterator(pointer_ + index);
}

template <typename T>
typename Vector<T>::Iterator& Vector<T>::Iterator::operator++() {
    pointer_++;
    return *this;
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::Iterator::operator++(int) {
    Iterator result = *this;
    ++this;
    return result;
}

template <typename T>
bool Vector<T>::Iterator::operator==(const Vector<T>::Iterator& other) { return (pointer_ == other.pointer_); }

template <typename T>
typename Vector<T>::Iterator Vector<T>::begin() const {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    return Iterator(&(container[0]));
}

template <typename T>
typename Vector<T>::Iterator Vector<T>::end() const {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    return Iterator((&container[size_ - 1]) + 1);
}

template <typename T>
bool Vector<T>::Iterator::operator!=(const Vector<T>::Iterator& other) { return !(*this == other); }

// Reverse iterator

template <typename T>
Vector<T>::ReverseIterator::ReverseIterator(T* pointer): pointer_(pointer) {};

template <typename T>
typename Vector<T>::ReverseIterator& Vector<T>::ReverseIterator::operator=(const Vector<T>::ReverseIterator& other) {
    pointer_ = other.pointer_;
    return *this;
}

template <typename T>
T& Vector<T>::ReverseIterator::operator*() {
    return *pointer_;
}

template <typename T>
typename Vector<T>::ReverseIterator Vector<T>::ReverseIterator::operator+(size_t index) {
    return ReverseIterator(pointer_ + index);
}

template <typename T>
typename Vector<T>::ReverseIterator& Vector<T>::ReverseIterator::operator++() {
    pointer_--;
    return *this;
}

template <typename T>
typename Vector<T>::ReverseIterator Vector<T>::ReverseIterator::operator++(int) {
    ReverseIterator result = *this;
    --this;
    return result;
}

template <typename T>
bool Vector<T>::ReverseIterator::operator==(const Vector<T>::ReverseIterator& other) { return (pointer_ == other.pointer_); }

template <typename T>
typename Vector<T>::ReverseIterator Vector<T>::rbegin() const {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    return ReverseIterator((&container[size_ - 1]));
}

template <typename T>
typename Vector<T>::ReverseIterator Vector<T>::rend() const {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }
    return ReverseIterator((&container[0]) - 1);
}

template <typename T>
bool Vector<T>::ReverseIterator::operator!=(const ReverseIterator& other) { return !(*this == other); }

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
void Vector<T>::reserve(size_t sz) {
    if (capacity_ >= sz) {
        return;
    }
    T* new_container = reinterpret_cast<T*>(operator new[](sz * single_object_size));
    for (size_t i = 0; i < size_; ++i) {
        try {
            if (std::is_move_assignable<T>::value) {
                new_container[i] = std::move(container[i]);
            } else if (std::is_copy_assignable<T>::value) {
                new_container[i] = std::move(container[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                new_container[j].~T();
            }
            return;
        };
    }
    clean(size_);
    operator delete(container);
    container = new_container;
    capacity_ = sz;
}

template <typename T>
size_t Vector<T>::capacity() const {
    return capacity_;
}

template <typename T>
void Vector<T>::shrink_to_fit() {
    T* new_container = reinterpret_cast<T*>(operator new[](single_object_size * size_));

    for (size_t i = 0; i < size_; ++i) {
        try {
            if (std::is_move_assignable<T>::value) {
                new_container[i] = std::move(container[i]);
            } else if (std::is_copy_assignable<T>::value) {
                new_container[i] = container[i];
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                new_container[i].~T();
            }
            break;
        };
        
    }

    clear();
    operator delete(container);

    container = new_container;
    capacity_ = size_;
}

// Modifiers

template <typename T>
void Vector<T>::clear() {
    clean(size_);
    operator delete(container);
    container = nullptr;
    size_ = 0;
    capacity_ = 0;    
}

template <typename T>
void Vector<T>::insert(Vector<T>::Iterator pos, const T& value) {
    T current = value;
    for (auto it = pos; it != end(); ++it) {
        T swap = *it;
        *it = current;
        current = std::move(swap);
    }
    container[size_++] = current;

    if (size_ == capacity_) {
        rellocate();
    }
}

template <typename T>
void Vector<T>::insert(Vector<T>::Iterator pos, T&& value) {
    T current = std::move(value);
    for (auto it = pos; it != end(); ++it) {
        T swap = *it;
        *it = current;
        current = std::move(swap);
    }
    container[size_++] = current;

    if (size_ == capacity_) {
        rellocate();
    }
}

template <typename T>
void Vector<T>::insert(Vector<T>::Iterator pos, std::initializer_list<T> initializer) {
    if (size_ + initializer.size() < capacity_) {

        auto init_it = initializer.begin();
        for (size_t i = 0; i < initializer.size(); ++i) {
            insert(pos, *init_it);
            ++pos;
            ++init_it;
        }
    } else {
        size_t index = 0;
        while (pos != end()) {
            ++index;
            ++pos;
        }
        // size_ - index
        reserve(2 * (size_ + initializer.size()));
        insert(Vector<T>::Iterator(&container[size_ - index]), initializer);
    }
}
template<typename T> 
template <class... Args>
void Vector<T>::emplace(Iterator pos, Args&&... args) {
    insert(pos, T(std::forward<Args>(args) ...));
}

template <typename T>
template <class... Args> 
void Vector<T>::emplace_back( Args&&... args ) {
    push_back(T(std::forward<Args>(args) ...));
}

template <typename T>
void Vector<T>::erase(Vector<T>::Iterator pos) {
    if (size_ == 0) {
        throw std::out_of_range("Container is empty");
    }

    (*pos).~T();
    auto last = Iterator(&container[size_ - 1]);
    for (; pos != last; ++pos) {
        *pos = std::move(*(pos + 1));
    }
    pop_back();
}

template <typename T>
void Vector<T>::erase(Vector<T>::Iterator start, Vector<T>::Iterator finish) {
    // [first, last)
    Vector<T>::Iterator it_s = start;
    Vector<T>::Iterator it_f = finish;

    size_t deleted = 0;
    for (auto it = start; it != finish; ++it) {
        (*it).~T();
        ++deleted;
    }

    for (; it_f != end(); ++it_f, ++it_s) {
        *it_s = std::move(*it_f);
    }
    resize(size_ - deleted);
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
            new_container[i] = std::move(container[i]);
        } else {
            new_container[i] = std::move(T()); // Должно быть
        }
    }
    size_ = new_size;
    capacity_ = size_ * 2;
    operator delete(container);
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
            new_container[i] = std::move(container[i]);
        } else {
            new_container[i] = value; // Должно быть
        }
    }
    size_ = new_size;
    capacity_ = size_ * 2;
    operator delete(container);
    container = new_container;
}

template <typename T>
void Vector<T>::swap(Vector<T>& other) {
    capacity_ = other.capacity_;
    size_ = other.size_;
    container = other.container;
}