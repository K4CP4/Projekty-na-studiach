#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

using std::cout;
using std::vector;
using std::thread;
using std::mutex;
using std::lock_guard;
using std::this_thread::sleep_for;
using std::condition_variable;
using std::unique_lock;
using std::atomic_bool;
using std::chrono::seconds;

int simulation_time = 60; 			// czas symulacji w sekundach
int threads = 11;					// liczba filozofów i widelców
vector<thread> thread_vec;			// rezerwacja miejsca na wątki
vector<mutex> sticks(threads);		// widelce
atomic_bool thread_check = true;	// flaga kontrolująca działanie wątków
mutex print_mutex;					// mutex do synchronizacji wypisywania na ekran
vector<int> meals;					// licznik zjedzonych posiłków przez każdego filozofa
// stany filozofów
enum philoState {		
	Think,
	Hungry,
	Eat
};

// klasa ograniczająca liczbę filozofów próbujących jeść na raz
class Semaphore {
	public:
		bool try_acquire() {
			unique_lock<mutex> lock(mtx, std::try_to_lock);
			if (!lock.owns_lock() || count == 0) return false;
			--count;
			return true;
		}
	mutex mtx;
	condition_variable cv;
	int count;
	explicit Semaphore(int initial) : count(initial) {}
	void acquire() {
		unique_lock<mutex> lock(mtx);
		cv.wait(lock, [&] { return count > 0; });
		--count;
	}
	void release() {
		unique_lock<mutex> lock(mtx);
		++count;
		cv.notify_one();
	}
};
Semaphore sem(threads - 1);			// ograniczenie jedzenia na raz

void philosopherLoop(const int id) {
	while (thread_check) {
		// myślenie
		int think_time = 1 + rand() % 5;
		{
			lock_guard<mutex> lock(print_mutex);
			cout << "Filozof " << id << " mysli (" << think_time << " s).\n";
		}
		sleep_for(seconds(think_time));
		// głód
		{
			lock_guard<mutex> lock(print_mutex);
			cout << "Filozof " << id << " jest glodny.\n";
		}
		time_t hungry_since = time(NULL);
		while (true) {
			if (!thread_check) return;
			if (time(NULL) - hungry_since > 10) {
				lock_guard<mutex> lock(print_mutex);
				cout << "Filozof " << id << " zostal zaglodzony!\n";
				thread_check = false;
				return;
			}
			if (sem.try_acquire()) break;
			sleep_for(seconds(1));
		}
		// próba podniesienia widelców
		int left = id;
		int right = (id + 1) % threads;
		std::lock(sticks[left], sticks[right]);
		// jedzenie
		{
			lock_guard<mutex> lock(print_mutex);
			cout << "Filozof " << id << " je (" << think_time << " s).\n";
		}
		sleep_for(seconds(think_time));
		// zakończony posiłek – odnotuj i odłóż widelce
		++meals[id];
		sticks[left].unlock();
		sticks[right].unlock();
		sem.release();
	}
}

int main() {
	srand(static_cast<unsigned>(time(nullptr)));
	thread_check.store(true);			// flaga kontrolująca działanie wątków
	thread_vec.resize(threads);			// rezerwacja miejsca na wątki
	meals.assign(threads, 0);
	int i = 0;							// id filozofa
	for (thread& thr : thread_vec) {
		thr = thread(philosopherLoop, i++);
	}
	sleep_for(seconds(simulation_time));
	thread_check.store(false);			// sygnał zakończenia dla wątków
	for (thread& thr : thread_vec) {
		thr.join();
	}
	{
		lock_guard<mutex> lock(print_mutex);
		cout << "\nPodsumowanie posilkow:" << "\n";
		for (int id = 0; id < threads; ++id) {
			cout << "Filozof " << id << ": " << meals[id] << " posilkow" << "\n";
		}
	}
	return 0;
}
