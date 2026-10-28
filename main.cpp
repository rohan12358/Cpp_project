#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "visualizer.h"
#include "importer.h"
#include "model.h"
#include "loss.h"
#include "dataset.h"
#include "scaler.h"
#include "optimizer.h"

using namespace std;

bool readNumber(const string& prompt, double& out) {
    while (true) {
        cout << prompt;
        string s;
        if (!(cin >> s)) return false;
        if (s == "q" || s == "Q") return false;
        try {
            size_t used = 0;
            out = stod(s, &used);
            if (used == s.size() && isfinite(out)) return true;
        } catch (...) {
        }
        cout << "please type a number (or q to quit)\n";
    }
}

int main(int argc, char* argv[]) {
    string file = "houses.csv";
    if (argc > 1) file = argv[1];

    cout << "Select loss (1: MSE, 2: MAE) [1]: ";
    string s;
    getline(cin, s);
    bool isMAE = (s == "2");
    ILossFunction* loss = isMAE ? static_cast<ILossFunction*>(new MAELoss()) : new MSELoss();
    string lossLabel = isMAE ? "MAE" : "MSE";

    cout << "Select optimizer (1: SGD, 2: Batch) [1]: ";
    getline(cin, s);
    bool isBatch = (s == "2");
    string optLabel = isBatch ? "Batch" : "SGD";

    double lr = 0.001;
    cout << "Enter learning rate [0.001]: ";
    getline(cin, s);
    if (!s.empty()) {
        try {
            double val = stod(s);
            if (val > 0) lr = val;
        } catch (...) {
        }
    }

    Optimizer* opt = isBatch ? static_cast<Optimizer*>(new BatchGradientDescentOptimizer(lr))
                             : new SGDOptimizer(lr);

    DataSet data;
    Importer imp;
    if (!imp.loadCSV(file, data)) return 1;
    cout << "rows: " << data.numRows() << " cols: " << data.numCols() << "\n";

    DataSet train, test;
    if (!data.split(0.8, train, test)) return 1;
    cout << "train rows: " << train.numRows() << " test rows: " << test.numRows() << "\n";

    StandardScaler scaler;
    scaler.fit(train);

    DataSet trainScaled, testScaled;
    if (!scaleDataSet(scaler, train, trainScaled)) return 1;
    if (!scaleDataSet(scaler, test, testScaled)) return 1;

    int epochs = 500;
    LinearModel model(data.numCols() - 1);
    Visualizer viz(epochs, lossLabel);

    cout << "training with " << lossLabel << " + " << optLabel
         << " (lr=" << lr << ", epochs=" << epochs << ")\n";

    double l = 0;
    bool diverged = false;
    for (int e = 1; e <= epochs; e++) {
        opt->update(model, trainScaled, *loss);
        l = averageLoss(model, trainScaled, *loss);
        if (!isfinite(l)) {
            cout << "training blew up at epoch " << e << ", try a smaller learning rate\n";
            diverged = true;
            break;
        }
        viz.update(model, trainScaled, testScaled, e, l);
        if (e % 100 == 0 || e == epochs) cout << "epoch " << e << " loss " << l << endl;
    }

    if (diverged) {
        delete loss;
        delete opt;
        return 1;
    }

    viz.waitUntilClosed();

    cout << "weights:";
    for (int i = 0; i < model.numWeights(); i++) {
        cout << " " << model.getWeight(i);
    }
    cout << "\nbias: " << model.getBias() << "\n";

    double testLoss = averageLoss(model, testScaled, *loss);
    cout << "test loss (" << lossLabel << "): " << testLoss;
    if (!isMAE) {
        cout << " (RMSE: $" << static_cast<long long>(sqrt(testLoss)) << ")";
    }
    cout << endl;
    cout << "test R2: " << r2Score(model, testScaled) << endl;

    cout << "enter a new input (q to quit)" << endl;
    while (true) {
        vector<double> x;
        bool quit = false;
        for (int i = 0; i < data.numCols() - 1; i++) {
            double v;
            if (!readNumber(data.getColumnName(i) + ": ", v)) {
                quit = true;
                break;
            }
            x.push_back(v);
        }
        if (quit) break;

        vector<double> xs = scaler.transformRow(x);
        double p = model.predict(xs);
        if (!isfinite(p)) cout << "could not predict for that input\n";
        else cout << "predicted " << data.getColumnName(data.numCols() - 1) << ": " << p << endl;
    }

    delete loss;
    delete opt;
    return 0;
}
