#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <chrono>
#include <pthread.h>
#include <locale>
#include <limits>
#include "thread_data.h"

constexpr int WEIGHTS[] = { 1, 3, 3, 1 }; // Весовые коэфициенты.
constexpr auto Cn = 8; // Сумма весов.
constexpr auto N = 3; // Порядок метода.

using namespace std;
using namespace std::chrono;

auto f = [](double x) -> double { return pow(x, 3) + 3 * pow(x, 2) - 2 * x + 1; };


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

	int cnt_threads = 0; // Кол-во потоков.
	double a = 0; // Левая граница интегрирования.
	double b = 0; // Правая граница интегрирования.
	int cnt_steps = 0; // Кол-во шагов. Чем больше, тем точнее результат.

	while (true) {
		cout << "Введите кол-во потоков (от 1 до 8 включительно): ";
		cin >> cnt_threads;

		if (cnt_threads < 1 || cnt_threads > 8) {
			cout << "Количество потоков должно быть числом от 1 до 8 включительно!" << endl;
		}
		else
			break;

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << endl;
	}

	//system("cls");

	while (true) {
		cout << "Введите левую границу интегрирования: ";
		cin >> a;
		cout << "Введите правую границу интегрирования: ";
		cin >> b;

		if (a > b) {
			cout << "Левая граница должна быть больше правой!" << endl;
		}
		else
			break;

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << endl;
	}

	//system("cls");

	while (true) {
		cout << "Введите кол-во шагов, кратное 3: ";
		cin >> cnt_steps;

		if (cnt_steps % 3 != 0) {
			cout << "Количество шагов должно быть целым числом, кратным 3!" << endl;
		}
		else
			break;

		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << endl;
	}

	//system("cls");

	double cnt_subinterval = 0; // Общее количество подынтервалов.
	double step = abs(b - a) / (double)cnt_steps; // Шаг интегрирования.
	cnt_subinterval = (double)cnt_steps / 3;

	double subinterval_for_thread = cnt_subinterval / cnt_threads; // Количество подынтервалов в потоке.
	int last_subinterval = 0; // Количество подынтервалов для последнего потока.

	if (floor(subinterval_for_thread) != subinterval_for_thread)
	{
		subinterval_for_thread = floor(subinterval_for_thread); // Целое, округленное количество подынтервалов для одного потока.
		last_subinterval = cnt_subinterval - (cnt_threads - 1) * subinterval_for_thread; // Количество подынтервалов для последнего потока. после округления.
	}
	else
		last_subinterval = subinterval_for_thread;

	thread_data* data = new thread_data[cnt_threads];
	pthread_t* threads = new pthread_t[cnt_threads];

	double left = a; // Левая граница.
	double total_result = 0.0; // Переменная, в которую будет записан общий результат выполнения потоков.

	auto time_start = high_resolution_clock::now(); // Фиксируем время перед началом работы потоков.
	for (int i = 0; i < cnt_threads; i++) {

		if (cnt_threads - i == 1)
			subinterval_for_thread = last_subinterval;

		data[i] = { left, step, 0, int(subinterval_for_thread) }; // Заполняем структуру данных для потока.
		pthread_create(&threads[i], nullptr, integrate, (void*)&data[i]); // Создаем поток и подаем в него данные.

		left = left + subinterval_for_thread * N * step; // Сдвигаем левую границу.
	}

	// В цикле отлавливаем потоки после их выполнения и достаем из них результат. Прибавляем результат в общий результат.
	for (int i = 0; i < cnt_threads; ++i) {
		pthread_join(threads[i], nullptr);
		total_result += data[i].result;
	}

	// Фиксируем и выводим время, за которое потоки выполнили работу.
	auto time_end = high_resolution_clock::now();
	duration<double> elapsed = time_end - time_start;

	cout << endl;
	cout << "Результат: " << total_result << endl;
	cout << "Время вычисления: " << elapsed.count() << " секунд" << endl;

	// Очищаем выделенную память.
	delete[] data;
	delete[] threads;

	return EXIT_SUCCESS;
}
