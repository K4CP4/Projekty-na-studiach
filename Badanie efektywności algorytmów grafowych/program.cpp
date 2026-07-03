#include <iostream>
#include <fstream>
#include <set>
#include <ctime>
#include <vector>
#include <queue>
using namespace std;

string current_file = "0";              // globalna zmienna na nazwę pliku grafu
const int infinity = 1000000000;        // duża liczba używana jako nieskończoność
bool open_file(const string& filename, ifstream& plik);     // deklaracja funkcji otwierającej plik

bool open_file(const string& filename, ifstream& plik) {
    plik.open(filename.c_str());
    if (!plik) {
        cout << "Nie mozna otworzyc pliku!" << endl;
        return false;
    }
    return true;
}

void load_file() {
    cout << "Podaj nazwę pliku z grafem: ";
    string filename;
    cin >> filename;
    ifstream testfile(filename.c_str());
    if (!testfile) {
        cout << "Plik " << filename << " nie istnieje lub nie można go otworzyć" << endl;
        return;
    }
    current_file = filename;
    cout << "Wczytano plik: " << current_file << endl;
    testfile.close();
}

void generate_graph(int n, double density, bool silent) {
    int max_edges = n * (n - 1);        // maksymalna liczba krawędzi bez pętli
    int m = density * max_edges + 0.5;  // liczba krawędzi zaokrąglona (od 0.5 w górę)
    if (m < n - 1) m = n - 1;           // zapewniamy spójność grafu
    int min_weight = rand() % 5 + 1;    // minimalna waga: 1-5
    int max_weight = min_weight + rand() % 21 + 5;      // maksymalna waga: min+5 do min+25

    ofstream plik("graf.txt", ios::trunc);
    if (!plik) {
        cout << "Nie mozna otworzyc pliku" << endl;
        return;
    }
    plik << n << " " << m << "\n";
    set<pair<int, int>> edges;          // zbiór krawędzi (u, v) aby uniknąć duplikatów i pętli
    while ((int)edges.size() < m) {
        int u = rand() % n;             // wierzchołek początkowy (od 0 do n-1)
        int v = rand() % n;             // wierzchołek końcowy (od 0 do n-1)
        if (u == v) continue;           // pomijanie pętli
        if (edges.count({u, v})) continue;      // sprawdzanie, czy krawędź już istnieje
        int w = rand() % (max_weight - min_weight + 1) + min_weight;    // waga krawędzi
        edges.insert({u, v});
        plik << u << " " << v << " " << w << "\n";
    }
    plik.close();
    if (!silent) cout << "Wygenerowano losowy graf i zapisano do pliku graf.txt" << endl;
}

void list(const string& filename, vector<pair<int, int>>*& adj, int& n, int& m, bool show) {
    ifstream plik;
    if (!open_file(filename, plik)) return;
    plik >> n >> m;
    adj = new vector<pair<int, int>>[n];
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        plik >> u >> v >> w;
        adj[u].push_back({v, w});
    }

    if (show) {
        cout << "Reprezentacja listowa:" << endl;
        for (int i = 0; i < n; ++i) {
            cout << i << ": ";
            for (auto& p : adj[i]) {
                cout << "(" << p.first << ", " << p.second << ") ";
            }
            cout << endl;
        }
    }
}

void matrix(const string& filename, int**& A, int &n, int &m, bool show) {
    ifstream plik;
    if (!open_file(filename, plik)) return;
    plik >> n >> m;
    // dynamiczne tworzenie listy sąsiedztwa
    A = new int*[n];
    for (int i = 0; i < n; ++i) A[i] = new int[n];
    // wczytywanie danych z pliku
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            A[i][j] = 0;
    for (int i = 0; i < m; ++i) {
        int u, v, w;
        plik >> u >> v >> w;
        A[u][v] = w;
    }

    if (show) {
        cout << "Reprezentacja macierzowa:" << endl;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                cout << A[i][j] << " ";
            }
            cout << endl;
        }
    }
}

