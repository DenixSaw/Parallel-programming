#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <chrono>
#include <pthread.h>
#include <locale>
#include "thread_data.h"


int calc_gcd(int a, int b) {
	while (a != b) {
		if (a > b) {
			a -= b;
		}
		else {
			b -= a;
		}
	}
	return b;
}

void run_thread(*thread_data data) {

}

int main()
{
	setlocale(0, "RUS");
	return EXIT_SUCCESS;
}
