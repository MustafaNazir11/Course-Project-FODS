#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <limits>
#include <chrono>
#include <fstream> // for CSV file writing

using namespace std;

struct Player
{
    string name;
    int score;
};

// ===================== File Handling =====================
void saveToCSV(const vector<Player> &lb, const string &algoName)
{
    ofstream file("leaderboard.csv");
    if (!file)
    {
        cout << " Error: Could not open leaderboard.csv\n";
        return;
    }
    file << "Rank,Name,Score,Algorithm\n";
    for (int i = 0; i < lb.size(); i++)
    {
        file << i + 1 << "," << lb[i].name << "," << lb[i].score << "," << algoName << "\n";
    }
    file.close();
    cout << " Leaderboard saved to 'leaderboard.csv' (open in Excel)\n";
}

// ===================== Display Function =====================
void displayLeaderboard(const vector<Player> &lb, const string &algoName, double timeTaken = -1)
{
    cout << "\n==============================\n";
    cout << "   " << algoName << " LEADERBOARD\n";
    cout << "==============================\n";
    cout << left << setw(6) << "Rank" << setw(20) << "Name" << setw(10) << "Score" << endl;
    cout << "------------------------------\n";
    for (int i = 0; i < lb.size(); i++)
    {
        cout << left << setw(6) << i + 1
             << setw(20) << lb[i].name
             << setw(10) << lb[i].score << endl;
    }
    cout << "==============================\n";
    if (timeTaken >= 0)
    {
        cout << "Time taken: " << fixed << setprecision(3) << timeTaken << " ms\n";
    }
    saveToCSV(lb, algoName);
}

// ===================== Sorting Algorithms =====================
void bubbleSort(vector<Player> &lb)
{
    int n = lb.size();
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (lb[j].score < lb[j + 1].score)
                swap(lb[j], lb[j + 1]);
        }
    }
}

void insertionSort(vector<Player> &lb)
{
    for (int i = 1; i < lb.size(); i++)
    {
        Player key = lb[i];
        int j = i - 1;
        while (j >= 0 && lb[j].score < key.score)
        {
            lb[j + 1] = lb[j];
            j--;
        }
        lb[j + 1] = key;
    }
}

void selectionSort(vector<Player> &lb)
{
    int n = lb.size();
    for (int i = 0; i < n - 1; i++)
    {
        int maxIndex = i;
        for (int j = i + 1; j < n; j++)
        {
            if (lb[j].score > lb[maxIndex].score)
                maxIndex = j;
        }
        swap(lb[i], lb[maxIndex]);
    }
}

void merge(vector<Player> &lb, int left, int mid, int right)
{
    int n1 = mid - left + 1, n2 = right - mid;
    vector<Player> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = lb[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = lb[mid + 1 + j];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2)
    {
        if (L[i].score >= R[j].score)
            lb[k++] = L[i++];
        else
            lb[k++] = R[j++];
    }
    while (i < n1)
        lb[k++] = L[i++];
    while (j < n2)
        lb[k++] = R[j++];
}

void mergeSort(vector<Player> &lb, int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        mergeSort(lb, left, mid);
        mergeSort(lb, mid + 1, right);
        merge(lb, left, mid, right);
    }
}

int partition(vector<Player> &lb, int low, int high)
{
    int pivot = lb[high].score;
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (lb[j].score >= pivot)
        {
            i++;
            swap(lb[i], lb[j]);
        }
    }
    swap(lb[i + 1], lb[high]);
    return i + 1;
}

void quickSort(vector<Player> &lb, int low, int high)
{
    if (low < high)
    {
        int pi = partition(lb, low, high);
        quickSort(lb, low, pi - 1);
        quickSort(lb, pi + 1, high);
    }
}

// ===================== Duplicate Name Handling =====================
string generateUniqueName(const string &name, const vector<Player> &lb)
{
    int count = 0;
    for (const auto &p : lb)
    {
        if (p.name == name || p.name.find(name + " (") == 0)
            count++;
    }
    if (count == 0)
        return name;
    else
        return name + " (" + to_string(count) + ")";
}

