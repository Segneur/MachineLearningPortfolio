#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
#include <set>
using namespace std;

vector<double> priors(vector<double> labels) {
    vector<double> apriori = {0, 0};

    for (int i = 0; i < labels.size(); i++) {
        labels[i] == 0 ? apriori[0]++ : apriori[1]++;
    }

    apriori[0] = apriori[0] / labels.size();
    apriori[1] = apriori[1] / labels.size();

    return apriori;
}

vector<vector<double>> qual_likelihood(vector<double> data, vector<double> labels) {
    vector<vector<double>> probabilities((set<double>(labels.begin(), labels.end()).size()), vector<double>(set<double>(data.begin(), data.end()).size(), 0));
    double count;

    for (int i = 0; i < data.size(); i++) {
        if (labels[i] == 0) {
            count++;
        }

        probabilities[labels[i]][data[i]]++;
    }

    for (int i = 0; i < probabilities.size(); i++) {
        for (int j = 0; j < probabilities[0].size(); j++) {
            if (i == 0) {
                probabilities[i][j] = probabilities[i][j] / count;
            }
            else {
                probabilities[i][j] = probabilities[i][j] / (data.size() - count);
            }
        }
    }

    return probabilities;
}

vector<double> mean(vector<double> data, vector<double> labels) {
    vector<double> means(2);
    double survived = 0;
    double died = 0;
    int count = 0;

    for (int i = 0; i < data.size(); i++) {
        if (labels[i] == 0) {
            died+=data[i];
            count++;
        }
        else {
            survived+=data[i];
        }
    }

    means = {died / count, survived / (data.size() - count)};

    return means;
}

vector<double> variance(vector<double> data, vector<double> labels) {
    vector<double> means = mean(data, labels);
    vector<double> variances(2);
    double rss_1 = 0;
    double rss_2 = 0;
    int count = 0;

    for (int i = 0; i < data.size(); i++) {
        if (labels[i] == 0) {
            rss_1 += pow(means[0] - data[i], 2);
            count++;
        }
        else {
            rss_2 += pow(means[1] - data[i], 2);
        }
    }

    variances = {rss_1 / (count - 1), rss_2 / (data.size() - count - 1)};

    return variances;
}

vector<double> quant_likelihood(vector<double> data, double mean, double variance) {
    vector<double> likelihood(data.size());

    for (int i = 0; i < data.size(); i++) {
        likelihood[i] = 1 / (sqrt(2 * M_PI * variance)) * exp(-(pow(data[i] - mean, 2) / (2 * variance)));
    }

    return likelihood;
}

vector<double> predict(vector<double> prior, vector<vector<double>> class_likelihood, vector<vector<double>> sex_likelihood, vector<double> data_mean, vector<double> data_variance, vector<double> p_class_test, vector<double> sex_test, vector<double> age_test, vector<double> labels_test) {
    vector<double> predictions(p_class_test.size());
    double denominator = 0;

    vector<double> age_perished = quant_likelihood(age_test, data_mean[0], data_variance[0]);
    vector<double> age_survived = quant_likelihood(age_test, data_mean[1], data_variance[1]);

    for (int i = 0; i < p_class_test.size(); i++) {
        denominator = (class_likelihood[0][p_class_test[i]] * sex_likelihood[0][sex_test[i]] * prior[0] * age_perished[i]) + (class_likelihood[1][p_class_test[i]] * sex_likelihood[1][sex_test[i]] * prior[1] * age_survived[i]);
/*         predictions[i][0] = (class_likelihood[0][p_class_test[i]] * sex_likelihood[0][sex_test[i]] * prior[0] * age_perished[i]) / denominator;
        predictions[i][1] = (class_likelihood[1][p_class_test[i]] * sex_likelihood[1][sex_test[i]] * prior[1] * age_survived[i]) / denominator; */

        predictions[i] = (class_likelihood[0][p_class_test[i]] * sex_likelihood[0][sex_test[i]] * prior[0] * age_perished[i]) / denominator > .5 ? 0 : 1;
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
    string survived_in, sex_in, age_in, class_in;

    vector<double> train_class(800), train_sex(800), train_age(800), test_class(800), test_sex(800), test_age(800), labels_train(800), labels_test(800);

    inFS.open("titanic_full.csv");
    if (!inFS.is_open()) {
        cout << "Could not open file." << endl;
        return 1;
    }

    int numObservations = 0;
    while (inFS.good()) {
        getline(inFS, class_in, ',');
        getline(inFS, sex_in, ',');
        getline(inFS, age_in, ',');
        getline(inFS, survived_in, '\n');

        if (numObservations < 800) {
            labels_train.at(numObservations) = stod(survived_in);
            train_class[numObservations] = stod(class_in) - 1;
            train_sex[numObservations] = stod(sex_in);
            train_age[numObservations] = stod(age_in);        
        }
        else {
            labels_test.at(numObservations - 800) = stod(survived_in);
            test_class[numObservations - 800] = stod(class_in) - 1;
            test_sex[numObservations - 800] = stod(sex_in);
            test_age[numObservations - 800] = stod(age_in);   
        }

        numObservations++;
    }

    inFS.close();

    test_class.resize(numObservations - 800);
    test_sex.resize(numObservations - 800);
    test_age.resize(numObservations - 800);
    labels_test.resize(numObservations - 800);

    auto start = chrono::steady_clock::now();

    vector<double> apriori = priors(labels_train);
    vector<double> data_mean = mean(train_age, labels_train);
    vector<double> data_variance = variance(train_age, labels_train);

    vector<vector<double>> pas_class = qual_likelihood(train_class, labels_train);
    vector<vector<double>> pas_sex = qual_likelihood(train_sex, labels_train);
    vector<double> pas_age_perished = quant_likelihood(train_age, data_mean[0], data_variance[0]);
    vector<double> pas_age_survived = quant_likelihood(train_age, data_mean[1], data_variance[1]);

    auto end = chrono::steady_clock::now();

    cout << "Elapsed time for training: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " microseconds" << endl;

    cout << "Dataset Priors:" << endl;
    cout << "0 1" << endl;
    cout << apriori[0] << " " << apriori[1] << endl;

    cout << "Passenger Survival Probabilities Based on Class:" << endl;
    cout << "1 2 3" << endl;
    for (int i = 0; i < pas_class.size(); i++) {
        cout << i << " ";
        for (int j = 0; j < pas_class[0].size(); j++) {
            cout << pas_class[i][j] << " ";
        }
        cout << endl;
    }

    cout << "Passenger Survival Probabilities Based on Sex:" << endl;
    cout << "Female Male" << endl;
    for (int i = 0; i < pas_sex.size(); i++) {
        cout << i << " ";
        for (int j = 0; j < pas_sex[0].size(); j++) {
            cout << pas_sex[i][j] << " ";
        }
        cout << endl;
    }

    vector<double> predictions = predict(apriori, pas_class, pas_sex, data_mean, data_variance, test_class, test_sex, test_age, labels_test);

    cout << "Accuracy: " << accuracy(predictions, labels_test) << endl;
    cout << "Sensitivity: " << sensitivity(predictions, labels_test) << endl;
    cout << "Specificity: " << specificity(predictions, labels_test) << endl;

    return 0;
}