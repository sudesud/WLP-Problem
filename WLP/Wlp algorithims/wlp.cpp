//tüm veri setinde aynı algoritmayı kullandım


#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <limits>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <map>
using namespace std;

struct Warehouse {
    int capacity;
    double setupCost;
    int remainingCapacity;
    bool opened;
};

struct Problem {
    int W, C;
    vector<Warehouse> warehouses;
    vector<int> demands;
    vector<vector<double>> cost;
};

struct Solution {
    vector<int> assignment;
    double totalCost;
};

Problem readInput(const string& filename) {
    Problem p;
    ifstream file(filename);
    file >> p.W >> p.C;

    p.warehouses.resize(p.W);
    for (int i = 0; i < p.W; ++i) {
        file >> p.warehouses[i].capacity >> p.warehouses[i].setupCost;
        p.warehouses[i].remainingCapacity = p.warehouses[i].capacity;
        p.warehouses[i].opened = false;
    }

    p.demands.resize(p.C);
    p.cost.resize(p.C, vector<double>(p.W));
    for (int i = 0; i < p.C; ++i) {
        file >> p.demands[i];
        for (int j = 0; j < p.W; ++j) {
            file >> p.cost[i][j];
        }
    }
    return p;
}

double evaluateSolution(const Solution& s, const Problem& p, double lambda = 0.2) {
    double total = 0.0;
    vector<bool> opened(p.W, false);
    vector<int> used(p.W, 0);

    for (int i = 0; i < p.C; ++i) {
        int w = s.assignment[i];
        total += p.cost[i][w];
        used[w] += p.demands[i];
        if (!opened[w]) {
            total += p.warehouses[w].setupCost;
            opened[w] = true;
        }
    }

    double imbalancePenalty = 0.0;
    for (int w = 0; w < p.W; ++w) {
        if (used[w] > 0) {
            double ratio = (double)used[w] / p.warehouses[w].capacity;
            imbalancePenalty += pow(ratio - 0.5, 2);
        }
    }

    total += lambda * imbalancePenalty;
    return total;
}

Solution greedyInitialSolution(const Problem& p, double lambda) {
    Solution s;
    s.assignment.resize(p.C, -1);
    s.totalCost = 0.0;
    vector<int> capacityLeft(p.W);
    vector<bool> opened(p.W, false);

    for (int i = 0; i < p.W; ++i)
        capacityLeft[i] = p.warehouses[i].capacity;

    for (int i = 0; i < p.C; ++i) {
        double minCost = numeric_limits<double>::max();
        int bestW = -1;

        for (int j = 0; j < p.W; ++j) {
            if (capacityLeft[j] >= p.demands[i]) {
                double cost = p.cost[i][j] + (opened[j] ? 0.0 : p.warehouses[j].setupCost);
                if (cost < minCost) {
                    minCost = cost;
                    bestW = j;
                }
            }
        }

        if (bestW != -1) {
            s.assignment[i] = bestW;
            capacityLeft[bestW] -= p.demands[i];
            opened[bestW] = true;
        }
    }

    s.totalCost = evaluateSolution(s, p, lambda);
    return s;
}

Solution localSearch(Solution s, const Problem& p, double lambda) {
    bool improved = true;

    while (improved) {
        improved = false;

        for (int i = 0; i < p.C; ++i) {
            int currentW = s.assignment[i];

            for (int w = 0; w < p.W; ++w) {
                if (w == currentW) continue;

                int used = 0;
                for (int j = 0; j < p.C; ++j)
                    if (s.assignment[j] == w)
                        used += p.demands[j];

                if (used + p.demands[i] > p.warehouses[w].capacity)
                    continue;

                int prev = s.assignment[i];
                s.assignment[i] = w;
                double newCost = evaluateSolution(s, p, lambda);
                if (newCost < s.totalCost) {
                    s.totalCost = newCost;
                    improved = true;
                    break;
                } else {
                    s.assignment[i] = prev;
                }
            }
        }
    }

    return s;
}

Solution perturbation(Solution s, const Problem& p, double lambda, int strength = 3) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> distClient(0, p.C - 1);
    uniform_int_distribution<> distWarehouse(0, p.W - 1);

    for (int k = 0; k < strength; ++k) {
        int i = distClient(gen);
        int newW = distWarehouse(gen);
        if (p.warehouses[newW].capacity >= p.demands[i]) {
            s.assignment[i] = newW;
        }
    }
    s.totalCost = evaluateSolution(s, p, lambda);
    return s;
}

Solution iteratedLocalSearch(const Problem& p, int maxIter = 100, double lambda = 0.2, int timeLimitSeconds = 60) {
    clock_t start = clock();
    Solution best = greedyInitialSolution(p, lambda);
    Solution current = best;

    for (int i = 0; i < maxIter; ++i) {
        double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;
        if (elapsed > timeLimitSeconds) break;

        Solution perturbed = perturbation(current, p, lambda);
        Solution improved = localSearch(perturbed, p, lambda);
        if (improved.totalCost < best.totalCost) {
            best = improved;
            cout << "Yeni en iyi çözüm (" << i << ". iterasyon): " << best.totalCost << endl;
        }
        current = improved;
    }

    return best;
}

void printWarehouseStats(const Problem& p, const Solution& s) {
    map<int, int> usedCapacity;
    map<int, int> customerCount;

    for (int i = 0; i < p.C; ++i) {
        int w = s.assignment[i];
        usedCapacity[w] += p.demands[i];
        customerCount[w]++;
    }

    cout << "\n📊 Depo Kullanım Dağılımı:" << endl;
    cout << "Depo\tKapasite\tKullanım\tMüşteri\tDolu %" << endl;
    for (const auto& [w, used] : usedCapacity) {
        int cap = p.warehouses[w].capacity;
        double ratio = (double)used / cap * 100.0;
        cout << w << "\t" << cap << "\t" << used << "\t" << customerCount[w] << "\t" << fixed << setprecision(1) << ratio << "%" << endl;
    }
}

int main() {
    Problem p = readInput("wl_500.txt");
    double lambda = 0.2;
    Solution best = iteratedLocalSearch(p, 500, lambda, 90);

    cout << "\n✅ En iyi maliyet: " << fixed << setprecision(2) << best.totalCost << endl;
    cout << "📤 Atamalar: ";
    for (int i = 0; i < p.C; ++i) {
        cout << best.assignment[i] << " ";
    }
    cout << endl;

    printWarehouseStats(p, best);

    ofstream outFile("wlp_output500-improved2.txt");
    outFile << best.totalCost << endl;
    for (int i = 0; i < p.C; ++i) {
        outFile << best.assignment[i];
        if (i != p.C - 1) outFile << " ";
    }
    outFile << endl;
    outFile.close();

    return 0;
}