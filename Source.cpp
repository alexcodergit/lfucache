#include<iostream>
#include<vector>
#include<unordered_map>
#include<list>
#include<assert.h>


using namespace std;

// implementation of LFU (least frequently used) cache
class LFUCache {
	typedef list<pair<int, list<pair<int, int>>>> frequencyList;
	typedef list<pair<int, list<pair<int, int>>>>::iterator frequencyListIterator;
	typedef unordered_map<int, pair<frequencyListIterator, list<pair<int, int>>::iterator>> elementsMap;
	int cap;
	frequencyList freqList;
	elementsMap eMap;

	void updateFrequency(elementsMap::iterator eit) {
		int fr = eit->second.first->first;
		int key = eit->second.second->first;
		int val = eit->second.second->second;

		auto fListItr = std::next(eit->second.first);
		eit->second.first->second.erase(eit->second.second);
		if (eit->second.first->second.size() < 1)
			freqList.erase(eit->second.first);

		if (fListItr == freqList.end()) {
			auto it = freqList.insert(freqList.end(), { fr + 1, {{key, val}} });
			eMap[key] = { it, it->second.begin() };
		}
		else if(fListItr->first == (fr + 1)){
			auto it = fListItr->second.insert(fListItr->second.begin(), { key, val });
			eMap[key] = { fListItr , it };
		}
		else {
			auto it = freqList.insert(fListItr, { fr + 1, {{key, val}} });
			eMap[key] = {it, it->second.begin()};
		}
	}

	void evictOldest() {
		auto fl = freqList.begin();
		if (fl != freqList.end()) {
			auto pit = prev(fl->second.end());
			eMap.erase(pit->first);
			fl->second.erase(pit);
			if (fl->second.size() < 1)
				freqList.erase(fl);
		}
	}

public:
	LFUCache(int capacity):cap(capacity) {}

	int get(int key) {
		elementsMap::iterator ef = eMap.find(key);
		if (ef == eMap.end()) {
			return -1;
		}
		int val = ef->second.second->second;
		updateFrequency(ef);
		return val;
	}

	void put(int key, int value) {
		if (cap < 1)
			return;
		auto ef = eMap.find(key);
		if (ef == eMap.end()) {
			if (eMap.size() >= cap) {
				evictOldest();
			}
			auto fl = freqList.begin();
			if (fl == freqList.end()) {
				auto it = freqList.insert(freqList.begin(), { 1, { {key, value}} });
				eMap[key] = { it , it->second.begin() };
			}
			else {
				if (fl->first == 1) {
					auto it = fl->second.insert(fl->second.begin(), { key, value });
					eMap[key] = { fl, it };
				}
				else {
					fl = freqList.insert(freqList.begin(), { 1, {{key, value}} });
					eMap[key] = { fl, fl->second.begin() };
				}
			}
		}
		else {
			*ef->second.second = { key, value };
			updateFrequency(ef);
		}
	}

	void clear() {
		freqList.clear();
		eMap.clear();
	}
};

void testCache();
void testCache2();

int main()
{
	testCache();
	testCache2();

	LFUCache cache(2);
	cache.put(1, 1);
	cache.put(2, 2);
	int v = cache.get(1);
	assert(v == 1);
	cache.put(3, 3);
	v = cache.get(2);
	assert(v == -1);
	v = cache.get(3);
	assert(v == 3);
	cache.put(4, 4);
	v = cache.get(1);
	assert(v == -1);
	v = cache.get(3);
	assert(v == 3);
	v = cache.get(4);
	assert(v == 4);

	cache = LFUCache(3);

	cache.put(2, 2);
	cache.put(1, 1);
	int r = cache.get(2);
	assert(r == 2);
	r = cache.get(1);
	assert(r == 1);
	r = cache.get(2);
	assert(r == 2);
	cache.put(3, 3);
	cache.put(4, 4);
	r = cache.get(3);
	assert(r == -1);
	r = cache.get(2);
	assert(r == 2);
	r = cache.get(1);
	assert(r == 1);
	r = cache.get(4);
	assert(r == 4);
	
	cache.clear();

	cache.put(1,2);
	cache.put(2,3);
	cache.put(3,4);
	cache.put(1,5);
	v = cache.get(1);
	cache.get(2);
	cache.get(3);
	cache.put(4, 5);
	v = cache.get(5);
	cout << __FUNCTION__ << " ok\n";

	char c = getchar();
}

void testCache2()
{
	LFUCache cache(5);
	cache.put(1,1);
	cache.put(2,2);
	cache.put(3,3);
	cache.put(4,4);

	int v = cache.get(1);
	v = cache.get(1);
	assert(v == 1);

	v = cache.get(2);
	v = cache.get(2);
	assert(v == 2);

	v = cache.get(3);
	v = cache.get(3);
	assert(v == 3);

	v = cache.get(4);
	v = cache.get(4);
	assert(v == 4);

	cout << __FUNCTION__ << " ok\n";

}

void testCache()
{
	LFUCache cache(10);
	cache.put(10,13);
	cache.put(3,17);
	cache.put(6,11);
	cache.put(10,5);
	cache.put(9,10);
	int v = cache.get(13);
	assert(v == -1);
	cache.put(2,19);
	v = cache.get(2);
	assert(v == 19);
	v = cache.get(3);
	assert(v == 17);
	cache.put(5, 25);
	v = cache.get(8);
	assert(v == -1);
	cache.put(9, 22);
	cache.put(5, 5);
	cache.put(1, 30);
	v = cache.get(11);
	assert(v == -1);
	cout << __FUNCTION__ << " ok\n";
}