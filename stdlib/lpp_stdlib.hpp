// LPP Standard Library - Data Structures
// This file provides modern data structures that transpile to C++ STL equivalents

#ifndef LPP_STDLIB_H
#define LPP_STDLIB_H

#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <deque>
#include <list>
#include <vector>
#include <algorithm>

namespace lpp
{
    namespace stdlib
    {

        // ===== SET =====
        template <typename T>
        class Set
        {
        private:
            std::set<T> data;

        public:
            void add(const T &item) { data.insert(item); }
            void remove(const T &item) { data.erase(item); }
            bool has(const T &item) const { return data.find(item) != data.end(); }
            int size() const { return data.size(); }
            void clear() { data.clear(); }

            auto begin() { return data.begin(); }
            auto end() { return data.end(); }
        };

        // ===== MAP =====
        template <typename K, typename V>
        class Map
        {
        private:
            std::map<K, V> data;

        public:
            void set(const K &key, const V &value) { data[key] = value; }
            V get(const K &key) const { return data.at(key); }
            bool has(const K &key) const { return data.find(key) != data.end(); }
            void remove(const K &key) { data.erase(key); }
            int size() const { return data.size(); }
            void clear() { data.clear(); }

            auto begin() { return data.begin(); }
            auto end() { return data.end(); }
        };

        // ===== HASHSET =====
        template <typename T>
        class HashSet
        {
        private:
            std::unordered_set<T> data;

        public:
            void add(const T &item) { data.insert(item); }
            void remove(const T &item) { data.erase(item); }
            bool has(const T &item) const { return data.find(item) != data.end(); }
            int size() const { return data.size(); }
            void clear() { data.clear(); }

            auto begin() { return data.begin(); }
            auto end() { return data.end(); }
        };

        // ===== HASHMAP =====
        template <typename K, typename V>
        class HashMap
        {
        private:
            std::unordered_map<K, V> data;

        public:
            void set(const K &key, const V &value) { data[key] = value; }
            V get(const K &key) const { return data.at(key); }
            bool has(const K &key) const { return data.find(key) != data.end(); }
            void remove(const K &key) { data.erase(key); }
            int size() const { return data.size(); }
            void clear() { data.clear(); }

            auto begin() { return data.begin(); }
            auto end() { return data.end(); }
        };

        // ===== QUEUE (FIFO) =====
        template <typename T>
        class Queue
        {
        private:
            std::queue<T> data;

        public:
            void enqueue(const T &item) { data.push(item); }
            T dequeue()
            {
                // FIX BUG #201a: No empty check causes undefined behavior
                if (data.empty())
                    throw std::runtime_error("dequeue from empty queue");
                T item = std::move(const_cast<T&>(data.front())); // Move instead of copy
                data.pop();
                return item;
            }
            T peek() const
            {
                // FIX BUG #201a: peek() crashes on empty queue
                if (data.empty())
                    throw std::runtime_error("peek on empty queue");
                return data.front();
            }
            bool isEmpty() const { return data.empty(); }
            int size() const { return data.size(); }
        };

        // ===== STACK (LIFO) =====
        template <typename T>
        class Stack
        {
        private:
            std::stack<T> data;

        public:
            void push(const T &item) { data.push(item); }
            T pop()
            {
                T item = data.top();
                data.pop();
                return item;
            }
            T peek() const { return data.top(); }
            bool isEmpty() const { return data.empty(); }
            int size() const { return data.size(); }
        };

        // ===== DEQUE (Double-ended queue) =====
        template <typename T>
        class Deque
        {
        private:
            std::deque<T> data;

        public:
            void pushFront(const T &item) { data.push_front(item); }
            void pushBack(const T &item) { data.push_back(item); }
            T popFront()
            {
                // FIX BUG #201b: No empty check causes undefined behavior
                if (data.empty())
                    throw std::runtime_error("popFront from empty deque");
                T item = std::move(data.front()); // Move semantics
                data.pop_front();
                return item;
            }
            T popBack()
            {
                // FIX BUG #201b: No empty check causes undefined behavior
                if (data.empty())
                    throw std::runtime_error("popBack from empty deque");
                T item = std::move(data.back()); // Move semantics
                data.pop_back();
                return item;
            }
            T peekFront() const
            {
                // FIX BUG #201b: peek crashes on empty deque
                if (data.empty())
                    throw std::runtime_error("peekFront on empty deque");
                return data.front();
            }
            T peekBack() const
            {
                // FIX BUG #201b: peek crashes on empty deque
                if (data.empty())
                    throw std::runtime_error("peekBack on empty deque");
                return data.back();
            }
            int size() const { return data.size(); }
            bool isEmpty() const { return data.empty(); }
        };

        // ===== PRIORITY QUEUE =====
        template <typename T>
        class PriorityQueue
        {
        private:
            std::priority_queue<T> data;

        public:
            void push(const T &item) { data.push(item); }
            T pop()
            {
                T item = data.top();
                data.pop();
                return item;
            }
            T peek() const { return data.top(); }
            int size() const { return data.size(); }
            bool isEmpty() const { return data.empty(); }
        };

