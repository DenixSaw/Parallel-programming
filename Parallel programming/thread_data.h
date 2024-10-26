#pragma once
#ifndef thread_data

typedef struct thread_data {
	double left; // Левая граница.
	double step; // Шаг.
	double result; // Результат, полученный потоком.
	int subintervals; // Количество интервалов, обрабатываемых потоком
} thread_data;

#endif