#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "loss.h"

class Optimizer {
protected:
    double learningRate;
public:
    Optimizer(double lr) {
        if (!(lr > 0) || !isfinite(lr)) lr = 0.0001;
        learningRate = lr;
    }
    virtual void update(LinearModel& model, DataSet& data, ILossFunction& loss) = 0;
    virtual ~Optimizer() {}
};

class SGDOptimizer : public Optimizer {
public:
    SGDOptimizer(double lr) : Optimizer(lr) {}

    void update(LinearModel& model, DataSet& data, ILossFunction& loss) {
        if (data.numRows() == 0) return;
        for (int r = 0; r < data.numRows(); r++) {
            vector<double> x = data.getFeatureRow(r);
            if (x.size() != model.numWeights()) return;
            double p = model.predict(x);
            double grad = loss.derivative(p, data.getTarget(r));
            if (!isfinite(grad)) return;
            for (int i = 0; i < model.numWeights(); i++) {
                model.setWeight(i, model.getWeight(i) - learningRate * grad * x[i]);
            }
            model.setBias(model.getBias() - learningRate * grad);
        }
    }
};

class BatchGradientDescentOptimizer : public Optimizer {
public:
    BatchGradientDescentOptimizer(double lr) : Optimizer(lr) {}

    void update(LinearModel& model, DataSet& data, ILossFunction& loss) override {
        const int rows = data.numRows();
        const int weights = model.numWeights();
        if (rows == 0) return;

        vector<double> weightGradients(weights, 0.0);
        double biasGradient = 0.0;
        for (int r = 0; r < rows; ++r) {
            vector<double> x = data.getFeatureRow(r);
            if (static_cast<int>(x.size()) != weights) return;
            const double gradient = loss.derivative(model.predict(x), data.getTarget(r));
            if (!isfinite(gradient)) return;
            for (int i = 0; i < weights; ++i) {
                weightGradients[i] += gradient * x[i];
            }
            biasGradient += gradient;
        }

        const double scale = learningRate / rows;
        for (int i = 0; i < weights; ++i) {
            model.setWeight(i, model.getWeight(i) - scale * weightGradients[i]);
        }
        model.setBias(model.getBias() - scale * biasGradient);
    }
};




#endif
