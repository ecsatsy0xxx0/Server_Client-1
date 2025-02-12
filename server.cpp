#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <set>

using namespace std;

const string SERVER_FILE = "C:/path/cons.txt";
const string RESULT_FILE = "C:/path/result.txt";

string evaluateDevelopment(float weight, float height) {
    float bmi = weight / ((height / 100) * (height / 100));
    if (bmi < 18.5) {
        return "Underweight";
    }
    else if (bmi >= 18.5 && bmi <= 24.9) {
        return "Normal";
    }
    else {
        return "Overweight";
    }
}

void loadProcessedResults(set<string>& processedResults) {
    ifstream resultFile(RESULT_FILE);
    string line;
    while (getline(resultFile, line)) {
        processedResults.insert(line);
    }
}

int main() {
    ifstream inputFile(SERVER_FILE);
    ofstream resultFile(RESULT_FILE, ios::app);

    if (!inputFile.is_open()) {
        cerr << "Failed to open server file!" << endl;
        return 1;
    }

    if (!resultFile.is_open()) {
        cerr << "Failed to open result file!" << endl;
        return 1;
    }

    set<string> processedEntries;
    loadProcessedResults(processedEntries);

    string line;

    while (true) {
        inputFile.clear();
        inputFile.seekg(0, ios::beg);

        while (getline(inputFile, line)) {
            if (line.empty() || processedEntries.count(line)) continue;

            istringstream iss(line);
            string surname;
            float height, weight;

            if (!(iss >> surname >> height >> weight)) {
                cerr << "Invalid input format: " << line << endl;
                continue;
            }

            string result = evaluateDevelopment(weight, height);
            string outputLine = "Student: " + surname + ", Result: " + result;
            
            if (processedEntries.insert(line).second) {
                cout << outputLine << endl;
                resultFile << outputLine << endl;
                resultFile.flush();
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
        }

    inputFile.close();
    resultFile.close();
    return 0;
}