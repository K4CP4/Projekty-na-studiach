#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>
#include <algorithm>
#include <cmath>

std::vector<int> path_single;
std::vector<int> path_double;

using std::cout;
using std::vector;
using std::pair;
using std::make_pair;
using std::priority_queue;
using std::greater;
using std::thread;
using std::mutex;
using std::lock_guard;
using std::chrono::high_resolution_clock;

int INF = 2000000000;                   // reprezentacja nieskończoności

vector<vector<int>> generateGraph(int n, double d, bool show_graph, bool grid_mode) {
    vector<vector<int>> adj_matrix(n, vector<int>(n, INF));
    for (int i = 0; i < n; ++i) adj_matrix[i][i] = 0;
    int max_edges = n * (n - 1);                     // maksymalna liczba krawędzi -> graf skierowany, bez pętli
    int m = (int)(d * max_edges + 0.5);              // m - liczba krawędzi
    vector<bool> edge_exists(n * n, false);
    int added = 0;                      // licznik dodanych krawędzi
    // generator losowy siatki
    if (grid_mode) {
        // wymiary siatki możliwie zbliżone do kwadratu
        int rows = (int)std::floor(std::sqrt((double)n));
        if (rows < 1) rows = 1;
        int cols = (int)std::ceil((double)n / rows);
        // krawędzie siatki: tylko w prawo i w dół (skierowane), wagi 1..n
        for (int id = 0; id < n; ++id) {
            int r = id / cols;
            int c = id % cols;
            // w prawo
            if (c + 1 < cols) {
                int v = id + 1;
                if (v < n) {
                    int idx = id * n + v;
                    if (!edge_exists[idx]) {
                        edge_exists[idx] = true;
                        int waga = 1 + (rand() % n);
                        adj_matrix[id][v] = waga;
                        ++added;
                    }
                }
            }
            // w dół
            if (r + 1 < rows) {
                int v = id + cols;
                if (v < n) {
                    int idx = id * n + v;
                    if (!edge_exists[idx]) {
                        edge_exists[idx] = true;
                        int waga = 1 + (rand() % n);
                        adj_matrix[id][v] = waga;
                        ++added;
                    }
                }
            }
        }
        // dobijanie gęstości losowymi krawędziami o większych wagach
        while (added < m) {
            int u = rand() % n;
            int v = rand() % n;
            if (u == v) continue;
            int idx = u * n + v;
            if (edge_exists[idx]) continue;
            edge_exists[idx] = true;
            int waga2 = (n * n) + (rand() % (n * n * n));
            adj_matrix[u][v] = waga2;
            ++added;
        }
    } else {
        // poprzedni generator losowy
        while (added < m) {
            int u = rand() % n;             // losowy wierzchołek początkowy
            int v = rand() % n;             // losowy wierzchołek końcowy
            if (u == v) continue;
            int idx = u * n + v;
            if (edge_exists[idx]) continue;
            edge_exists[idx] = true;
            int waga = 1 + (rand() % n);    // losowa waga krawędzi z zakresu [1, n]
            adj_matrix[u][v] = waga;
            ++added;
        }
    }
    if (show_graph) {
        cout << "Wygenerowany graf (n=" << n << ", m=" << m << "):\n";
        for (int u = 0; u < n; ++u) {
            cout << u << ":";
            for (int v = 0; v < n; ++v) {
                if (u != v && adj_matrix[u][v] < INF) {
                    cout << " (" << v << "," << adj_matrix[u][v] << ")";
                }
            }
            cout << "\n";
        }
    }
    return adj_matrix;
}

// rekonstrukcja ścieżki
static void reconstruct_path(vector<int>& parent_start, vector<int>& parent_end, int meet, vector<int>& out_path) {
    out_path.clear();
    if (meet == -1) return;
    vector<int> left, right;
    int v = meet;
    while (v != -1) { 
        left.push_back(v); v = parent_start[v]; 
    }
    reverse(left.begin(), left.end());
    v = parent_end[meet];
    while (v != -1) { 
        right.push_back(v); v = parent_end[v]; 
    }
    out_path = left;
    out_path.insert(out_path.end(), right.begin(), right.end());
}

