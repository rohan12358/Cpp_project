#ifndef COLUMN_H
#define COLUMN_H

#include <bits/stdc++.h>
using namespace std;

class ColumnBase {
public:
    virtual string getName() = 0;
    virtual int size() = 0;
    virtual double getAsDouble(int i) = 0;
    virtual ~ColumnBase() {}
};

template <typename T>
class Column : public ColumnBase {
    string name;
    vector<T> data;
public:
    Column(string n) {
        name = n;
    }
    void addValue(T v) {
        data.push_back(v);
    }
    string getName() {
        return name;
    }
    int size() {
        return data.size();
    }
    double getAsDouble(int i) {
        return data[i];
    }
};

#endif

