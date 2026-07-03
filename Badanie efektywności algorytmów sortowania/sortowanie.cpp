#include <iostream>
#include <time.h>
#include <windows.h>
#include <fstream>
using namespace std;

void sortA(int *A, int n, long long &comps, long long &swaps) {
    int h,j;
    comps = 0, swaps = 0;
    for(int i=1; i<n; i++) {
        h = A[i];       // element h będzie wstawiony w odpowiednie miejsce
        j = i-1;
        // przesuwanie elementów większych od h
        while(j>=0) {
            comps++;
            if(A[j]>h) {
                A[j+1] = A[j];
                swaps++;
                j--;
            } else break;
        }
        // wstawienie h w odpowiednie miejsce
        if(j+1!=i) {
            A[j+1] = h;
            swaps++;
        }
    }
}

void sortB(int *B, int n, long long &comps, long long &swaps) {
    int h,j;
    comps = 0, swaps = 0;
    for(int i=1; i<n; i++) {
        h = B[i];       // element h będzie wstawiony w odpowiednie miejsce
        j = 0;
        // szukanie miejsca, gdzie wstawić h
        while(j<i) {
            comps++;
            if(B[j]<=h) {
                j++;
            } else break;
        }
        // przesuwanie elementów większych od h
        for(int k=i; k>j; k--) {
            B[k] = B[k-1];
            swaps++;
        }
        // wstawienie h w odpowiednie miejsce
        if(j!=i) {
            B[j] = h;
            swaps++;
        }
    }
}

void sortC(int *C, int n, long long &comps, long long &swaps) {
    int h,j;
    comps = 0, swaps = 0;
    for(int i=n-1; i>=0; i--) {
        h = C[i];       // element h będzie wstawiony w odpowiednie miejsce
        j = n;
        // szukanie miejsca, gdzie wstawić h
        while(j>i) {
            comps++;
            if(C[j]>=h) {
                j--;
            } else break;
        }
        // przesuwanie elementów mniejszych od h
        for(int k=i; k<j; k++) {
            C[k] = C[k+1];
            swaps++;
        }
        // wstawienie h w odpowiednie miejsce
        if(j!=i) {
            C[j] = h;
            swaps++;
        }
    }
}

void sortD(int *D, int n, long long &comps, long long &swaps) {
    int h,j;
    comps = 0, swaps = 0;
    for(int i=n-1; i>=0; i--) {
        h = D[i];       // element h będzie wstawiony w odpowiednie miejsce
        j = i+1;
        // przesuwanie elementów większych od h
        while(j<n) {
            comps++;
            if(D[j]<h) {
                D[j-1] = D[j];
                swaps++;
                j++;
            } else break;
        }
        // wstawienie h w odpowiednie miejsce
        if(j-1!=i) {
            D[j-1] = h;
            swaps++;
        }
    }
}

void sortE(int *E, int n, long long &comps, long long &swaps) {
    int h,left,right,mid;
    comps = 0, swaps = 0;
    for(int i=1; i<n; i++) {
        h = E[i];       // element h będzie wstawiony w odpowiednie miejsce
        left = 0;
        right = i-1;
        // wyszukiwanie binarne miejsca wstawienia
        while(left<=right) {
            comps++;
            mid = (left+right)/2;
            if(E[mid]>h) right = mid-1;
            else left = mid+1;
        }
        // przesuwanie elementów większych od h
        for(int j=i; j>left; j--) {
            E[j] = E[j-1];
            swaps++;
        }
        // wstawienie h w odpowiednie miejsce
        if(left!=i) {
            E[left] = h;
            swaps++;
        }
    }
}

bool isSorted(int *arr, int n) {
    for(int i=1; i<n; i++) {
        if(arr[i]<arr[i-1]) {
            return false;       // tablica nie jest posortowana
        }
    }
    return true;        // tablica jest posortowana
}

