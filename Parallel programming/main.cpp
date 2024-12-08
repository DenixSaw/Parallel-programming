#include <iostream>
#include <thread>
#include <locale>
#include "thread_data.h"
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <limits>
#include <mutex>
#include <condition_variable>

using namespace std::chrono;

const int MAX_RES = 5;
mutex mtx;
condition_variable cv;

int calc_gcd(int a, int b) {
	if (a == 0 || b == 0)
		return 1;

	while (a && b)
		if (a > b) a %= b;
		else b %= a;
	return a + b;
}

void record_result(thread_data* data) {
	int a = data->queue[0].first;
	int b = data->queue[0].second;
	data->queue.erase(data->queue.begin()); // Удаляем обработанную пару
	int current_result = calc_gcd(a, b);
	data->results.push_back(current_result);
}

void write_results(thread_data* data) {
	unique_lock<mutex> lock(mtx);
	fstream file("results.txt", std::ios::in | std::ios::out | std::ios::app);
	if (!file) {
		file.open("results.txt", std::ios::out);
	}

	while (!data->results.empty()) {
		file << data->results[0] << endl;
		data->results.erase(data->results.begin());
	}

	lock.unlock();
	cv.notify_all(); // Уведомляем другие потоки
	//cout << "Результаты записаны потоком №" << data->index << endl;
}

void worker(thread_data* data) {
	while (true) {
		if (data->queue.empty() && data->is_file_ended) {
			// Если очередь пуста и файл закончился, выходим из цикла
			break;
		}

		if (!data->queue.empty()) {
			record_result(data); // Обрабатываем пару

			if (!data->results.empty() && data->results.size() == MAX_RES) {
				write_results(data); // Записываем результаты в файл
			}
		}
		//else {
		//	// Если очередь пуста, ждем, пока появятся новые данные. Можем здесь делать задержку
		//	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		//	continue;
		//}
	}

	// Записываем оставшиеся результаты, если есть
	if (!data->results.empty()) {
		write_results(data);
	}
}

int main() {
	setlocale(0, "RUS");

	string line;
	ifstream source_file("nums_for_gcd.txt");
	int a, b;
	int N = 4;

	thread* threads = new thread[N];
	thread_data* data = new thread_data[N];

	ofstream file("results.txt", std::ios::out | std::ios::trunc);
	if (!file) {
		file.open("results.txt", std::ios::out);
	}
	file.close();

	// Инициализация данных для потоков
	for (int i = 0; i < N; i++) {
		data[i].index = i;
	}
	// Запуск потоков
	for (int i = 0; i < N; i++) {
		threads[i] = thread(worker, &data[i]);
	}

	auto time_start = high_resolution_clock::now();

	if (source_file.is_open()) {
		int cnt = 0;
		while (getline(source_file, line)) {
			istringstream temp(line);
			temp >> a >> b;
			data[cnt].queue.push_back(make_pair(a, b));
			cnt = (cnt + 1) % N; // Циклическое распределение пар
		}
	}
	source_file.close();

	// Установка флага окончания обработки файла
	for (int i = 0; i < N; i++) {
		data[i].is_file_ended = true;
	}

	// Ожидание завершения потоков
	for (int i = 0; i < N; i++) {
		threads[i].join();
	}

	auto time_end = high_resolution_clock::now();
	duration<double> elapsed = time_end - time_start;

	cout << endl;
	cout << "Время вычисления: " << elapsed.count() << " секунд" << endl;

	delete[] threads;
	delete[] data;

	return EXIT_SUCCESS;
}