#ifndef IMPORTER_H
#define IMPORTER_H

#include "dataset.h"

class Importer {
    string trim(string s) {
        while (!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
        int i = 0;
        while (i < s.size() && isspace((unsigned char)s[i])) i++;
        return s.substr(i);
    }

    bool toNumber(string s, double& out) {
        s = trim(s);
        if (s.empty()) return false;
        try {
            size_t used = 0;
            out = stod(s, &used);
            if (used != s.size()) return false;
        } catch (...) {
            return false;
        }
        return isfinite(out);
    }

public:
    bool loadCSV(string file, DataSet& dataset) {
        ifstream in(file);
        if (!in.is_open()) {
            cout << "error: cannot open " << file << "\n";
            return false;
        }

        string line;
        if (!getline(in, line)) {
            cout << "error: " << file << " is empty\n";
            return false;
        }
        if (line.size() >= 3 && line.substr(0, 3) == "\xEF\xBB\xBF") line = line.substr(3);
        if (!line.empty() && line.back() == '\r') line.pop_back();

        vector<Column<double>*> cols;
        stringstream header(line);
        string cell;
        while (getline(header, cell, ',')) cols.push_back(new Column<double>(trim(cell)));

        if (cols.size() < 2) {
            cout << "error: need at least one feature column and one target column\n";
            for (auto column : cols) {
                delete column;
            }
            return false;
        }

        int good = 0;
        int skipped = 0;
        while (getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (trim(line).empty()) continue;

            vector<double> row;
            stringstream ss(line);
            bool ok = true;
            while (getline(ss, cell, ',')) {
                double v;
                if (!toNumber(cell, v)) {
                    ok = false;
                    break;
                }
                row.push_back(v);
            }
            if (!line.empty() && line.back() == ',') ok = false;

            if (!ok || row.size() != cols.size()) {
                skipped++;
                continue;
            }
            for (int i = 0; i < cols.size(); i++) {
                cols[i]->addValue(row[i]);
            }
            good++;
        }

        if (skipped > 0) cout << "warning: skipped " << skipped << " bad rows\n";
        if (good < 2) {
            cout << "error: need at least 2 valid rows, found " << good << "\n";
            for (auto column : cols) {
                delete column;
            }
            return false;
        }

        for (auto column : cols) {
            dataset.addColumn(column);
        }
        return true;
    }
};

#endif
