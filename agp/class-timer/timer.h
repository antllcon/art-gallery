#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>
#include <string>

class Timer
{
public:
	Timer(const std::string& name = "", bool autoPrint = true);
	~Timer();

	void Reset();
	double ElapsedMs() const;
	void Print() const;

private:
	std::string m_name;
	bool m_autoPrint;
	std::chrono::high_resolution_clock::time_point m_start;
};

#endif // TIMER_H