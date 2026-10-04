#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <matplot/matplot.h>
#include <matplot/backend/backend_registry.h>
#include "dataset.h"
#include "model.h"

namespace plt = matplot;

class Visualizer {
    plt::figure_handle fig;
    plt::axes_handle lossAxes;
    plt::axes_handle scatterAxes;
    plt::line_handle lossLine;
    int totalEpochs;
    int firstSnapshotEpoch;
    int secondSnapshotEpoch;
    bool opened;
    vector<double> epochs;
    vector<double> losses;

    void fail() {
        if (opened) cout << "warning: could not draw the plot (is gnuplot installed?), training without it\n";
        opened = false;
    }
public:
    Visualizer(int e) {
        if (e < 1) e = 1;
        totalEpochs = e;
        firstSnapshotEpoch = max(1, (totalEpochs + 1) / 3);
        secondSnapshotEpoch = max(firstSnapshotEpoch + 1, (2 * totalEpochs + 1) / 3);
        opened = true;
        try {
            // Keep the figure quiet so changing plot data while saving the
            // first two checkpoints does not redraw the interactive window.
            fig = plt::figure(true);
            fig->backend(plt::create_default_backend());
            fig->size(1000, 450);
            plt::tiledlayout(1, 2);
            lossAxes = plt::nexttile();
            scatterAxes = plt::nexttile();

            lossLine = lossAxes->plot(vector<double>{}, vector<double>{}, "b");
            lossAxes->xlabel("epoch");
            lossAxes->ylabel("log10 of training MSE");
        } catch (...) {
            fail();
        }
    }

    bool isOpen() {
        return opened;
    }

    void update(LinearModel& model, DataSet& train, DataSet& test, int epoch, double loss) {
        if (!opened) return;
        if (isfinite(loss) && loss > 0) {
            epochs.push_back(epoch);
            losses.push_back(log10(loss));
        }
        const bool snapshotEpoch = epoch == firstSnapshotEpoch ||
                                   epoch == secondSnapshotEpoch ||
                                   epoch == totalEpochs;
        if (!snapshotEpoch) return;
        if (epochs.empty()) return;

        vector<double> trainX, trainY, testX, testY;
        bool first = true;
        double lo = 0;
        double hi = 0;
        for (int r = 0; r < train.numRows() + test.numRows(); r++) {
            bool isTrain = r < train.numRows();
            DataSet& d = isTrain ? train : test;
            int row = isTrain ? r : r - train.numRows();
            vector<double> x = d.getFeatureRow(row);
            double p = model.predict(x);
            double t = d.getTarget(row);
            if (!isfinite(p) || !isfinite(t)) continue;
            if (isTrain) {
                trainX.push_back(t);
                trainY.push_back(p);
            } else {
                testX.push_back(t);
                testY.push_back(p);
            }
            if (first) {
                lo = min(t, p);
                hi = max(t, p);
                first = false;
            }
            lo = min(lo, min(t, p));
            hi = max(hi, max(t, p));
        }

        try {
            lossLine->x_data(epochs).y_data(losses);
            lossAxes->title("epoch " + to_string(epoch) + " / " + to_string(totalEpochs) + "   loss " + to_string((long long)loss));
            lossAxes->xlim({0.0, (double)totalEpochs});

            if (!first) {
                if (hi == lo) hi = lo + 1;
                vector<double> lineX = {lo, hi};
                vector<double> lineY = {lo, hi};

                // Rebuild this axes with data-bound marker objects at each
                // snapshot so Matplot++ renders the train and test series reliably.
                scatterAxes->clear();
                scatterAxes->title("actual vs predicted price (blue = train, red = test)");
                scatterAxes->xlabel("actual price");
                scatterAxes->ylabel("predicted price");

                scatterAxes->hold(false);
                auto idealLine = scatterAxes->plot(lineX, lineY, "k--");
                idealLine->line_width(0.8f);

                scatterAxes->hold(true);
                auto trainLine = scatterAxes->plot(trainX, trainY, "ob");
                trainLine->line_spec().line_style(plt::line_spec::line_style::none);
                trainLine->marker_size(12.0f);

                auto testLine = scatterAxes->plot(testX, testY, "or");
                testLine->line_spec().line_style(plt::line_spec::line_style::none);
                testLine->marker_size(12.0f);
                scatterAxes->hold(false);

                scatterAxes->xlim({lo, hi});
                scatterAxes->ylim({lo, hi});
            }
            // Save each checkpoint to PNG without opening an interim window.
            string filename = "../plots/epoch_" + to_string(epoch) + ".png";
            if (!fig->save(filename)) {
                cout << "warning: could not save plot " << filename << "\n";
            }

            // Only display the completed training plot.
            if (epoch == totalEpochs) fig->draw();
        } catch (...) {
            fail();
        }
    }

    void waitUntilClosed() {
        if (!opened) return;
        try {
            fig->show();
        } catch (...) {
            fail();
        }
    }
};

#endif
