#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <string>
#include <limits>
#include <chrono>
#include <fstream> // for CSV file writing
#include <algorithm>
#include <filesystem>

using namespace std;

struct Player
{
    string name;
    int score;
};

struct ComparisonResult
{
    string algorithm;
    double timeTaken;
    string complexity;
};

// -------------------- File handling: CSV helpers --------------------

// Append leaderboard rows to leaderboard.csv (creates file if missing).
void saveToCSV(const vector<Player> &lb, const string &algoName)
{
    bool fileExists = filesystem::exists("leaderboard.csv");

    ofstream file("leaderboard.csv", ios::app); // append mode
    if (!file)
    {
        cout << "Error: Could not open leaderboard.csv\n";
        return;
    }

    // Write header only once
    if (!fileExists)
        file << "Rank,Name,Score,Algorithm\n";

    int startRank = 1;
    ifstream countFile("leaderboard.csv");
    string line;
    while (getline(countFile, line))
        startRank++;
    countFile.close();

    for (int i = 0; i < lb.size(); i++)
    {
        file << startRank + i << "," << lb[i].name << "," << lb[i].score << "," << algoName << "\n";
    }

    file.close();
    cout << "Leaderboard updated and saved to 'leaderboard.csv'\n";
}

// Ensure the main players CSV exists; create if missing.
void ensureCSVExists(const string &filename = "players.csv")
{
    ifstream infile(filename);
    if (!infile.good())
    {
        ofstream outfile(filename);
        outfile.close();
        cout << "Created new file: " << filename << "\n";
    }
}

// Load players from players.csv into the provided vector.
void loadPlayersFromCSV(vector<Player> &leaderboard, const string &filename = "players.csv")
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << ".\n";
        return;
    }

    string line;
    leaderboard.clear();
    while (getline(file, line))
    {
        if (line.empty())
            continue;
        stringstream ss(line);
        string name;
        int score;
        getline(ss, name, ',');
        ss >> score;
        leaderboard.push_back({name, score});
    }
    file.close();

    cout << "Loaded " << leaderboard.size() << " players from " << filename << ".\n";
}

// Append a single player record to players.csv.
void appendPlayerToCSV(const Player &p, const string &filename = "players.csv")
{
    ofstream file(filename, ios::app);
    if (!file.is_open())
    {
        cout << "Could not open " << filename << " for writing.\n";
        return;
    }
    file << p.name << "," << p.score << "\n";
    file.close();
}

// ------------------------------------------------------------------

// Prototype for complexity info used by display functions.
string getComplexityInfo(const string &algoName);

// -------------------- Display utilities --------------------

// Display leaderboard vector in a formatted table and optionally show time/complexity.
// Also saves the displayed leaderboard to CSV.
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
        cout << "Complexity: " << getComplexityInfo(algoName) << "\n";
    }

    saveToCSV(lb, algoName);
}

// -------------------- Sorting algorithms --------------------

// Bubble sort (descending by score).
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

// Insertion sort (descending by score).
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

// Selection sort (descending by score).
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

// Merge helper for merge sort.
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

// Merge sort (descending by score).
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

// Partition used by quick sort (descending by score).
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

// Quick sort (descending by score).
void quickSort(vector<Player> &lb, int low, int high)
{
    if (low < high)
    {
        int pi = partition(lb, low, high);
        quickSort(lb, low, pi - 1);
        quickSort(lb, pi + 1, high);
    }
}

