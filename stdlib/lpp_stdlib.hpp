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
                T item = data.front();
                data.pop();
                return item;
            }
            T peek() const { return data.front(); }
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
                T item = data.front();
                data.pop_front();
                return item;
            }
            T popBack()
            {
                T item = data.back();
                data.pop_back();
                return item;
            }
            T peekFront() const { return data.front(); }
            T peekBack() const { return data.back(); }
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
                T item = data.front();
                data.pop_front();
                return item;
            }
            T popBack()
            {
                T item = data.back();
                data.pop_back();
                return item;
            }
            int size() const { return data.size(); }
            bool isEmpty() const { return data.empty(); }

            auto begin() { return data.begin(); }
            auto end() { return data.end(); }
        };

    } // namespace stdlib
} // namespace lpp

#endif // LPP_STDLIB_H
