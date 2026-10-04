#ifndef DATASET_H
#define DATASET_H

#include "column.h"

class DataSet {
    vector<ColumnBase*> columns;
public:
    DataSet() {}
    DataSet(const DataSet&) = delete;

    ~DataSet() {
        for (auto column : columns) {
            delete column;
        }
    }

    void addColumn(ColumnBase* c) {
        if (c == NULL) return;
        columns.push_back(c);
    }

    int numRows() {
        if (columns.empty()) return 0;
        return columns[0]->size();
    }

    int numCols() {
        return columns.size();
    }

    string getColumnName(int i) {
        if (i < 0 || i >= numCols()) return "";
        return columns[i]->getName();
    }

    vector<double> getFeatureRow(int r) {
        vector<double> row;
        if (r < 0 || r >= numRows()) return row;
        for (int i = 0; i < numCols() - 1; i++) {
            row.push_back(columns[i]->getAsDouble(r));
        }
        return row;
    }

    double getTarget(int r) {
        if (r < 0 || r >= numRows() || numCols() < 2) return 0;
        return columns[numCols() - 1]->getAsDouble(r);
    }

    bool split(double ratio, DataSet& train, DataSet& test) {
        if (numCols() < 2 || numRows() < 2) {
            cout << "error: need at least 2 rows and 2 columns to split\n";
            return false;
        }
        if (train.numCols() != 0 || test.numCols() != 0) {
            cout << "error: train and test must be empty\n";
            return false;
        }
        if (!(ratio > 0 && ratio < 1)) {
            cout << "error: split ratio must be between 0 and 1\n";
            return false;
        }

        int n = numRows();
        vector<int> idx;
        for (int i = 0; i < n; i++) {
            idx.push_back(i);
        }

        mt19937 rng(42);
        shuffle(idx.begin(), idx.end(), rng);

        int trainCount = n * ratio;
        if (trainCount < 1) trainCount = 1;
        if (trainCount > n - 1) trainCount = n - 1;

        for (int c = 0; c < numCols(); c++) {
            Column<double>* a = new Column<double>(columns[c]->getName());
            Column<double>* b = new Column<double>(columns[c]->getName());
            for (int i = 0; i < n; i++) {
                double v = columns[c]->getAsDouble(idx[i]);
                if (i < trainCount) a->addValue(v);
                else b->addValue(v);
            }
            train.addColumn(a);
            test.addColumn(b);
        }
        return true;
    }
};

#endif