// ===================== Main =====================
int main()
{
    vector<Player> leaderboard;
    int choice;

    while (true)
    {
        cout << "\n===== LEADERBOARD MENU =====\n";
        cout << "1. Add Player\n";
        cout << "2. Show Leaderboard (choose algorithm)\n";
        cout << "3. Show All Sorting Algorithms (with time)\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 1)
        {
            Player p;
            cout << "Enter player name: ";
            getline(cin, p.name);
            p.name = generateUniqueName(p.name, leaderboard);

            cout << "Enter player score: ";
            cin >> p.score;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            leaderboard.push_back(p);
            saveToCSV(leaderboard, "Unsorted");

            cout << "✅ Player added as: " << p.name << endl;
        }

        else if (choice == 2)
        {
            if (leaderboard.empty())
            {
                cout << "Leaderboard is empty!\n";
                continue;
            }

            cout << "\nChoose sorting algorithm:\n";
            cout << "1. Bubble Sort\n2. Insertion Sort\n3. Selection Sort\n4. Merge Sort\n5. Quick Sort\n";
            int algo;
            cin >> algo;

            string algoName; // 🔹 store chosen algorithm name
            vector<Player> temp = leaderboard;
            auto start = chrono::high_resolution_clock::now();

            switch (algo)
            {
            case 1:
                algoName = "Bubble Sort";
                bubbleSort(temp);
                break;
            case 2:
                algoName = "Insertion Sort";
                insertionSort(temp);
                break;
            case 3:
                algoName = "Selection Sort";
                selectionSort(temp);
                break;
            case 4:
                algoName = "Merge Sort";
                mergeSort(temp, 0, temp.size() - 1);
                break;
            case 5:
                algoName = "Quick Sort";
                quickSort(temp, 0, temp.size() - 1);
                break;
            default:
                cout << "Invalid choice!\n";
                continue;
            }

            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            // 🔹 Display sorted leaderboard with algorithm name
            displayLeaderboard(temp, algoName, duration);

            // 🔹 Save *unsorted* leaderboard with algorithm info
            saveToCSV(temp, algoName);
        }

        else if (choice == 3)
        {
            if (leaderboard.empty())
            {
                cout << "Leaderboard is empty!\n";
                continue;
            }

            vector<Player> temp;
            auto start = chrono::high_resolution_clock::now();
            auto end = start;
            double duration = 0.0;

            temp = leaderboard;
            start = chrono::high_resolution_clock::now();
            bubbleSort(temp);
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration<double, milli>(end - start).count();
            displayLeaderboard(temp, "Bubble Sort", duration);

            temp = leaderboard;
            start = chrono::high_resolution_clock::now();
            insertionSort(temp);
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration<double, milli>(end - start).count();
            displayLeaderboard(temp, "Insertion Sort", duration);

            temp = leaderboard;
            start = chrono::high_resolution_clock::now();
            selectionSort(temp);
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration<double, milli>(end - start).count();
            displayLeaderboard(temp, "Selection Sort", duration);

            temp = leaderboard;
            start = chrono::high_resolution_clock::now();
            mergeSort(temp, 0, temp.size() - 1);
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration<double, milli>(end - start).count();
            displayLeaderboard(temp, "Merge Sort", duration);

            temp = leaderboard;
            start = chrono::high_resolution_clock::now();
            quickSort(temp, 0, temp.size() - 1);
            end = chrono::high_resolution_clock::now();
            duration = chrono::duration<double, milli>(end - start).count();
            displayLeaderboard(temp, "Quick Sort", duration);
        }

        else if (choice == 4)
        {
            cout << "Exiting... Goodbye!\n";
            break;
        }

        else
        {
            cout << "Invalid option, try again!\n";
        }
    }

    return 0;
}
