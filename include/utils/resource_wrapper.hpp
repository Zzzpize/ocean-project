#pragma once
#include <cstring> 
#include <utility> 
#include <iostream> 

class ResourceWrapper {
private:
    char* data_;
    size_t size_; 

public:
    explicit ResourceWrapper(const char* str = "") {
        if (str == nullptr) str = ""; 
        size_ = std::strlen(str) + 1;
        data_ = new char[size_];
        #ifdef _MSC_VER
        strcpy_s(data_, size_, str);
        #else
        std::strcpy(data_, str);
        #endif
    }

    ~ResourceWrapper() {
        delete[] data_;
        data_ = nullptr; 
        size_ = 0;
    }

    ResourceWrapper(const ResourceWrapper& other) : size_(other.size_) {
        if (other.data_) {
            data_ = new char[size_];
            #ifdef _MSC_VER
            strcpy_s(data_, size_, other.data_);
            #else
            std::strcpy(data_, other.data_);
            #endif
        } else {
            data_ = nullptr; 
            size_ = 0; 
        }
    }

    ResourceWrapper& operator=(const ResourceWrapper& other) {
        if (this != &other) { 
            ResourceWrapper temp(other); 
            swap(*this, temp);           
        }                                
        return *this;
    }

    ResourceWrapper(ResourceWrapper&& other) noexcept 
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    ResourceWrapper& operator=(ResourceWrapper&& other) noexcept {
        if (this != &other) { 
            delete[] data_; 

            data_ = other.data_; 
            size_ = other.size_;

            other.data_ = nullptr; 
            other.size_ = 0;
        }
        return *this;
    }
    
    friend void swap(ResourceWrapper& first, ResourceWrapper& second) noexcept {
        using std::swap;
        swap(first.data_, second.data_);
        swap(first.size_, second.size_);
    }

    void print(const std::string& prefix = "") const {
        if (!prefix.empty()) {
            std::cout << prefix;
        }
        if (data_) {
            std::cout << "ResourceWrapper data: \"" << data_ << "\" (size: " << size_ << ", addr: " << static_cast<const void*>(data_) << ")" << std::endl;
        } else {
            std::cout << "ResourceWrapper data: null (size: " << size_ << ", addr: null)" << std::endl;
        }
    }

    bool operator==(const ResourceWrapper& other) const {
        if (size_ != other.size_) return false;
        if (data_ == nullptr && other.data_ == nullptr) return true;
        if (data_ == nullptr || other.data_ == nullptr) return false;
        return std::strcmp(data_, other.data_) == 0;
    }
};