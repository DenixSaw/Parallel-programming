#include <iostream>
#define HAVE_STRUCT_TIMESPEC
#include <chrono>
#include <pthread.h>
#include <locale>
#include <limits>
#include "thread_data.h"


int main()
{
	setlocale(0, "RUS");
	return EXIT_SUCCESS;
}
