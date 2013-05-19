#!/usr/bin/python

import sys
debugMode = False;

class OutputDataItem:
    def __init__(self, name, rate):
        ''' OutputDataItem Constructor '''
        self.name = name
        self.rate = rate

    def comparaotor(x, y):
        ''' Comparotor function for sorting '''
        if x.rate > y.rate:
            return 1
        if x.rate < y.rate:
            return -1
        else:
            if x.name > y.name:
                return 1
            else:
                return -1

class CpuUtilization:
    def __init__(self, filename = ""):
        ''' CpuUtilization Constructor '''
        if filename != "":
            self.readData(filename)

    def readData(self, filename):
        ''' Read data from input file '''
        try:
            debugPrint("--- Reading input file ---")
            file = open(filename)
            self.data = {}
            firstLine = True
            for line in file:
                line = line.strip()
                debugPrint(">>> " + line)
                if firstLine:
                    self.series = line.split(' ')
                    firstLine = False
                else:
                    data = line.split(' ')
                    name = data[0]
                    start = data[1]
                    stop = data[2]
                    interval = data[3]
                    debugPrint("Data: " + name + " = " + start + "-" + stop + " : " + interval)
                    self.data[name] = (start, stop, interval)
        except:
            print "Unexpected error while reading file: ", sys.exc_info()[0]

    def calculate(self):
        ''' Calculate data and produce output data '''
        debugPrint("--- Calculating ---")
        debugPrint("Series: " + ', '.join(self.series))
        for d in self.data:
            debugPrint(d + " => " + self.data[d][0] + "-" + self.data[d][1] + " : " + self.data[d][2])
        self.output = dict()
        for series in self.series:
            debugPrint("series=" + series)
            itemList = list()
            itemSum = 0
            for d in self.data:
                if int(series) >= int(self.data[d][0]) and int(series) < int(self.data[d][1]):
                    debugPrint(d + " : add to output " + self.data[d][2])
                    rate = float(self.data[d][2]) / (float(self.data[d][1]) - float(self.data[d][0]))
                    item = OutputDataItem(d, rate)
                    itemList.append(item)
                    itemSum += rate
            for item in itemList:
                item.rate *= 100 / itemSum
                item.rate = round(item.rate * 10.0) / 10.0;
            if len(itemList) > 0:
                self.output[series] = itemList

    def printOut(self):
        ''' Printing output data with sorting '''
        debugPrint("--- Printing ---")
        for series in self.output:
            print("\n" + series)
            for item in sorted(self.output[series], cmp=OutputDataItem.comparaotor):
                print(item.name + " : " + str(item.rate) + "%")


def debugPrint(message):
    ''' Print debug messages if debugMode is enabled '''
    if debugMode:
        print "[DEBUG] " + message


def printUsage():
    ''' Print usage '''
    print "No input file! \n\n   Usage: " + sys.argv[0] + ' <input-file>\n'


def main():
    ''' Main function '''
    try:
        if len(sys.argv) > 1:
            filename = sys.argv[1]
            debugPrint("filename=" + filename)
            cpuUtilization = CpuUtilization(filename)
            cpuUtilization.calculate()
            cpuUtilization.printOut()
        else:
            printUsage()
    except:
        print "Unexpected error: ", sys.exc_info()[0]


if __name__ == '__main__':
    main()
