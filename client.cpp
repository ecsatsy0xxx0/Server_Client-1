#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <sys/stat.h>
using namespace std;

const string SERVER_FILE = "C:/path/cons.txt";
const string RESULT_FILE = "C:/path/result.txt";

bool resultFileUpdated = false;
mutex mtx;
time_t lastReadTime = 0;

void displayNewResults() {
    ifstream resultFile(RESULT_FILE);
    string line;
    while (getline(resultFile, line)) {
        if (resultFile.tellg() > lastReadTime) {
            cout << line << endl;
        }
    }
    lastReadTime = resultFile.tellg();
}
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

void monitorResultFile() {
    time_t lastModifiedTime = 0;
    struct stat fileStats;

    while (true) {
        this_thread::sleep_for(chrono::milliseconds(100));
        if (stat(RESULT_FILE.c_str(), &fileStats) == 0) {
            if (lastModifiedTime != fileStats.st_mtime) {
                lastModifiedTime = fileStats.st_mtime;
                mtx.lock();
                resultFileUpdated = true;
                mtx.unlock();
            }
        }
    }
}

int main() {
    string surname;
    float height, weight;
    thread monitorThread(monitorResultFile);
    monitorThread.detach();

    ofstream serverFile(SERVER_FILE, ios::app);
    if (!serverFile.is_open()) {
        cerr << "Failed to open server file!" << endl;
        return 1;
    }

    while (true) {
        cout << "Enter the student's surname (or type 'e' to quit): ";
        cin >> surname;
        if (surname == "e") {
            break;
        }

        cout << "Enter height (in cm): ";
        cin >> height;
        cout << "Enter weight (in kg): ";
        cin >> weight;

        serverFile << surname << " " << height << " " << weight << endl;
        serverFile.flush();

        evaluateDevelopment(weight, height);
        string result = evaluateDevelopment(weight, height);
        cout << "Student: " << surname << ", Result: " << result << endl;
        mtx.lock();
        if (resultFileUpdated) {
            cout << "The result file has been updated" << endl;
            displayNewResults();
            resultFileUpdated = false;
        }
        mtx.unlock();
    }

    serverFile.close();
    return 0;
}