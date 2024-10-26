#include <iostream>
#include <cmath>
#define HAVE_STRUCT_TIMESPEC
#include <chrono>
#include <pthread.h>
#include <locale>

constexpr int WEIGHTS[] = { 1, 3, 3, 1 }; // Весовые коэфициенты.
constexpr auto Cn = 8; // Сумма весов.
constexpr auto N = 3; // Порядок метода.

using namespace std;
using namespace std::chrono;

auto f = [](double x) -> double { return pow(x, 3) + 3 * pow(x, 2) - 2 * x + 1; };

typedef struct thread_data {
	double left; // Левая граница.
	double step; // Шаг.
	double result; // Результат, полученный потоком.
	int subintervals; // Количество интервалов, обрабатываемых потоком
} thread_data;


static void* integrate(void* thr_data) {
	thread_data* data = (thread_data*)thr_data;
	double x_i, x_j = data->left; // Начальное положение xj и xi.
	double subinterval_sum; // Сумма на одном подинтервале.

	for (int j = 0; j < data->subintervals; j++) {
		subinterval_sum = 0;
		x_j = data->left + j * N * data->step;

		for (int i = 0; i < N + 1; i++) {
			x_i = x_j + i * data->step;
			subinterval_sum += WEIGHTS[i] * f(x_i);
		}
		data->result += subinterval_sum;
	}
	data->result = data->result / Cn * N * data->step;

	return nullptr;
}


int main()
{
	setlocale(0, "RUS");

	int count_threads = 8; // Кол-во потоков.
	double a = -3; // Левая граница интегрирования.
	double b = 5; // Правая граница интегрирования.
	int cnt_step = 30000000; // Кол-во шагов. Чем больше, тем точнее результат.

	if (count_threads > 8 || count_threads < 1) {
		cout << "Количество потоков должно быть числом от 1 до 8 включительно!" << endl;
		return -1;
	}
	else if (b < a) {
		cout << "Левая граница интегрирования должна быть больше правой! Проверьте переменные a и b!" << endl;
		return -1;
	}
	else if (cnt_step % 3 != 0) {
		cout << "Количество шагов должно быть целое число, кратное 3!" << endl;
		return -1;
	}

	double cnt_subinterval = 0; // Общее количество подынтервалов.
	double step = abs(b - a) / (double)cnt_step; // Шаг интегрирования.
	cnt_subinterval = (double)cnt_step / 3;

	double subinterval_for_thread = cnt_subinterval / count_threads; // Количество подынтервалов в потоке.
	int last_subinterval = 0; // Количество подынтервалов для последнего потока.

	if (floor(subinterval_for_thread) != subinterval_for_thread)
	{
		subinterval_for_thread = floor(subinterval_for_thread); // Целое, округленное количество подынтервалов для одного потока.
		last_subinterval = cnt_subinterval - (count_threads - 1) * subinterval_for_thread; // Количество подынтервалов для последнего потока. после округления.
	}
	else
		last_subinterval = subinterval_for_thread;

	thread_data* data = new thread_data[count_threads];
	pthread_t* threads = new pthread_t[count_threads];

	double left = a; // Левая граница.
	double right = 0.0; // Правая граница.
	double total_result = 0.0; // Переменная, в которую будет записан общий результат выполнения потоков.

	auto start = high_resolution_clock::now(); // Фиксируем время перед началом работы потоков.
	for (int i = 0; i < count_threads; i++) {

		if (count_threads - i == 1)
			subinterval_for_thread = last_subinterval;

		right = left + subinterval_for_thread * N * step; // Пересчитываем правую границу.

		data[i] = { left, step, 0, int(subinterval_for_thread) }; // Заполняем структуру данных для потока.
		pthread_create(&threads[i], nullptr, integrate, (void*)&data[i]); // Создаем поток и подаем в него данные.

		left = right; // Сдвигаем левую границу. Права граница текущего подынтервала является левой границей следующего.
	}

	// В цикле отлавливаем потоки после их выполнения и достаем из них результат. Прибавляем результат в общий результат.
	for (int i = 0; i < count_threads; ++i) {
		pthread_join(threads[i], nullptr);
		total_result += data[i].result;
	}

	// Фиксируем и выводим время, за которое потоки выполнили работу.
	auto end = high_resolution_clock::now();
	duration<double> elapsed = end - start;

	cout << total_result << endl;
	cout << "Время выполнения: " << elapsed.count() << " секунд" << endl;

	// Очищаем выделенную память.
	delete[] data;
	delete[] threads;

	return 0;
}