int dijkstraSingle(vector<vector<int>> &adj_matrix, int start, int end) {
    int n = (int)adj_matrix.size();
    // odwrócony graf jako macierz
    vector<vector<int>> rev_matrix(n, vector<int>(n, INF));
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (adj_matrix[u][v] < INF) rev_matrix[v][u] = adj_matrix[u][v];
        }
    }
    vector<int> dist_start(n, INF), dist_end(n, INF);
    vector<int> parent_start(n, -1), parent_end(n, -1);
    dist_start[start] = 0;
    dist_end[end] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq_start, pq_end;
    pq_start.push(make_pair(0, start));
    pq_end.push(make_pair(0, end));
    int best = INF, meet = -1;
    while (true) {
        if (pq_start.empty() && pq_end.empty()) break;
        // krok od początku
        if (!pq_start.empty()) {
            int d = pq_start.top().first;
            int u = pq_start.top().second;
            pq_start.pop();
            if (d <= dist_start[u]) {
                // aktualizacja best/meet na przecięciu
                if (dist_end[u] < INF) {
                    int candidate = dist_start[u] + dist_end[u];
                    if (candidate < best) { best = candidate; meet = u; }
                }
                for (int v = 0; v < n; ++v) {
                    int w = adj_matrix[u][v];
                    if (w < INF && dist_start[v] > dist_start[u] + w) {
                        dist_start[v] = dist_start[u] + w;
                        parent_start[v] = u;
                        pq_start.push(make_pair(dist_start[v], v));
                        if (dist_end[v] < INF) {
                            int candidate2 = dist_start[v] + dist_end[v];
                            if (candidate2 < best) { best = candidate2; meet = v; }
                        }
                    }
                }
            }
        }
        // krok od końca
        if (!pq_end.empty()) {
            int d = pq_end.top().first;
            int u = pq_end.top().second;
            pq_end.pop();
            if (d <= dist_end[u]) {
                if (dist_start[u] < INF) {
                    int candidate = dist_end[u] + dist_start[u];
                    if (candidate < best) { best = candidate; meet = u; }
                }
                for (int v = 0; v < n; ++v) {
                    int w = rev_matrix[u][v];
                    if (w < INF && dist_end[v] > dist_end[u] + w) {
                        dist_end[v] = dist_end[u] + w;
                        parent_end[v] = u;
                        pq_end.push(make_pair(dist_end[v], v));
                        if (dist_start[v] < INF) {
                            int candidate2 = dist_end[v] + dist_start[v];
                            if (candidate2 < best) { best = candidate2; meet = v; }
                        }
                    }
                }
            }
        }
        // warunek szybkiego zakończenia
        bool canImproveStart = !pq_start.empty() && pq_start.top().first < best;
        bool canImproveEnd   = !pq_end.empty()   && pq_end.top().first   < best;
        if (!canImproveStart && !canImproveEnd) break;
    }
    reconstruct_path(parent_start, parent_end, meet, path_single);
    return best < INF ? best : INF;
}

int dijkstraDouble(vector<vector<int>> &adj_matrix, int start, int end) {
    int n = (int)adj_matrix.size();
    // odwrócony graf jako macierz
    vector<vector<int>> rev_matrix(n, vector<int>(n, INF));
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (adj_matrix[u][v] < INF) rev_matrix[v][u] = adj_matrix[u][v];
        }
    }
    vector<int> dist_start(n, INF), dist_end(n, INF);
    vector<int> parent_start(n, -1), parent_end(n, -1);
    dist_start[start] = 0;
    dist_end[end] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq_start, pq_end;
    pq_start.push(make_pair(0, start));
    pq_end.push(make_pair(0, end));
    // wspólne zmienne
    int best = INF;
    int meet = -1;
    bool stop = false;
    mutex mtx_shared;
    auto search = [&](vector<int> &dist, vector<int> &parent, priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> &pq, vector<vector<int>> &G, const vector<int> &otherDist) {
        int nLocal = (int)G.size();
        while (true) {
            {
                lock_guard<mutex> lk(mtx_shared);
                if (stop) break;
            }
            if (pq.empty()) break;
            int d = pq.top().first;
            int u = pq.top().second;
            pq.pop();
            if (d > dist[u]) continue;
            {
                lock_guard<mutex> lk(mtx_shared);
                if (otherDist[u] < INF) {
                    int candidate = dist[u] + otherDist[u];
                    if (candidate < best) {
                        best = candidate;
                        meet = u;
                    }
                }
            }
            for (int v = 0; v < nLocal; ++v) {
                int w = G[u][v];
                if (w < INF && dist[v] > dist[u] + w) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    pq.push(make_pair(dist[v], v));
                    {
                        lock_guard<mutex> lk(mtx_shared);
                        if (otherDist[v] < INF) {
                            int candidate = dist[v] + otherDist[v];
                            if (candidate < best) {
                                best = candidate;
                                meet = v;
                            }
                        }
                    }
                }
            }
            {
                lock_guard<mutex> lk(mtx_shared);
                if (!pq.empty() && pq.top().first >= best) {
                    stop = true;
                }
            }
        }
    };
    // uruchomienie wątków
    thread t1(search, ref(dist_start), ref(parent_start), ref(pq_start), ref(adj_matrix), ref(dist_end));
    thread t2(search, ref(dist_end), ref(parent_end), ref(pq_end), ref(rev_matrix), ref(dist_start));
    t1.join();
    t2.join();
    reconstruct_path(parent_start, parent_end, meet, path_double);
    return best < INF ? best : INF;
}

