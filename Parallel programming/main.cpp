#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <omp.h>
#include <sys/stat.h>
using namespace std;

int calc_gcd(int a, int b) {
	if (a == 0 || b == 0)
		return 1;

	while (a && b)
		if (a > b) a %= b;
		else b %= a;
	return a + b;
}

void write_results(const vector<int>& results) {
	ofstream file("results.txt", ios::app);
	for (const int& result : results) {
		file << result << endl;
	}
}

int main() {
	setlocale(0, "RUS");

	int num_threads = 8;
	omp_set_num_threads(num_threads);

	string line;
	ifstream source_file("nums_for_gcd.txt");
	double start_time = omp_get_wtime();
#pragma opm parallel
	vector<pair<int, int>> pairs;

#pragma omp master
	{
		// Чтение пар чисел из файла
		if (source_file.is_open()) {
			while (getline(source_file, line)) {
				istringstream temp(line);
				int a, b;
				temp >> a >> b;
#pragma omp critical
				{
					pairs.push_back(make_pair(a, b));
				}
			}
		}
		source_file.close();
	}
	int num_pairs = pairs.size();

	// Параллельное вычисление GCD и запись результатов в файл
#pragma omp parallel
	{
		vector<int> thread_results; // Результаты для каждого потока
#pragma omp for
		for (int i = 0; i < num_pairs; i++) {
			int a = pairs[i].first;
			int b = pairs[i].second;
			int current_result = calc_gcd(a, b);
#pragma omp critical
			{
				thread_results.push_back(current_result); // Сохраняем результаты в локальный вектор
			}
		}
		// Записываем результаты в файл из каждого потока
#pragma omp critical
		{
			write_results(thread_results);
		}
	}
	double end_time = omp_get_wtime();
	cout << "Результаты записаны." << endl;
	cout << "Время выполнения:" << end_time - start_time << endl;

	return EXIT_SUCCESS;
}
