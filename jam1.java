import java.io.*; 
import java.util.ArrayList;
import java.util.Scanner;
import java.util.Comparator;
import java.util.Collections;

/**
 * CpuUtilization class calculates CPU utilization
 * takes input file name as an argument
 * Usage: java CpuUtilization inputFile.txt 
 */
class CpuUtilization {
  CpuUtilization(String inputFileName) throws FileNotFoundException, IOException { 
    series = new ArrayList<Integer>();
    inputData = new ArrayList<InputData>();
    outputData = new ArrayList<OutputData>();

    debugPrint("inputFileName: " + inputFileName); 
    read(inputFileName);
  }

  /**
   * Reads input file to construct input data structures
   */
  protected void read(String fileName) throws FileNotFoundException, IOException {
    debugPrint("--- Reading ---"); 
    BufferedReader in = new BufferedReader(new FileReader(fileName));
    boolean firstLine = true;
    while (in.ready()) {
      String line = in.readLine();
      debugPrint("New input line: " + line); 
      Scanner scanner = new Scanner(line);
      if (firstLine) {
        firstLine = false;
        while (scanner.hasNextInt()) {
          series.add(scanner.nextInt());
        }
      } else {
        InputData data = new InputData();
        data.name = scanner.next();
        data.start   = scanner.nextInt();
        data.stop    = scanner.nextInt();
        data.counts  = scanner.nextInt();
        debugPrint("Adding new data: " + data.name + ", " + data.start + "..." + data.stop + ", " + data.counts); 
        inputData.add(data);
      }
    }
    in.close();
  } 

  /**
   * Performms calculation of CPU utilization using input data
   */
  protected void calculate() {
    debugPrint("--- Calculating ---"); 
    for (int i = 0; i < series.size(); i++) {
      int section = series.get(i);
      OutputData data = new OutputData();
      data.section = section;
      data.items = new ArrayList<OutputData.Item>();
      debugPrint("series[" + i + "]=" + section);
      double rate = 0;
      for (int j = 0; j < inputData.size(); j++) {
        if (section >= inputData.get(j).start && section < inputData.get(j).stop) {
          data.items.add(data.new Item(inputData.get(j).name, inputData.get(j).getRate()));
          rate += inputData.get(j).getRate();
        }
      }

      for (int j = 0; j < data.items.size(); j++) {
        OutputData.Item item = data.items.get(j);
        item.rate *= 100.0/rate;
        item.rate = Math.round(item.rate * 10.0) / 10.0;
        data.items.set(j, item);
      }

      Collections.sort(data.items, data.ItemComparator);

      outputData.add(data);
    }
  } 

  /**
   * Prints out the result of CPU utilization calculation
   */
  protected void print(PrintStream out) {
    debugPrint("--- Printing ---"); 
    out.println();
    for (int i = 0; i < outputData.size(); i++) {
      out.println(outputData.get(i).section);
      for (int j = 0; j < outputData.get(i).items.size(); j++) {
        out.println(outputData.get(i).items.get(j).name + " " 
          + outputData.get(i).items.get(j).rate + "%");
      }
      out.println();
    }
  } 

  /**
   * Private class represents input data
   */
  private class InputData {
    String name;
    Integer start;
    Integer stop;
    Integer counts;
    public double getRate() { return counts / (stop - start); };
  }

  /**
   * Private class represents output data
   */
  private class OutputData {
    class Item {
      Item(String n, double r) { name = n; rate = r; }
      String name;
      double rate;
    }

    Comparator<Item> ItemComparator = new Comparator<Item>() {
      public int compare(Item x, Item y) {
        if (x.rate > y.rate)
          return 1;
        if (x.rate == y.rate)
          return x.name.compareTo(y.name);

        return 0;
      }
    };

    int section;
    ArrayList<Item> items;
  }

  /**
   * Private input and output data
   */
  private ArrayList<Integer> series;
  private ArrayList<InputData> inputData;
  private ArrayList<OutputData> outputData;

  /**
   * Private internal function to print debug logs
   * Should be disabled in release mode
   */
  private static void debugPrint(String message) {
    //System.out.println("[DEBUG] " + message); 
  }

  /**
   * Main
   */
  public static void main(String[] args) { 
    try {

      if (args.length > 0) {
        String inputFileName = args[0];
        CpuUtilization cpuUtilization = new CpuUtilization(inputFileName);
        cpuUtilization.calculate();
        cpuUtilization.print(System.out);
      }

    } catch(Exception e) {
      System.out.println("ERROR: " + e); 
    }
  }
}
