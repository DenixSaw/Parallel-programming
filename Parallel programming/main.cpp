#include <iostream>
#include <thread>
#include <locale>
#include "thread_data.h"
#include <sstream>
#include <fstream>
#include <string>



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

void worker(thread_data* data) {
	
}


int main()
{
	setlocale(0, "RUS");

	string line;
	ifstream source_file("nums_for_gcd.txt");
	int a, b;
	int N = 8;

	thread* threads = new thread[N];
	thread_data* data = new thread_data[N];


	for (int i = 0; i < N; i++) {
		threads[i] = thread(worker, &data[i]);
	}

	if (source_file.is_open()) {
		while (getline(source_file, line)) {
			istringstream temp(line); // TODO: поменять название переменной temp.
			temp >> a >> b;
			
		}
	}
	source_file.close();

	for (int i = 0; i < N; i++) {
		threads[i].join();
	}

	delete[] threads;
	delete[] data;

	return EXIT_SUCCESS;
}
