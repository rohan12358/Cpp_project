#ifndef SCALER_H
#define SCALER_H

#include "dataset.h"

class IScaler {
public:
    virtual void fit(DataSet& data) = 0;
    virtual vector<double> transformRow(vector<double>& x) = 0;
    virtual ~IScaler() {}
};

class StandardScaler : public IScaler {
    vector<double> mean;
    vector<double> stddev;
public:
    void fit(DataSet& data) {
        int n = data.numCols() - 1;
        int rows = data.numRows();
        if (n < 0) n = 0;
        mean.assign(n, 0.0);
        stddev.assign(n, 1.0);
        if (rows == 0) return;

        for (int r = 0; r < rows; r++) {
            vector<double> x = data.getFeatureRow(r);
            for (int i = 0; i < n; i++) {
                mean[i] += x[i];
            }
        }
        for (int i = 0; i < n; i++) {
            mean[i] /= rows;
        }

        vector<double> sq(n, 0.0);
        for (int r = 0; r < rows; r++) {
            vector<double> x = data.getFeatureRow(r);
            for (int i = 0; i < n; i++) {
                sq[i] += (x[i] - mean[i]) * (x[i] - mean[i]);
            }
        }
        for (int i = 0; i < n; i++) {
            stddev[i] = sqrt(sq[i] / rows);
            if (stddev[i] == 0) stddev[i] = 1.0;
        }
    }

    vector<double> transformRow(vector<double>& x) {
        vector<double> out;
        if (x.size() != mean.size()) return out;
        for (int i = 0; i < x.size(); i++) {
            out.push_back((x[i] - mean[i]) / stddev[i]);
        }
        return out;
    }
};

bool scaleDataSet(IScaler& scaler, DataSet& in, DataSet& out) {
    int n = in.numCols() - 1;
    if (n < 1 || in.numRows() == 0) {
        cout << "error: nothing to scale\n";
        return false;
    }

    vector<Column<double>*> cols;
    for (int i = 0; i < n; i++) {
        cols.push_back(new Column<double>(in.getColumnName(i)));
    }
    Column<double>* target = new Column<double>(in.getColumnName(n));

    for (int r = 0; r < in.numRows(); r++) {
        vector<double> x = in.getFeatureRow(r);
        vector<double> s = scaler.transformRow(x);
        if (s.size() != n) {
            cout << "error: scaler does not match the data\n";
            for (auto column : cols) {
                delete column;
            }
            delete target;
            return false;
        }
        for (int i = 0; i < n; i++) {
            cols[i]->addValue(s[i]);
        }
        target->addValue(in.getTarget(r));
    }

    for (auto column : cols) {
        out.addColumn(column);
    }
    out.addColumn(target);
    return true;
}

#endif
