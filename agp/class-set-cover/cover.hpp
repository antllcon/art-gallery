#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

template <typename T>
class GreedySetCover
{
public:
	using SetType = unordered_set<T>;
	using IndexType = int;

	struct InternalSetEntry
	{
		int original_index; // Исходный индекс множества во входном векторе
		unordered_set<int> elements; // Элементы множества, отображенные на int
	};

	GreedySetCover(const vector<SetType>& inputSets)
	{
		int current_internal_index = 0;
		for (int i = 0; i < inputSets.size(); ++i)
		{
			InternalSetEntry entry;
			entry.original_index = i;
			for (const auto& item : inputSets[i])
			{
				int mapped = mapElement(item);
				entry.elements.insert(mapped);
				m_universe.insert(mapped);
			}
			m_internalSets.push_back(entry);
		}
	}

	vector<int> solve() const
	{
		unordered_set<int> uncovered = m_universe;
		vector<int> result;

		priority_queue<pair<int, int>> pq;

		for (const auto& entry : m_internalSets)
		{
			int coverCount = 0;
			for (int x : entry.elements)
			{
				if (uncovered.count(x))
				{
					++coverCount;
				}
			}

			if (coverCount > 0)
			{
				pq.emplace(coverCount, entry.original_index);
			}
		}

		while (!uncovered.empty() && !pq.empty())
		{
			auto [current_potential_count, original_idx] = pq.top();
			pq.pop();

			const auto& current_set_elements = m_internalSets[original_idx].elements;

			int actual_cover_count = 0;
			unordered_set<int> newly_covered_by_this_set;

			for (int x : current_set_elements)
			{
				if (uncovered.count(x))
				{
					newly_covered_by_this_set.insert(x);
					actual_cover_count++;
				}
			}

			if (actual_cover_count == 0)
			{
				continue;
			}
			if (actual_cover_count < current_potential_count)
			{
				pq.emplace(actual_cover_count, original_idx);
				continue;
			}

			result.push_back(original_idx);

			for (int x : newly_covered_by_this_set)
			{
				uncovered.erase(x);
			}
		}

		return result;
	}

	unordered_set<T> coveredElements(const vector<int>& resultIndices) const
	{
		unordered_set<T> covered;
		for (int original_idx : resultIndices)
		{
			if (original_idx >= 0 && original_idx < m_internalSets.size())
			{
				for (int elem : m_internalSets[original_idx].elements)
				{
					if (m_reverseMap.count(elem))
					{
						covered.insert(m_reverseMap.at(elem));
					}
				}
			}
		}
		return covered;
	}

	size_t getUniverseSize() const
	{
		return m_universe.size();
	}

	size_t getNumInputSets() const
	{
		return m_internalSets.size();
	}

private:
	vector<InternalSetEntry> m_internalSets;

	unordered_set<int> m_universe;
	unordered_map<T, int> m_elemToIndex;
	unordered_map<int, T> m_reverseMap;
	int m_nextIndex = 0;

	int mapElement(const T& elem)
	{
		auto [it, inserted] = m_elemToIndex.emplace(elem, m_nextIndex);
		if (inserted)
		{
			m_reverseMap[m_nextIndex++] = elem;
		}
		return it->second;
	}
};