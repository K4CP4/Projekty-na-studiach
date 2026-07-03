# Projekty na studiach

## Opis projektów

### 1. Badanie efektywności algorytmów grafowych
Zaimplementowano i porównano konkretnie dwie wersje algorytmu najkrótszej ścieżki Dijkstry:
1) `dijkstra_list` - wersja na liście sąsiedztwa z `priority_queue`,
2) `dijkstra_matrix` - wersja na macierzy sąsiedztwa z wyborem minimum w czasie O(V^2).
Do testów zaimplementowano też generator skierowanych, ważonych grafów oraz dwa sposoby reprezentacji danych (`list`, `matrix`). Pomiary wykonywano dla rozmiarów n = 10, 50, 100, 500, 1000 oraz gęstości 0.10, 0.25, 0.50, 0.75, 1.00, porównując czas obliczeń i poprawność wyznaczonej ścieżki.

Technologie i narzędzia: C++ (implementacja algorytmów i obsługa danych wejścia/wyjścia), kompilator g++ (budowanie programu), pliki tekstowe TXT (zestawy testowe i wyniki), podstawowe narzędzia terminalowe (uruchamianie testów i organizacja plików).

### 2. Badanie efektywności algorytmów sortowania
W projekcie przetestowano 5 konkretnych wariantów sortowania przez wstawianie (zgodnie z implementacją):
1) `sortA` - klasyczne insertion sort (przesuwanie w lewo od i-1),
2) `sortB` - insertion sort z liniowym wyszukaniem miejsca od początku tablicy,
3) `sortC` - insertion sort realizowane od końca tablicy (wariant z przesuwaniem w drugą stronę),
4) `sortD` - insertion sort od końca z przesuwaniem elementów mniejszych,
5) `sortE` - binary insertion sort (wyznaczanie pozycji przez wyszukiwanie binarne).
Każdy algorytm był uruchamiany na tych samych danych wejściowych i porównywany metrykami: czas [ms], liczba porównań i liczba przestawień. Testy wykonano dla rozmiarów 1000, 5000, 10000, 50000, 100000 oraz dla 3 typów danych: losowe, posortowane rosnąco, posortowane malejąco (dla losowych 10 powtórzeń i uśrednianie).

Technologie i narzędzia: C++ (implementacje algorytmów i pomiar czasu), STL (operacje na kontenerach i danych), g++ (kompilacja), pliki TXT/CSV (zapisywanie wyników), podstawowe narzędzia terminalowe (uruchamianie testów i organizacja plików).

### 3. Projektowanie Efektywnych Algorytmów
To część podzielona na zad1-zad4, w której użyto konkretnych algorytmów TSP i metaheurystyk:
- zad1: `RAND` (random search), `NN` (Nearest Neighbour), `RNN` (Repetitive Nearest Neighbour), `BRUTE_FORCE`,
- zad2: `Branch and Bound` w trzech strategiach rozwijania węzłów: `BFS`, `DFS`, `LC`, opcjonalnie z początkowym ograniczeniem górnym (UB) z `RNN`,
- zad3: `Simulated Annealing` z ruchami sąsiedztwa `swap`, `insert`, `invert` oraz harmonogramem chłodzenia `geometric` lub `logarithmic`,
- zad4: `Genetic Algorithm` z selekcją turniejową, krzyżowaniem `order crossover (OX)` i mutacjami `swap/insert/invert`, z możliwością inicjalizacji rozwiązaniem `RNN`.
W każdej części realizowano pełny eksperyment: odczyt instancji (w tym TSPLIB), konfigurację parametrów z `config.txt`, wielokrotne uruchomienia, zapis do CSV (wyniki surowe i historie przebiegu), a następnie analizę błędu względnego i czasu działania.

Technologie i narzędzia: C++ (główna implementacja), Python (skrypty pomocnicze do wybranych zadań - zad3 i zad4), struktura modułowa, pliki konfiguracyjne TXT, wyniki w CSV, LaTeX (raporty i dokumentacja eksperymentów), dane TSPLIB oraz własne generatory instancji.

### 4. dijkstra i filozofowie
W tej części zaimplementowano konkretne rozwiązania:
1) `dijkstraSingle` - jednowątkowe wyznaczanie najkrótszej ścieżki,
2) `dijkstraDouble` - dwuwątkowa wersja Dijkstry z równoległym przeszukiwaniem od początku i od końca,
3) symulację problemu ucztujących filozofów z własną klasą `Semaphore`, tablicą `mutex` dla widelców i wątkami `std::thread`.
W eksperymentach sprawdzano czas działania wersji jedno- i dwuwątkowej, poprawność wyniku oraz zachowanie systemu współbieżnego pod kątem zakleszczenia i zagłodzenia (m.in. limit semafora `threads - 1` i kontrola czasu oczekiwania).

Technologie i narzędzia: C++ (logika współbieżna), mechanizmy wielowątkowości i synchronizacji z biblioteki standardowej C++ (wątki, mutexy, sekcje krytyczne), pliki TXT (wejście/wyjście), LaTeX (sprawozdanie), podstawowe narzędzia terminalowe (uruchamianie testów i organizacja plików).

### 5. wielowątkowy serwer
Projekt zawiera konkretną implementację serwera usług z:
1) bezpieczną kolejką priorytetową opartą o `std::priority_queue`, `std::mutex` i `std::condition_variable`,
2) pulą wątków roboczych w klasie `Service`,
3) współdzielonymi zasobami chronionymi modelem czytelnicy-pisarz (`std::shared_mutex`) i klasycznym `std::mutex` dla logów,
4) zadaniami o priorytetach (`Task`), które wpływają na kolejność pobierania zadań z kolejki,
5) rejestracją zdarzeń i metryk w logach tekstowych.
Przeprowadzono scenariusze obciążeniowe i analizę logów pod kątem kolejności obsługi zadań, poprawności synchronizacji oraz zachowania serwera podczas uruchamiania i kontrolowanego zatrzymywania.

Technologie i narzędzia: C++, programowanie wielowątkowe (std::thread, std::mutex, std::condition_variable), własna bezpieczna struktura kolejki priorytetowej, architektura oparta o klasy (`Service`, `Task`, `SharedResources`), logi tekstowe, podstawowe narzędzia terminalowe (uruchamianie testów i organizacja plików).