void displayAndSort(int *arr, int n, void(*sortFunc)(int *, int, long long &, long long &), const string &arrayName, double *czas, long long *comps, long long *swaps, int index) {
    long long compsCount = 0, swapsCount = 0;

    // cout<<endl<<"Tablica "<<arrayName<<" przed posortowaniem: "<<endl;
    // for(int i=0; i<n; i++) {cout<<arr[i]<<" ";}

    // cout<<"Start sortowania tablicy "<<arrayName;

    clock_t start = clock();
    sortFunc(arr, n, compsCount, swapsCount);
    clock_t stop = clock();
    double t = (double)(stop-start)/CLOCKS_PER_SEC*1000;        // czas w milisekundach

    // sprawdzenie poprawności sortowania
    if(!isSorted(arr, n)) {
        cout<<"Błąd: Tablica "<<arrayName<<" nie została poprawnie posortowana!"<<endl;
    }

    // zapis wyników do tablic
    czas[index] += t;
    comps[index] += compsCount;
    swaps[index] += swapsCount;

    // cout<<"Czas sortowania tablicy "<<arrayName<<": "<<t<<"ms"<<endl;
    // cout<<"Liczba porownan: "<<compsCount<<endl;
    // cout<<"Liczba przestawien: "<<swapsCount<<endl;
    
    // cout<<"Tablica "<<arrayName<<" po posortowaniu: "<<endl;
    // for(int i=0; i<n; i++) {cout<<arr[i]<<" ";}
    // cout<<endl;
}

int main() {
    srand(time(NULL));      // inicjalizacja generatora liczb losowych

    const int tests = 10;        // liczba powtórzeń dla uśredniania wyników

    // rozmiary tablic do testowania
    int sizes[] = {1000, 5000, 10000, 50000, 100000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);

   for (int s=0; s<numSizes; s++) {
        int n = sizes[s];
        // cout<<"Rozmiar tablicy: "<<n<<endl;

        int *T = new int[n];
        int *A = new int[n];
        int *B = new int[n];
        int *C = new int[n];
        int *D = new int[n];
        int *E = new int[n];

        // tablice do przechowywania wyników
        double *czas = new double[5];
        long long *comps = new long long[5];
        long long *swaps = new long long[5];

        // testowanie sortowań dla różnych danych
        for(int testType=0; testType<3; testType++) {
            // inicjalizacja wyników
            for(int i=0; i<5; i++) {
                czas[i] = 0;
                comps[i] = 0;
                swaps[i] = 0;
            }

            // ustawienie liczby testów w zależności od typu danych
            int numTests = (testType == 0) ? tests : 1;

            // wybór danych wejściowych
            for(int test=0; test<numTests; test++) {
                if(testType==0) {
                    // losowe dane
                    for(int i=0; i<n; i++) {
                        T[i] = rand()%100000;
                    }
                    if(test==0) cout<<"Testowanie dla losowych danych:"<<endl<<endl;
                } else if(testType==1) {
                    // dane posortowane rosnąco
                    for(int i=0; i<n; i++) {
                        T[i] = i;
                    }
                    if(test==0) cout<< "Testowanie dla danych posortowanych rosnaco:"<<endl<<endl;
                } else if(testType==2) {
                    // dane posortowane malejąco
                    for(int i=0; i<n; i++) {
                        T[i] = n-i;
                    }
                    if(test==0) cout<<"Testowanie dla danych posortowanych malejaco:"<<endl<<endl;
                }

                // przepisanie tablicy T do tablic A, B, C, D, E
                for(int i=0; i<n; i++) {
                    A[i] = T[i];
                    B[i] = T[i];
                    C[i] = T[i];
                    D[i] = T[i];
                    E[i] = T[i];
                }

                // sortowanie i pomiar wyników
                cout<<"Test nr: "<<test+1<<endl;
                displayAndSort(A, n, sortA, "A", czas, comps, swaps, 0);
                displayAndSort(B, n, sortB, "B", czas, comps, swaps, 1);
                displayAndSort(C, n, sortC, "C", czas, comps, swaps, 2);
                displayAndSort(D, n, sortD, "D", czas, comps, swaps, 3);
                displayAndSort(E, n, sortE, "E", czas, comps, swaps, 4);
            }

            // uśrednianie wyników
            for(int i=0; i<5; i++) {
                czas[i] /= numTests;
                comps[i] /= numTests;
                swaps[i] /= numTests;
            }

            // wyświetlenie tablic wyników
            cout<<"n\tCzas[ms]\tPorownania\tPrzestawienia"<<endl;
            for (int i=0; i<5; i++) {
                cout<<n<<"\t"<<czas[i]<<"\t"<<comps[i]<<"\t"<<swaps[i]<<endl;
            }
        }

        // zwolnienie pamięci
        delete[] T;
        delete[] A;
        delete[] B;
        delete[] C;
        delete[] D;
        delete[] E;
        delete[] czas;
        delete[] comps;
        delete[] swaps;
    }
    return 0;
}
