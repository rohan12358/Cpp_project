#ifndef MODEL_H
#define MODEL_H

#include <bits/stdc++.h>
using namespace std;

class LinearModel {
    vector<double> weights;
    double bias;
public:
    LinearModel(int n) {
        if (n < 0) n = 0;
        weights.assign(n, 0.0);
        bias = 0.0;
    }

    double predict(vector<double>& x) {
        if (x.size() != weights.size()) return NAN;
        double sum = bias;
        for (int i = 0; i < weights.size(); i++) {
            sum += weights[i] * x[i];
        }
        return sum;
    }

    int numWeights() {
        return weights.size();
    }

    double getWeight(int i) {
        if (i < 0 || i >= weights.size()) return 0;
        return weights[i];
    }

    void setWeight(int i, double v) {
        if (i < 0 || i >= weights.size()) return;
        weights[i] = v;
    }

    double getBias() {
        return bias;
    }

    void setBias(double v) {
        bias = v;
    }
};

#endif