// Heapify helper for heap sort.
void heapify(vector<Player> &arr, int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left].score > arr[largest].score)
        largest = left;

    if (right < n && arr[right].score > arr[largest].score)
        largest = right;

    if (largest != i)
    {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

// Heap sort (descending by score).
void heapSort(vector<Player> &arr)
{
    int n = arr.size();
    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements from heap
    for (int i = n - 1; i >= 0; i--)
    {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }

    // Reverse because we need descending (highest score first)
    reverse(arr.begin(), arr.end());
}

// Counting sort for integer score range (descending).
void countingSort(vector<Player> &arr)
{
    if (arr.empty())
        return;

    int maxScore = arr[0].score, minScore = arr[0].score;
    for (const auto &p : arr)
    {
        maxScore = max(maxScore, p.score);
        minScore = min(minScore, p.score);
    }

    int range = maxScore - minScore + 1;
    vector<int> count(range, 0);

    for (const auto &p : arr)
        count[p.score - minScore]++;

    for (int i = range - 2; i >= 0; i--)
        count[i] += count[i + 1]; // cumulative for descending

    vector<Player> output(arr.size());
    for (const auto &p : arr)
    {
        output[count[p.score - minScore] - 1] = p;
        count[p.score - minScore]--;
    }

    arr = output;
}

// -------------------- Searching --------------------

// Linear search by exact name.
int linearSearch(const vector<Player> &leaderboard, const string &name)
{
    for (int i = 0; i < leaderboard.size(); i++)
    {
        if (leaderboard[i].name == name)
            return i;
    }
    return -1; // not found
}

// Binary search on sorted-by-name vector.
int binarySearch(const vector<Player> &leaderboard, const string &name)
{
    int low = 0, high = leaderboard.size() - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        if (leaderboard[mid].name == name)
            return mid;
        else if (leaderboard[mid].name < name)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return -1; // not found
}

// -------------------- Utilities --------------------

// Create a unique name if duplicate exists (adds " (n)").
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

// Return complexity description for display.
string getComplexityInfo(const string &algo)
{
    if (algo == "Bubble Sort")
        return "Time: O(n^2), Space: O(1)";
    if (algo == "Insertion Sort")
        return "Time: O(n^2), Space: O(1)";
    if (algo == "Selection Sort")
        return "Time: O(n^2), Space: O(1)";
    if (algo == "Merge Sort")
        return "Time: O(n log n), Space: O(n)";
    if (algo == "Quick Sort")
        return "Time: O(n log n) , O(n^2) , Space: O(log n)";
    if (algo == "Heap Sort")
        return "Time: O(n log n), Space: O(1)";
    if (algo == "Counting Sort")
        return "Time: O(n + k), Space: O(n + k)";
    return "Unknown Complexity";
}

// -------------------- Comparison and visualization --------------------

// Compare all sorting algorithms by running them on a copy of the leaderboard,
// measuring execution time, printing a formatted table and a console bar graph.
void compareAllAlgorithms(vector<Player> &leaderboard)
{
    if (leaderboard.empty())
    {
        cout << "Leaderboard is empty! Please add players first.\n";
        return;
    }

    vector<ComparisonResult> results;
    vector<Player> temp;

    auto measureSortTime = [&](auto sortFunc, const string &algoName)
    {
        temp = leaderboard;
        auto start = chrono::high_resolution_clock::now();
        sortFunc(temp);
        auto end = chrono::high_resolution_clock::now();
        double duration = chrono::duration<double, milli>(end - start).count();
        results.push_back({algoName, duration, getComplexityInfo(algoName)});
    };

    // Measure all algorithms
    measureSortTime([](auto &arr)
                    { bubbleSort(arr); }, "Bubble Sort");
    measureSortTime([](auto &arr)
                    { insertionSort(arr); }, "Insertion Sort");
    measureSortTime([](auto &arr)
                    { selectionSort(arr); }, "Selection Sort");
    measureSortTime([](auto &arr)
                    { mergeSort(arr, 0, arr.size() - 1); }, "Merge Sort");
    measureSortTime([](auto &arr)
                    { quickSort(arr, 0, arr.size() - 1); }, "Quick Sort");
    measureSortTime([](auto &arr)
                    { heapSort(arr); }, "Heap Sort");
    measureSortTime([](auto &arr)
                    { countingSort(arr); }, "Counting Sort");

    // Print comparison table (aligned)
    cout << "\n==================== SORTING ALGORITHM COMPARISON ====================\n";
    cout << left << setw(20) << "Algorithm"
         << setw(15) << "Time (ms)"
         << "Complexity" << endl;
    cout << string(85, '-') << "\n";

    for (auto &r : results)
    {
        cout << left << setw(20) << r.algorithm
             << setw(15) << fixed << setprecision(3) << r.timeTaken;

        // Nicely aligned complexity text
        if (r.algorithm == "Bubble Sort")
            cout << "Time: O(n^2)              | Space: O(1)";
        else if (r.algorithm == "Insertion Sort")
            cout << "Time: O(n^2)              | Space: O(1)";
        else if (r.algorithm == "Selection Sort")
            cout << "Time: O(n^2)              | Space: O(1)";
        else if (r.algorithm == "Merge Sort")
            cout << "Time: O(n log n)          | Space: O(n)";
        else if (r.algorithm == "Quick Sort")
            cout << "Time: O(n log n) / O(n^2) | Space: O(log n)";
        else if (r.algorithm == "Heap Sort")
            cout << "Time: O(n log n)          | Space: O(1)";
        else if (r.algorithm == "Counting Sort")
            cout << "Time: O(n + k)            | Space: O(n + k)";
        else
            cout << r.complexity; // fallback

        cout << endl;
    }

    cout << string(85, '=') << "\n";

    // Console bar graph visualization
    cout << "\nExecution Time Visualization\n";
    cout << string(70, '-') << "\n";

    double maxTime = 0.0;
    for (auto &r : results)
        if (r.timeTaken > maxTime)
            maxTime = r.timeTaken;

    if (maxTime == 0.0)
        maxTime = 1.0; // prevent division by zero

    for (auto &r : results)
    {
        int barLength = static_cast<int>((r.timeTaken / maxTime) * 40);
        cout << left << setw(18) << r.algorithm << " | ";

        for (int i = 0; i < barLength; i++)
            cout << "_";

        cout << "  " << setw(7) << fixed << setprecision(3) << r.timeTaken << " ms" << endl;
    }

    cout << string(70, '-') << "\n";

    // Save comparison to CSV
    ofstream file("comparison.csv");
    if (file)
    {
        file << "Algorithm,Time(ms),Complexity\n";
        for (auto &r : results)
            file << r.algorithm << "," << r.timeTaken << "," << r.complexity << "\n";
        file.close();
        cout << "Comparison results saved to 'comparison.csv'\n";
    }
    else
    {
        cout << "Error: Could not save to comparison.csv\n";
    }
}

// -------------------- Main program & menu --------------------

int main()
{
    vector<Player> leaderboard;
    int choice;

    ensureCSVExists();
    loadPlayersFromCSV(leaderboard);

    while (true)
    {
        cout << "\n===== LEADERBOARD MENU =====\n";
        cout << "1. Add Player\n";
        cout << "2. Show Leaderboard (choose algorithm)\n";
        cout << "3. Show All Sorting Algorithms (with time)\n";
        cout << "4. Compare All Sorting Algorithms\n";
        cout << "5. Search Player (Linear / Binary)\n";
        cout << "6. Exit\n";

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

            // Save this player permanently to players.csv
            appendPlayerToCSV(p, "players.csv");

            // Also save full leaderboard for viewing/sorting
            saveToCSV(leaderboard, "Unsorted");

            cout << "Player added as: " << p.name << " and stored permanently.\n";
        }

        else if (choice == 2)
        {
            if (leaderboard.empty())
            {
                cout << "Leaderboard is empty!\n";
                continue;
            }

            cout << "\nChoose sorting algorithm:\n";
            cout << "1. Bubble Sort\n2. Insertion Sort\n3. Selection Sort\n4. Merge Sort\n5. Quick Sort\n6. Heap Sort\n7. Counting Sort\n";
            int algo;
            cin >> algo;

            string algoName; // holds chosen algorithm name
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
            case 6:
                algoName = "Heap Sort";
                heapSort(temp);
                break;
            case 7:
                algoName = "Counting Sort";
                countingSort(temp);
                break;
            default:
                cout << "Invalid choice!\n";
                continue;
            }

            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            // Display sorted leaderboard and save
            displayLeaderboard(temp, algoName, duration);

            // Save sorted leaderboard to CSV once more (if desired)
            //saveToCSV(temp, algoName);
        }

        else if (choice == 4)
        {
            compareAllAlgorithms(leaderboard);
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

        else if (choice == 5)
        {
            if (leaderboard.empty())
            {
                cout << "Leaderboard is empty!\n";
                continue;
            }

            string name;
            cout << "Enter player name to search: ";
            getline(cin, name);

            cout << "\nChoose search method:\n1. Linear Search\n2. Binary Search (alphabetically sorted)\n";
            int sChoice;
            cin >> sChoice;
            cin.ignore();

            int index = -1;
            auto start = chrono::high_resolution_clock::now();

            if (sChoice == 1)
            {
                index = linearSearch(leaderboard, name);
            }
            else if (sChoice == 2)
            {
                vector<Player> temp = leaderboard;
                sort(temp.begin(), temp.end(), [](auto &a, auto &b)
                     { return a.name < b.name; });
                index = binarySearch(temp, name);
            }

            auto end = chrono::high_resolution_clock::now();
            double duration = chrono::duration<double, milli>(end - start).count();

            if (index != -1)
                cout << "Player found! Name: " << name << " | Time: " << duration << " ms\n";
            else
                cout << "Player not found | Time: " << duration << " ms\n";
        }

        else if (choice == 6)
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