void dijkstra_list(const string& filename, int start, int end, bool silent) {
    int n, m;                           // n - liczba wierzchołków, m - liczba krawędzi
    bool show = false;                  // zmienna logiczna do sterowania wyświetlaniem grafu
    vector<pair<int, int>>* adj;        // adj - lista sąsiedztwa
    list(filename, adj, n, m, show);
    if (n == 0) return;
    if (start < 0 || start >= n || end < 0 || end >= n) {
        if (!silent) cout << "Podany wierzchołek nie istnieje w grafie!" << endl;
        delete[] adj;
        return;
    }

    vector<int> dist(n, infinity);      // dist - najkrótsze odległości
    vector<int> prev(n, -1);            // prev - poprzednik na ścieżce
    vector<bool> visited(n, false);     // visited - czy wierzchołek był odwiedzony

    // kolejka priorytetowa (odległość, wierzchołek)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (visited[u]) continue;
        visited[u] = true;
        for (auto& edge : adj[u]) {
            int v = edge.first;
            int w = edge.second;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (!silent) {
        if (dist[end] == infinity) {
            cout << "Brak ścieżki z " << start << " do " << end << endl;
        } else {
            cout << "Najkrótsza odległość: " << dist[end] << endl;
            cout << "Sciezka: ";
            vector<int> path;
            for (int v = end; v != -1; v = prev[v]) path.push_back(v);
            for (int i = path.size() - 1; i >= 0; --i) {
                cout << path[i] << (i ? " -> " : "\n");
            }
        }
    }
    delete[] adj;
}

void dijkstra_matrix(const string& filename, int start, int end, bool silent) {
    int n, m;                           // n - liczba wierzchołków, m - liczba krawędzi
    int** A;                            // A - macierz sąsiedztwa
    bool show = false;                  // zmienna logiczna do sterowania wyświetlaniem grafu
    matrix(filename, A, n, m, show);    // wczytaj i wyświetl macierz sąsiedztwa
    if (n == 0) return;                 // pusty graf
    // sprawdzenie poprawności wierzchołków
    if (start < 0 || start >= n || end < 0 || end >= n) {
        cout << "Podany wierzchołek nie istnieje w grafie!" << endl;
        return;
    }

    int* dist = new int[n];             // dist - najkrótsze odległości
    int* prev = new int[n];             // prev - poprzednik na ścieżce
    int* visited = new int[n];          // visited - czy wierzchołek był odwiedzony
    for (int i = 0; i < n; ++i) {
        dist[i] = infinity;
        prev[i] = -1;
        visited[i] = 0;
    }
    dist[start] = 0;
    for (int i = 0; i < n; ++i) {
        int u = -1, min_dist = infinity;
        for (int j = 0; j < n; ++j)
            if (!visited[j] && dist[j] < min_dist) {
                min_dist = dist[j];
                u = j;
            }
        if (u == -1) break;
        visited[u] = 1;
        for (int v = 0; v < n; ++v) {
            if (A[u][v] && dist[v] > dist[u] + A[u][v]) {
                dist[v] = dist[u] + A[u][v];
                prev[v] = u;
            }
        }
    }

    if (!silent) {
        if (dist[end] == infinity) {
            cout << "Brak ścieżki z " << start << " do " << end << endl;
        } else {
            cout << "Najkrótsza odległość: " << dist[end] << endl;
            cout << "Sciezka: ";
            int path[21], len = 0, v = end;
            while (v != -1) {
                path[len++] = v;
                v = prev[v];
            }
            for (int i = len-1; i >= 0; --i) {
                cout << path[i] << (i ? " -> " : "\n");
            }
        }
    }
    // zwolnienie pamięci
    for (int i = 0; i < n; ++i) delete[] A[i];
    delete[] A;
    delete[] dist;
    delete[] prev;
    delete[] visited;
}

int main() {
    srand(time(NULL));                                  // inicjalizacja generatora liczb losowych
    int choice;                                         // wybór opcji w menu
    int n[5] = {10, 50, 100, 500, 1000};                // tablica liczby wierzchołków
    double density[5] = {0.1, 0.25, 0.50, 0.75, 1.00};  // tablica gęstości grafu
    bool show;                                          // zmienna logiczna do sterowania wyświetlaniem grafów
    bool silent;                                        // zmienna logiczna do sterowania wyświetlaniem komunikatów

    do {
        cout << "============== MENU ==============" << endl;
        cout << "0. Wyjscie" << endl;
        cout << "1. Generuj losowy graf" << endl;
        cout << "2. Wczytaj plik z grafem" << endl;
        cout << "3. Wyswietl graf (lista i macierz)" << endl;
        cout << "4. Dijkstra (lista i macierz)" << endl;
        cout << "5. Testowanie" << endl;
        cout << "Wybor: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
            case 0: {
                cout << "Koniec programu." << endl;
                break;
            }
            case 1: {
                silent = true;
                cout << "Podaj liczbe wierzcholkow (domyslnie 10): ";
                cin >> n[0];
                cout << "Podaj gestosc grafu (domyslnie 0.25): ";
                cin >> density[0];
                generate_graph(n[0], density[0], silent);
                break;
            }
            case 2: {
                load_file();
                break;
            }
            case 3: {
                show = true;
                silent = true;
                if (current_file == "0") {
                    generate_graph(n[0], density[0], silent);
                    cout << endl;
                    current_file = "graf.txt";
                }
                {
                    int n, m;                       // n - liczba wierzchołków, m - liczba krawędzi
                    vector<pair<int, int>>* adj;    // adj - lista sąsiedztwa
                    list(current_file, adj, n, m, show);
                    delete[] adj;
                }
                cout << endl;
                {
                    int n, m;                       // n - liczba wierzchołków, m - liczba krawędzi
                    int** A;                        // A - macierz sąsiedztwa
                    matrix(current_file, A, n, m, show);
                }
                break;
            }
            case 4: {
                if (current_file == "0") {
                    generate_graph(n[0], density[0], silent = true);
                    cout << endl;
                    current_file = "graf.txt";
                }
                ifstream plik;
                if (!open_file(current_file, plik)) {
                    break;
                }
                int n = 0;
                plik >> n;
                cout << "Zakres numerów wierzchołków: 0 - " << (n-1) << endl;
                int start, end;
                cout << "Podaj wierzcholek poczatkowy: ";
                cin >> start;
                cout << "Podaj wierzcholek koncowy: ";
                cin >> end;
                cout << "Wykonanie algorytmu Dijkstry dla listy:" << endl;
                clock_t t1 = clock();
                dijkstra_list(current_file, start, end, silent = false);
                clock_t t2 = clock();
                cout << "Czas wykonania algorytmu Dijkstry: " << (double)(t2 - t1) * 1000 / CLOCKS_PER_SEC << "ms" << endl;
                cout << endl;
                cout << "Wykonanie algorytmu Dijkstry dla macierzy:" << endl;
                clock_t t11 = clock();
                dijkstra_matrix(current_file, start, end, silent = false);
                clock_t t22 = clock();
                cout << "Czas wykonania algorytmu Dijkstry: " << (double)(t22 - t11) * 1000 / CLOCKS_PER_SEC << "ms" << endl;
                cout << endl;
                break;
            }
            case 5: {
                silent = true;
                int num_tests = 20;             // liczba powtórzeń

                cout << "Testowanie algorytmu Dijkstry dla listy:" << endl;
                for (int ni = 0; ni < 5; ni++) {
                    for (int di = 0; di < 5; di++) {
                        double total_time = 0;
                        for (int t = 0; t < num_tests; t++) {
                            int start = rand() % n[ni];
                            int end = rand() % n[ni];
                            generate_graph(n[ni], density[di], silent);
                            clock_t t1 = clock();
                            dijkstra_list("graf.txt", start, end, silent);
                            clock_t t2 = clock();
                            total_time += (double)(t2 - t1) * 1000 / CLOCKS_PER_SEC;
                        }
                        cout << "n: " << n[ni] << ", density: " << density[di] << ", czas(srednia): " << (total_time / num_tests) << "ms" << endl;
                    }
                }

                cout << endl << "Testowanie algorytmu Dijkstry dla macierza:" << endl;
                for (int ni = 0; ni < 5; ni++) {
                    for (int di = 0; di < 5; di++) {
                        double total_time = 0;
                        for (int t = 0; t < num_tests; t++) {
                            int start = rand() % n[ni];
                            int end = rand() % n[ni];
                            generate_graph(n[ni], density[di], silent);
                            clock_t t1 = clock();
                            dijkstra_matrix("graf.txt", start, end, silent);
                            clock_t t2 = clock();
                            total_time += (double)(t2 - t1) * 1000 / CLOCKS_PER_SEC;
                        }
                        cout << "n: " << n[ni] << ", density: " << density[di] << ", czas(srednia): " << (total_time / num_tests) << "ms" << endl;
                    }
                }
                break;
            }
            default:
                cout << "Nieprawidlowy wybor" << endl;
        }
        cout << endl;
    } while (choice != 0);
    return 0;
}
