// Compile: g++ -std=c++0x jam2.cpp
#include <stdio.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

/// 
/// Uncommend DEBUG define to enable console lons
///
#undef  DEBUG
#define DEBUG
#ifdef  DEBUG
#define DEBUG_LOG(fmt, msg...) { printf("[DEBUG] " fmt "\n", ##msg); }
#else
#define DEBUG_LOG(msg...) {}
#endif

///
/// Class Box represents a box to be packed into 2D stripe
/// with width, height, position within stripe: x, y
/// and also information about rotation and initial index
/// in input file
///
class Box
{
public:
    static const int nowhere = -1;
    Box(int w, int h, int i) 
        : x(nowhere), y(nowhere), index(i)
    {
        if (w < h) {
            width = w;
            height = h;
            rotated = false;
        } else {
            width = h;
            height = w;
            rotated = true;
        }

    }
    int getWidth(void) { return width; }
    int getHeight(void) { return height; }
    bool isRotated(void) { return rotated; }
    int getX(void) { return x; }
    int getY(void) { return y; }
    void move(int a, int b) { x = a; y = b; }
    bool isPlaced(void) { return x != nowhere && y != nowhere; }
    int getIndex(void) { return index; }

private:
    int width;
    int height;
    int index;
    bool rotated;
    int x, y;
};

///
/// Class Stripe represents 2D stripe within which boxes
/// should be packed
///
class Stripe
{
public:
    Stripe(const char* inputFileName = 0) 
        : width(0), height(0)
    {
        if (inputFileName && inputFileName[0]) 
            readFromFile(inputFileName);
        else
            initializeSample();
    }

    void readFromFile(const char* inputFileName) throw(std::runtime_error)
    {
        DEBUG_LOG("=== Reading input file ============");
        std::ifstream  inputFileStream(inputFileName);
        std::string    line;
        bool firstLine = true;
        int  boxIndex = 0;
        int  numberOfBoxes = 0;
        while (std::getline(inputFileStream, line)) {
            std::stringstream values;
            values << line;
            DEBUG_LOG("'%s'", values.str().c_str());

            if (firstLine) {
                values >> numberOfBoxes;
                DEBUG_LOG("    Number of boxes: %d", numberOfBoxes);
                firstLine = false;
            } else {
                int w, h;
                values >> w >> h;
                DEBUG_LOG("    Box: %d x %d", w, h);
                Box box(w, h, boxIndex);
                boxes.push_back(box);
                boxIndex++;
            }
        }

        if (boxIndex < 1)
            throw std::runtime_error("No boxes!");

        if (numberOfBoxes != boxIndex)
            throw std::runtime_error("Wrong number of boxes!");

        DEBUG_LOG("............");
        DEBUG_LOG("Input check:");
        for (auto i = boxes.begin(); i != boxes.end(); ++i) {
            DEBUG_LOG("[%d] %d x %d (rotated=%d)", i->getIndex(), i->getWidth(), i->getHeight(), i->isRotated());
        }
    }

    void initializeSample(void)
    {
//        static const size_t numberOfBoxes = 2;
//        struct { int w, h; } sample[numberOfBoxes] 
//            = {{2, 4}, {5, 2}};
        static const size_t numberOfBoxes = 9;
        struct { int w, h; } sample[numberOfBoxes] 
            = {{1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1}, {8, 1}, {9, 1}};
        for (auto i = 0; i < numberOfBoxes; i++) {
            Box box(sample[i].w, sample[i].h, i);
            boxes.push_back(box);
        }
    }

    void pack(void)
    {
        std::sort(boxes.begin(), boxes.end(), 
            [&] (Box x, Box y) {
                return x.getHeight() > y.getHeight();
            }
        );

        if (boxes.size() > 0)
            height = boxes[0].getHeight();
        width = 0;
        for (auto i = boxes.begin(); i != boxes.end(); i++) {
            i->move(width, 0);
            width += i->getWidth();
        }
    }

    void print(std::ostream& out)
    {
        DEBUG_LOG("=== Output ============");
        out << width << " " << height << std::endl;

        std::sort(boxes.begin(), boxes.end(), 
            [&] (Box x, Box y) {
                return x.getIndex() < y.getIndex();
            }
        );

        for (auto i = boxes.begin(); i != boxes.end(); i++) {
#ifdef DEBUG
            DEBUG_LOG("[%d] %d,%d (%dx%d) %d", i->getIndex(), i->getX(), i->getY(), i->getWidth(), i->getHeight(), i->isRotated());
#else
            out << i->getX() << " " << i->getY() << " " << i->isRotated() << std::endl;
#endif
        }
    }

private:
    std::vector<Box> boxes;
    int height;
    int width;
};

///
/// Main
///
int main(int argc, char* argv[])
{
    try {
        if (argc > 1) {
            char* inputFileName = argv[1];
            DEBUG_LOG("Input file: %s", inputFileName);
            Stripe stripe;//(inputFileName);
            stripe.pack();
            std::ostream& out = std::cout;
            stripe.print(out);
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
