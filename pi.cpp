#include <iostream>
#include <fstream>
#include <vector>
#include <boost/thread.hpp>

const char* pattern = "000003399960000003495999940001275959999710395959599999936959595999999618595959999981";
const int patternSize = 84;

std::string readData()
{
  std::ifstream in("pi-billion.txt", std::ios::in);
  if (!in) return std::string();

  std::string contents;
  in.seekg(0, std::ios::end);
  contents.resize(in.tellg());
  in.seekg(0, std::ios::beg);
  in.read(&contents[0], contents.size());

  return contents;
}

inline int compare(const char* data)
{
    int diff = 0;
    for (int i = 0; i < patternSize; ++i)
        diff += std::abs(pattern[i] - data[i]);
    return diff;
}

class Processor {
    const char* data;
    int start, end;
public:
    Processor(const char* data, int start, int size)
        : data(data), start(start), end(start + size - patternSize)
    {}
    void operator()() {
        int bestCandidate = 800000;
        int bestPos = 0;
        for (int i = start; i < end; ++i) {
            int comp = compare(data + i);
            if (comp < bestCandidate) {
                bestCandidate = comp;
                bestPos = i;
            }
        }
        std::cout << "Best candidate: " << bestCandidate << " at " << bestPos << std::endl;
        std::cout << std::string(data + bestPos, data + bestPos + patternSize) << std::endl;
    }
};


int main(void)
{
    std::string data = readData();
    std::cout << "Size read: " << data.size() << std::endl;

    boost::thread_group threads;
    const int numThreads = 16;
    const char* dataStr = data.c_str();
    for (int i = 0; i < numThreads; ++i) {
        int pos = i * (data.size() / numThreads);
        int size = data.size() / numThreads + 1;
        if (pos + size > data.size())
            size = data.size() - pos;
        threads.create_thread(Processor(dataStr, pos, size));
    }
    threads.join_all();
}