double measureTime(const std::function<void()>& func) {
    auto t1 = high_resolution_clock::now();
    func();
    auto t2 = high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
}

int main() {
    srand((unsigned)time(nullptr));   // ziarno dla generatora liczb losowych
    bool show_graph = false;          // drukowanie grafu
    bool show_path = false;           // wypisanie ścieżki i odległości
    const int Ns[] = {500, 1000, 5000, 10000, 20000};
    const double Ds[] = {0.01, 0.25, 0.5, 0.75, 0.9};
    const int numN = sizeof(Ns) / sizeof(Ns[0]);
    const int numD = sizeof(Ds) / sizeof(Ds[0]);
    const int trials = 10;
    for (int iN = 0; iN < numN; ++iN) {
        int n = Ns[iN];
        for (int iD = 0; iD < numD; ++iD) {
            double d = Ds[iD];
            double sum_single = 0.0;
            double sum_double = 0.0;
            int good_trials = 0;
            int attempts = 0;
            while (good_trials < trials) {
                ++attempts;
                bool grid_mode = true; // tryb siatki
                vector<vector<int>> adj_matrix = generateGraph(n, d, show_graph, grid_mode);
                int start, end;
                if (grid_mode) {
                    start = 0;
                    end = n - 1;
                } else {
                    start = rand() % n;
                    end = rand() % n;
                    while (end == start) end = rand() % n;
                }
                int dist_single;
                double time_single = measureTime([&]() {
                    dist_single = dijkstraSingle(adj_matrix, start, end);
                });
                int dist_double;
                double time_double = measureTime([&]() {
                    dist_double = dijkstraDouble(adj_matrix, start, end);
                });
                if (dist_single == dist_double) {
                    sum_single += time_single;
                    sum_double += time_double;
                    ++good_trials;
                    if (show_path) {
                        cout << "[Proba " << (good_trials) << "] n=" << n << ", d=" << d << ", start=" << start << ", end=" << end << "\n";
                        cout << "Jednowatkowy: odleglosc = ";
                        if (dist_single == INF) {
                            cout << "brak sciezki";
                        } else {
                            cout << dist_single << ", sciezka: ";
                            for (size_t i = 0; i < path_single.size(); ++i) {
                                cout << path_single[i];
                                if (i + 1 < path_single.size()) cout << " -> ";
                            }
                        }
                        cout << "\n";
                        cout << "Dwuwatkowy: odleglosc= ";
                        if (dist_double == INF) {
                            cout << "brak sciezki";
                        } else {
                            cout << dist_double << ", sciezka: ";
                            for (size_t i = 0; i < path_double.size(); ++i) {
                                cout << path_double[i];
                                if (i + 1 < path_double.size()) cout << " -> ";
                            }
                        }
                        cout << "\n";
                        cout << "Czas jednowatkowy = " << time_single << " ms\n";
                        cout << "Czas dwuwatkowy = " << time_double << " ms\n";
                    }
                } else {
                    if (show_path) {
                        cout << "[Proba odrzucona]\n";
                    }
                }
            }
            double avg_single = sum_single / trials;
            double avg_double = sum_double / trials;
            double proportion = (double)((int)(avg_single/avg_double * 100.0)) / 100.0;
            cout << "n=" << n << ", d=" << d 
                 << ": sredni czas jednowatkowy = " << avg_single << " ms, "
                 << "dwuwatkowy = " << avg_double << " ms, roznica: " << proportion << "\n";
        }
    }
    return 0;
}
