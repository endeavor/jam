// Compile: g++ -std=c++0x jam1.cpp
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <cstring>

//#define DEBUG_LOG(fmt, msg...) { printf("[DEBUG] " fmt "\n", ##msg); }
#define DEBUG_LOG(msg...) {}

class CpuUtilization
{
public:
    CpuUtilization(const char* inputFileName)
    {
        std::ifstream  inputFileStream(inputFileName);
        std::string    line;

        std::stringstream firstLine;
        std::getline(inputFileStream, line);
        firstLine << line;
        while (!firstLine.eof()) {
            int value;
            firstLine >> value;
            DEBUG_LOG("serie=%d", value);
            series.push_back(value);
        }

        while (std::getline(inputFileStream, line)) {
            DEBUG_LOG("line=%s", line.c_str());
            std::stringstream values;
            values << line;
            DEBUG_LOG("values=%s", values.str().c_str());

            std::string name;
            int start, stop, counts;
            values >> name >> start >> stop >> counts;
            DEBUG_LOG("New data: %s %d %d %d", name.c_str(), start, stop, counts);
            InputData data(name, start, stop, counts);
            inputData.push_back(data);
        }
    };

    void calculate()
    {
        DEBUG_LOG("=== Calculation ============");
        for (auto i = series.begin(); i != series.end(); i++) {
            DEBUG_LOG("serie = %d", *i);
            std::vector< std::pair<std::string, double> > data;
            double rate = 0;
            for (auto j = inputData.begin(); j != inputData.end(); j++) {
                if (*i >= j->getStart() && *i < j->getStop()) {
                    DEBUG_LOG("%s : %f", j->getName().c_str(), j->getRate());
                    data.push_back(std::pair<std::string, double>(j->getName(), j->getRate()));
                    rate += j->getRate();
                }
            }

            for (auto j = data.begin(); j != data.end(); j++) {
                (*j).second *= 100.0/rate;
                (*j).second = floor((*j).second * 10.0 + 0.5) / 10.0;
            }

            outputData.push_back(OutputData(*i, data));
        }
    };

    void print(std::ostream& out)
    {
        DEBUG_LOG("=== Output ============");
        out << std::endl;
        for (auto i = outputData.begin(); i != outputData.end(); i++) {
            out << i->getSection() << std::endl;
            for (auto j = i->getData().begin(); j != i->getData().end(); j++) {
                out << (*j).first << " " << (*j).second << "%" << std::endl;
            }
            out << std::endl;
        }
    };

protected:
    class InputData
    {
    public:
        InputData(std::string n, int s, int t, int c) : name(n), start(s), stop(t), counts(c) 
        {
            DEBUG_LOG("New data: %s %d %d %d", name.c_str(), start, stop, counts);
        };

        std::string getName(void) { return name; };
        int getStart(void) { return start; };
        int getStop(void) { return stop; };
        int getCounts(void) { return counts; };

        int getInterval(void) { return stop - start; };
        double getRate(void) { return double(counts) / double(stop - start); };

    private:
        std::string name;
        int start;
        int stop;
        int counts;
    };

    class OutputData
    {
    public:
        OutputData(int s, std::vector< std::pair<std::string, double> > d) 
            : section(s), data(d) 
        {
            sort();
        };
        bool operator > (OutputData& other) { return section > other.section; };
        int getSection(void) { return section; };
        std::vector< std::pair<std::string, double> >& getData(void) { return data; };

    protected:
        void sort(void)
        {
            std::sort(data.begin(), data.end(), 
                [] (std::pair<std::string, double> x, std::pair<std::string, double> y)
                {
                    if (x.second > y.second)
                        return true;
                    if (x.second == y.second)
                        return std::strcmp(x.first.c_str(), y.first.c_str()) < 0;

                    return false;
                }
            );
        };

    private:
        int section;
        std::vector< std::pair<std::string, double> > data;
    };

private:
    std::vector<int> series;
    std::vector<InputData> inputData;
    std::vector<OutputData> outputData;
};

int main(int argc, char* argv[])
{
    try {
        if (argc > 1) {
            char* inputFileName = argv[1];
            DEBUG_LOG("Input file: %s", inputFileName);
            CpuUtilization cpuUtilization(inputFileName);
            cpuUtilization.calculate();
            std::ostream& out = std::cout;
            cpuUtilization.print(out);
        } else {
            std::cout << "No input file" << std::endl << std::endl;
            std::cout << "    Usage: " << argv[0] << " inputFileName" << std::endl << std::endl;
        }
    } catch (std::exception &e) {
        std::cout << "ERROR: " << e.what () << std::endl;
    } catch (...) {
        std::cout << "ERROR: Unknown!" << std::endl;
    }
    return 0;
};
