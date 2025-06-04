import java.util.Map;
import java.util.Scanner;
import java.util.TreeMap;

public class Lr3_1 {
    public static void main(String[] args) {

        TreeMap<Integer, String> map = new TreeMap<>();

        initializeMapProgrammatically(map);

        System.out.println("Current Map contents:");
        printMap(map);

        Scanner scanner = new Scanner(System.in);
        System.out.print("\nEnter the key to find the lower entry for: ");
        int key = scanner.nextInt();

        Map.Entry<Integer, String> lowerEntry = findLowerEntry(map, key);

        if (lowerEntry != null) {
            System.out.println("\nLower entry found: Key = " + lowerEntry.getKey() +
                    ", Value = " + lowerEntry.getValue());
        } else {
            System.out.println("\nNo lower entry found for key: " + key);
        }

        System.out.println("\nWould you like to add more entries to the map? (y/n)");
        scanner.nextLine();
        String response = scanner.nextLine();

        if (response.equalsIgnoreCase("y")) {
            addUserEntries(map, scanner);

            System.out.println("\nUpdated Map contents:");
            printMap(map);

            System.out.print("\nEnter another key to find the lower entry for: ");
            key = scanner.nextInt();

            lowerEntry = findLowerEntry(map, key);

            if (lowerEntry != null) {
                System.out.println("\nLower entry found: Key = " + lowerEntry.getKey() +
                        ", Value = " + lowerEntry.getValue());
            } else {
                System.out.println("\nNo lower entry found for key: " + key);
            }
        }

        scanner.close();
    }

    public static <K, V> Map.Entry<K, V> findLowerEntry(TreeMap<K, V> map, K key) {
        return map.lowerEntry(key);
    }

    private static void initializeMapProgrammatically(TreeMap<Integer, String> map) {
        map.put(10, "Ten");
        map.put(20, "Twenty");
        map.put(30, "Thirty");
        map.put(40, "Forty");
        map.put(50, "Fifty");
        map.put(75, "Seventy-Five");
        map.put(100, "Hundred");
    }

    private static void addUserEntries(TreeMap<Integer, String> map, Scanner scanner) {
        System.out.println("How many entries would you like to add?");
        int count = scanner.nextInt();
        scanner.nextLine();

        for (int i = 0; i < count; i++) {
            System.out.print("Enter key (integer): ");
            int key = scanner.nextInt();
            scanner.nextLine();

            System.out.print("Enter value (string): ");
            String value = scanner.nextLine();

            map.put(key, value);
            System.out.println("Entry added: " + key + " -> " + value);
        }
    }

    private static void printMap(TreeMap<Integer, String> map) {
        for (Map.Entry<Integer, String> entry : map.entrySet()) {
            System.out.println("Key: " + entry.getKey() + ", Value: " + entry.getValue());
        }
    }
    public static void KeyValue (Map<String , String > map, Scanner scanner) {}
}