        // ===== LINKED LIST =====
        template <typename T>
        class LinkedList
        {
        private:
            std::list<T> data;

        public:
            void pushFront(const T &item) { data.push_front(item); }
            void pushBack(const T &item) { data.push_back(item); }
            T popFront()
            {
                // FIX BUG #201c: No empty check causes undefined behavior
                if (data.empty())
                    throw std::runtime_error("popFront from empty list");
                T item = std::move(data.front()); // Move semantics
                data.pop_front();
                return item;
            }
            T popBack()
            {
                // FIX BUG #201c: No empty check causes undefined behavior
                if (data.empty())
                    throw std::runtime_error("popBack from empty list");
                T item = std::move(data.back()); // Move semantics
                data.pop_back();
                return item;
            }
            int size() const { return data.size(); }
            bool isEmpty() const { return data.empty(); }

            auto begin() { return data.begin(); }
            auto end() { return data.end(); }
        };

        // ===== STRING UTILITIES =====

        // Get length of string or vector
        template <typename T>
        int len(const std::vector<T> &vec)
        {
            return vec.size();
        }

        int len(const std::string &str)
        {
            return str.length();
        }

        // Push element to vector
        template <typename T>
        void push(std::vector<T> &vec, const T &item)
        {
            vec.push_back(item);
        }

        // Pop element from vector
        template <typename T>
        T pop(std::vector<T> &vec)
        {
            if (vec.empty())
                throw std::runtime_error("pop from empty vector");
            T item = vec.back();
            vec.pop_back();
            return item;
        }

        // Split string by delimiter
        std::vector<std::string> split(const std::string &str, const std::string &delimiter)
        {
            std::vector<std::string> result;
            size_t start = 0;
            size_t end = str.find(delimiter);

            while (end != std::string::npos)
            {
                result.push_back(str.substr(start, end - start));
                start = end + delimiter.length();
                end = str.find(delimiter, start);
            }
            result.push_back(str.substr(start));
            return result;
        }

        // Join vector of strings with delimiter
        std::string join(const std::vector<std::string> &vec, const std::string &delimiter)
        {
            std::string result;
            for (size_t i = 0; i < vec.size(); i++)
            {
                result += vec[i];
                if (i < vec.size() - 1)
                {
                    result += delimiter;
                }
            }
            return result;
        }

        // Slice vector
        template <typename T>
        std::vector<T> slice(const std::vector<T> &vec, int start, int end = -1)
        {
            if (end == -1)
                end = vec.size();
            if (start < 0)
                start = vec.size() + start;
            if (end < 0)
                end = vec.size() + end;

            std::vector<T> result;
            for (int i = start; i < end && i < (int)vec.size(); i++)
            {
                result.push_back(vec[i]);
            }
            return result;
        }

        // Slice string
        std::string slice(const std::string &str, int start, int end = -1)
        {
            if (end == -1)
                end = str.length();
            if (start < 0)
                start = str.length() + start;
            if (end < 0)
                end = str.length() + end;
            return str.substr(start, end - start);
        }

        // Get character at index
        char charAt(const std::string &str, int index)
        {
            if (index < 0 || index >= (int)str.length())
            {
                throw std::out_of_range("charAt index out of range");
            }
            return str[index];
        }

        // Substring
        std::string substring(const std::string &str, int start, int length)
        {
            return str.substr(start, length);
        }

        // To uppercase
        std::string toUpper(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            return result;
        }

        // To lowercase
        std::string toLower(const std::string &str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }

        // Trim whitespace
        std::string trim(const std::string &str)
        {
            size_t start = str.find_first_not_of(" \t\n\r");
            if (start == std::string::npos)
                return "";
            size_t end = str.find_last_not_of(" \t\n\r");
            return str.substr(start, end - start + 1);
        }

        // String contains
        bool contains(const std::string &str, const std::string &substr)
        {
            return str.find(substr) != std::string::npos;
        }

        // String starts with
        bool startsWith(const std::string &str, const std::string &prefix)
        {
            return str.substr(0, prefix.length()) == prefix;
        }

        // String ends with
        bool endsWith(const std::string &str, const std::string &suffix)
        {
            if (suffix.length() > str.length())
                return false;
            return str.substr(str.length() - suffix.length()) == suffix;
        }

        // Replace all occurrences
        std::string replace(const std::string &str, const std::string &from, const std::string &to)
        {
            std::string result = str;
            size_t pos = 0;
            while ((pos = result.find(from, pos)) != std::string::npos)
            {
                result.replace(pos, from.length(), to);
                pos += to.length();
            }
            return result;
        }

        // Repeat string n times
        std::string repeat(const std::string &str, int count)
        {
            std::string result;
            for (int i = 0; i < count; i++)
            {
                result += str;
            }
            return result;
        }

        // Reverse string
        std::string reverse(const std::string &str)
        {
            return std::string(str.rbegin(), str.rend());
        }

    } // namespace stdlib
} // namespace lpp

#endif // LPP_STDLIB_H
