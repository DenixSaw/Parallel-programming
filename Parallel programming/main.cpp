#include <iostream>
#include <omp.h>
#include <locale>
#include <limits>
#include <cmath>

constexpr int WEIGHTS[] = { 1, 3, 3, 1 }; // Весовые коэфициенты.
constexpr auto Cn = 8; // Сумма весов.
constexpr auto N = 3; // Порядок метода.

using namespace std;

auto f = [](double x) -> double { return pow(x, 3) + 3 * pow(x, 2) - 2 * x + 1; };

int main()
{
    setlocale(0, "RUS");

    int cnt_threads = 8; // Кол-во потоков.
    double a = -3; // Левая граница интегрирования.
    double b = 5; // Правая граница интегрирования.
    int cnt_steps = 3000000; // Кол-во шагов. Чем больше, тем точнее результат.

    //while (true) {
    //    cout << "Введите кол-во потоков (от 1 до 8 включительно): ";
    //    cin >> cnt_threads;

    //    if (cnt_threads < 1 || cnt_threads > 8) {
    //        cout << "Количество потоков должно быть числом от 1 до 8 включительно!" << endl;
    //    }
    //    else
    //        break;

    //    cin.clear();
    //    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //    cout << endl;
    //}

    //while (true) {
    //    cout << "Введите левую границу интегрирования: ";
    //    cin >> a;
    //    cout << "Введите правую границу интегрирования: ";
    //    cin >> b;

    //    if (a > b) {
    //        cout << "Левая граница должна быть меньше правой!" << endl; // Исправлено сообщение
    //    }
    //    else
    //        break;

    //    cin.clear();
    //    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //    cout << endl;
    //}

    //while (true) {
    //    cout << "Введите кол-во шагов, кратное 3: ";
    //    cin >> cnt_steps;

    //    if (cnt_steps % 3 != 0) {
    //        cout << "Количество шагов должно быть целым числом, кратным 3!" << endl;
    //    }
    //    else
    //        break;

    //    cin.clear();
    //    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    //    cout << endl;
    //}

    double step = abs(b - a) / (double)cnt_steps; // Шаг интегрирования.
    double total_result = 0.0; // Переменная, в которую будет записан общий результат выполнения потоков.

    auto time_start = omp_get_wtime(); // Фиксируем время перед началом работы потоков.

#pragma omp parallel for num_threads(cnt_threads) reduction(+:total_result)
    for (int j = 0; j < cnt_steps / 3; j++) {
        double subinterval_sum = 0;
        double x_j = a + j * N * step;

        for (int i = 0; i <= N; i++) {
            double x_i = x_j + i * step;
            subinterval_sum += WEIGHTS[i] * f(x_i);
        }

        total_result += subinterval_sum / Cn * N * step;
    }

    auto time_end = omp_get_wtime(); // Фиксируем время после окончания работы потоков.
    cout << endl;
    cout << "Результат: " << total_result << endl;
    cout << "Время вычисления: " << (time_end - time_start) << " секунд" << endl;

    return EXIT_SUCCESS;
}
