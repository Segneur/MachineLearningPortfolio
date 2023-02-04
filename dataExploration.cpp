#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
using namespace std;

double sum(vector<double> values) {
    double total = 0;

    for (int i = 0; i < values.size(); i++) {
        total = total + values[i];
    }

    return total;
}

double mean(vector<double> values) {
    double total = sum(values);

    return total / values.size();
}

double median(vector<double> values) {
    std::sort(values.begin(), values.end());

    if (values.size() % 2 == 1) {
        return values[values.size() / 2];
    }
    else {
        return (values[values.size() / 2] + values[values.size() / 2 - 1]) / 2;
    }
}

double range(vector<double> values) {
    std::sort(values.begin(), values.end());

    return values[values.size() - 1] - values[0];
}

double covariance(vector<double> xValues, vector<double> yValues) {
    double xMean = mean(xValues);
    double yMean = mean(yValues);

    double varianceXY = 0;

    for (int i = 0; i < max(xValues.size(), yValues.size()); i++) {
        varianceXY = varianceXY + (xValues[i] - xMean) * (yValues[i] - yMean);
    }

    return varianceXY / (xValues.size() - 1);
}

double correlation(vector <double> xValues, vector<double> yValues) {
    double sigmaX = sqrt(covariance(xValues, xValues));
    double sigmaY = sqrt(covariance(yValues, yValues));

    return covariance(xValues, yValues) / (sigmaX * sigmaY);
}

int main() {
    ifstream inFS;
    string line;
    string rm_in, medv_in;
    const int MAX_LEN = 1000;
    vector<double> rm(MAX_LEN);
    vector<double> medv(MAX_LEN);

    inFS.open("Boston.csv");
    if (!inFS.is_open()) {
        cout << "Could not open file." << endl;
        return 1;
    }

    getline(inFS, line);
    int numObservations = 0;
    while (inFS.good()) {
        getline(inFS, rm_in, ',');
        getline(inFS, medv_in, '\n');

        rm.at(numObservations) = stof(rm_in);
        medv.at(numObservations) = stof(medv_in);

        numObservations++;
    }

    rm.resize(numObservations);
    medv.resize(numObservations);

    inFS.close();


    cout << "Sum: " << sum(rm) << endl;
    cout << "Mean: " << mean(rm) << endl;
    cout << "Median: " << median(rm) << endl;
    cout << "Range: " << range(rm) << endl;
    
    cout << "Sum: " << sum(medv) << endl;
    cout << "Mean: " << mean(medv) << endl;
    cout << "Median: " << median(medv) << endl;
    cout << "Range: " << range(medv) << endl;

    cout << "Covariance: " << covariance(rm, medv) << endl;
    cout << "Correlation: " << correlation(rm, medv) << endl;

    return 0;
}