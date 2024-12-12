#pragma once
#ifndef thread_data

#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

typedef struct thread_data {
	vector<int> results; // Буфер результатов.
	vector<pair<int, int>> queue; // Очередь пар чисел.
	bool is_file_ended = false; // Флаг - закончился ли файл.
	int index; // Индекс потока.
	mutex qmtx; // Мьютекс доступа к очереди с парами.
} thread_data;

#endif