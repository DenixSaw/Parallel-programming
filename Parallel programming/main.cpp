#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <omp.h>
#include <sys/stat.h>
#define MAX_SIZE 5

using namespace std;

typedef struct thread_data {
	vector<int> results; // Буфер результатов.
	vector<pair<int, int>> queue; // Очередь пар чисел.
	bool is_file_ended = false; // Флаг - закончился ли файл.
	omp_lock_t q_lock; // omp мьютекс, для доступа к очереди рабочего потока. 
} thread_data;

int calc_gcd(int a, int b) {
	if (a == 0 || b == 0)
		return 1;

	while (a && b)
		if (a > b) a %= b;
		else b %= a;
	return a + b;
}

void write_results(thread_data& data) {
	fstream file("results.txt", std::ios::in | std::ios::out | std::ios::app);
	if (!file) {
		file.open("results.txt", std::ios::out);
	}

	while (!data.results.empty()) {
		file << data.results[0] << endl;
		data.results.erase(data.results.begin());
	}
	file.close();
}

void clear_or_create_file(const char* filename) {
	struct stat buffer;

	// Проверяем, существует ли файл и не пустой ли он
	if (stat(filename, &buffer) == 0 && buffer.st_size > 0) {
		// Файл существует и не пустой, очищаем его
		fstream file(filename, ios::out | ios::trunc);
		file.close();
	}
	else {
		// Файл не существует, создаем его
		fstream file(filename, ios::out);
		file.close();
	}

}

int main() {
	setlocale(0, "RUS");

	int num_threads = 8;
	omp_set_num_threads(num_threads);

	string line;
	ifstream source_file("nums_for_gcd.txt");

	const char* result_file = "results.txt";
	clear_or_create_file(result_file);

	thread_data* data = new thread_data[num_threads - 1]; // массив структур thread_data для рабочих потоков.
	bool is_file_ended = false; // Флаг окончания файла.

	// Инициализируем omp_lock для каждого из рабочих потоков.
	for (int i = 0; i < num_threads - 1; i++) {
		omp_init_lock(&data[i].q_lock);
	}


	double start_time = omp_get_wtime();
#pragma omp parallel
	{
		int thread_index = omp_get_thread_num();
#pragma omp master
		{
			// Чтение пар чисел из файла
			if (source_file.is_open()) {
				int cnt = 1;
				while (getline(source_file, line)) {
					istringstream temp(line);
					int a, b;
					temp >> a >> b;

					omp_set_lock(&data[cnt - 1].q_lock);
					data[cnt - 1].queue.push_back(make_pair(a, b));
					omp_unset_lock(&data[cnt - 1].q_lock);

					cnt = (cnt % (num_threads - 1)) + 1;
				}
			}
			source_file.close();
			is_file_ended = true;
		}

		if (thread_index > 0) {
			int a = 0, b = 0;
			while (true) {
				if (data[thread_index - 1].queue.empty() && is_file_ended)
					break;

				if (!data[thread_index - 1].queue.empty()) {

					omp_set_lock(&data[thread_index - 1].q_lock);
					a = data[thread_index - 1].queue[0].first;
					b = data[thread_index - 1].queue[0].second;
					data[thread_index - 1].queue.erase(data[thread_index - 1].queue.begin());
					omp_unset_lock(&data[thread_index - 1].q_lock);

					int current_result = calc_gcd(a, b);
					data[thread_index - 1].results.push_back(current_result);

					if (data[thread_index - 1].results.size() == MAX_SIZE) {
#pragma omp critical (write_results)
						{
							write_results(data[thread_index - 1]);
						}
					}
				}
			}

			if (!data[thread_index - 1].results.empty()) {
#pragma omp critical (write_results)
				{
					write_results(data[thread_index - 1]);
				}
			}
		}

#pragma omp barrier
		if (thread_index == 0) {
			double end_time = omp_get_wtime();
			cout << "Результаты записаны." << endl;
			cout << "Время выполнения: " << end_time - start_time << " секунд" << endl;
			for (int i = 0; i < num_threads - 1; i++) {
				omp_destroy_lock(&data[i].q_lock);
			}
			delete[] data;
		}
	}
	return 0;
}
