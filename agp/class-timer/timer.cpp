#include "timer.h"

Timer::Timer(const std::string& name, bool autoPrint)
	: m_name(name)
	, m_autoPrint(autoPrint)
	, m_start(std::chrono::high_resolution_clock::now())
{
}

Timer::~Timer()
{
	if (m_autoPrint)
	{
		Print();
	}
}

void Timer::Reset()
{
	m_start = std::chrono::high_resolution_clock::now();
}

double Timer::ElapsedMs() const
{
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<double, std::milli>(end - m_start).count();
}

void Timer::Print() const
{
	std::cout << (m_name.empty() ? "" : m_name + ": ") << ElapsedMs() << " ms\n";
}
