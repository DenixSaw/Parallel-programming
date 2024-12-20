
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

int main() {
	setlocale(0, "RUS");

	int num_threads = 8;
	omp_set_num_threads(num_threads);

	const char* filename = "results.txt";
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

	string line;
	ifstream source_file("nums_for_gcd.txt");
	double start_time = omp_get_wtime();
	thread_data* data = new thread_data[num_threads]; // Увеличиваем размер массива
	bool is_file_ended = false;

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
#pragma omp critical
					{
						// Перемещаем добавление в очередь в critical
						data[cnt].queue.push_back(make_pair(a, b));
					}
					cnt = (cnt - 1) % num_threads + 1; // Изменяем на num_threads, чтобы корректно распределить
				}
			}
			source_file.close();
			is_file_ended = true;
		}

		if (thread_index != 0) {
			int a = 0, b = 0;
			while (true) {
				if (data[thread_index].queue.empty() && is_file_ended) // Используем omp_get_thread_num() - 1
					break;

				if (!data[thread_index].queue.empty()) {
#pragma omp critical
					{
						a = data[thread_index].queue[0].first;
						b = data[thread_index].queue[0].second;
						data[thread_index].queue.erase(data[thread_index].queue.begin());
					}
					int current_result = calc_gcd(a, b);
					data[thread_index].results.push_back(current_result);

					if (data[thread_index].results.size() == MAX_SIZE) {
#pragma omp critical
						{
							write_results(data[thread_index]);


						}

					}
				}
			}

			if (!data[thread_index].results.empty()) {
#pragma omp critical
				{
					write_results(data[thread_index]);
				}
			}

		}

#pragma omp barrier
		if (thread_index == 0) {
			double end_time = omp_get_wtime();
			cout << "Результаты записаны." << endl;
			cout << "Время выполнения: " << end_time - start_time << " секунд" << endl;
			delete[] data; // Освобождаем выделенную память
		}
	}
	return 0;
}