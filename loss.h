#ifndef LOSS_H
#define LOSS_H

#include "dataset.h"
#include "model.h"

class ILossFunction {
public:
    virtual double compute(double pred, double target) = 0;
    virtual double derivative(double pred, double target) = 0;
    virtual ~ILossFunction() {}
};

class MSELoss : public ILossFunction {
public:
    double compute(double pred, double target) {
        return (pred - target) * (pred - target);
    }
    double derivative(double pred, double target) {
        return 2 * (pred - target);
    }
};

class MAELoss : public ILossFunction {
public:
    double compute(double pred, double target) override {
        return abs(pred - target);
    }

    double derivative(double pred, double target) override {
        const double error = pred - target;
        if (error > 0) return 1.0;
        if (error < 0) return -1.0;
        return 0.0;
    }
};

double averageLoss(LinearModel& model, DataSet& data, ILossFunction& loss) {
    if (data.numRows() == 0) return 0;
    double sum = 0;
    for (int r = 0; r < data.numRows(); r++) {
        vector<double> x = data.getFeatureRow(r);
        sum += loss.compute(model.predict(x), data.getTarget(r));
    }
    return sum / data.numRows();
}

double r2Score(LinearModel& model, DataSet& data) {
    if (data.numRows() < 2) return 0;

    double mean = 0;
    for (int r = 0; r < data.numRows(); r++) {
        mean += data.getTarget(r);
    }
    mean /= data.numRows();

    double ssRes = 0;
    double ssTot = 0;
    for (int r = 0; r < data.numRows(); r++) {
        vector<double> x = data.getFeatureRow(r);
        double t = data.getTarget(r);
        double p = model.predict(x);
        ssRes += (t - p) * (t - p);
        ssTot += (t - mean) * (t - mean);
    }
    if (ssTot == 0) return 0;
    return 1 - ssRes / ssTot;
}

#endif
