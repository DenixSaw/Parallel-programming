#pragma once
#ifndef thread_data

typedef struct thread_data {
	double left; // ����� �������.
	double step; // ���.
	double result; // ���������, ���������� �������.
	int subintervals; // ���������� ����������, �������������� �������
} thread_data;

#endif