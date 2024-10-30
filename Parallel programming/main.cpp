#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <chrono>
#include <pthread.h>
#include <locale>


constexpr int WEIGHTS[] = { 1, 3, 3, 1 }; // Весовые коэфициенты.
constexpr auto Cn = 8; // Сумма весов.
constexpr auto N = 3; // Порядок метода.

using namespace std;
using namespace std::chrono;


int main()
{
	setlocale(0, "RUS");

	return EXIT_SUCCESS;
}
