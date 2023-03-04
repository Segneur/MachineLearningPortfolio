#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;

vector<double> sigmoid(vector<double> data) {
    vector<double> probabilities(data.size(), 1);

    for (int i = 0; i < data.size(); i++) {
        probabilities[i] = 1 / (1 + exp(-1 * data[i]));
    }

    return probabilities;
}

vector<double> subtract(vector<double> a, vector<double> b) {
    vector<double> result(a.size());

    if (a.size() != b.size()) {
        throw std::invalid_argument("Lengths of Vectors are Different");
    }

    for (int i = 0; i < a.size(); i++) {
        result[i] = a[i] - b[i];
    }

    return result;
}

vector<double> multiply(vector<vector<double>> data, vector<double> weights) {
    vector<double> result(data.size(), 0);

    for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < weights.size(); j++) {
            result[i] = result[i] + data[i][j] * weights[j];
        }
    }

    return result;
}

vector<double> findDifference(vector<vector<double>> data, vector<double> error, double learning_rate) { 
    vector<double> result(data[0].size(), 0);
    
    for (int j = 0; j < data[0].size(); j++) {
        for (int i = 0; i < data.size(); i++) {
            result[j] = result[j] + data[i][j] * error[i] * learning_rate;
        }
    }

    return result;
}

vector<double> predict(vector<double> data) {
    vector<double> result(data.size());

    for (int i = 0; i < data.size(); i++) {
        result[i] = exp(data[i]) / (1 + exp(data[i]));
    }

    return result;
}

vector<double> find_coeffs(vector<vector<double>> data, vector<double> weights, vector<double> labels) {
    double learning_rate = 0.001;
    vector<double> prev_weights = {0, 0};
    vector<double> prob_vector(labels.size());
    vector<double> error(labels.size());

    while ((abs(weights[0] - prev_weights[0]) > .00000001) || (abs(weights[1] - prev_weights[1]) > .0000001)) {
        prob_vector = sigmoid(multiply(data, weights));
        error = subtract(labels, prob_vector);
        prev_weights = weights;
        weights = subtract(weights, findDifference(data, error, -learning_rate));
    }

    return weights;
}

vector<double> test(vector<vector<double>> data, vector<double> weights) {
    vector<double> probabilities(data.size());
    vector<double> predictions(data.size());

    probabilities = predict(multiply(data, weights));

    for (int i = 0; i < probabilities.size(); i++) {
        predictions[i] = probabilities[i] < .5 ?  0 : 1;
    }

    return predictions;
}

double accuracy(vector<double> results, vector<double> labels) {
    double correct = 0;
    
    for (int i = 0; i < results.size(); i++) {
        if (results[i] == labels[i]) {
            correct++;
        }
    }

    return correct / results.size();;
}

double sensitivity(vector<double> results, vector<double> labels) {
    double correct = 0;
    double count = 0;

    for (int i = 0; i < results.size(); i++) {
        if (labels[i] == 1) {
            count++;
            if (results[i] == 1) {
                correct++;
            }
        }
    }

    return correct / count;
}

double specificity(vector<double> results, vector<double> labels) {
    double correct = 0;
    double count = 0;

    for (int i = 0; i < results.size(); i++) {
        if (labels[i] == 0) {
            count++;
            if (results[i] == 0) {
                correct++;
            }
        }
    }

    return correct / count;
}


int main() {
    ifstream inFS;
    string line;
    string survived_in, sex_in;
    vector<vector<double>> train((800), vector<double>(2, 1));
    vector<vector<double>> test_data((800), vector<double>(2, 1));
    vector<double> labels_train(800);
    vector<double> labels_test(800);

    inFS.open("titanic.csv");
    if (!inFS.is_open()) {
        cout << "Could not open file." << endl;
        return 1;
    }

    int numObservations = 0;
    while (inFS.good()) {
        getline(inFS, survived_in, ',');
        getline(inFS, sex_in, '\n');

        if (numObservations < 800) {
            labels_train.at(numObservations) = stod(survived_in);
            train[numObservations][1] = stod(sex_in);
        }
        else {
            labels_test.at(numObservations - 800) = stod(survived_in);
            test_data[numObservations - 800][1] = stod(sex_in);
        }

        numObservations++;
    }

    inFS.close();


    test_data.resize(numObservations - 800);
    labels_test.resize(numObservations - 800);

    vector<double> weights = {1, 1};

    auto start = chrono::steady_clock::now();
    weights = find_coeffs(train, weights, labels_train);
    auto end = chrono::steady_clock::now();

    cout << "Elapsed time for training: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds" << endl;
    cout << "Intercept: " << weights[0] << endl;
    cout << "Slope: " << weights[1] << endl;

    vector <double> results(test_data.size());

    results = test(test_data, weights);

    cout << "Accuracy: " << accuracy(results, labels_test) << endl;
    cout << "Sensitivity: " << sensitivity(results, labels_test) << endl;
    cout << "Specificity: " << specificity(results, labels_test) << endl;

    return 0;
}