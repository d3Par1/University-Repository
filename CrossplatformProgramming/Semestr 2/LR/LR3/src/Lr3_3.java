import java.util.*;

public class Lr3_3 {
    public static void main(String[] args) {
        TreeSet<String> colorSet = new TreeSet<>();

        System.out.println("Initializing TreeSet with programmatic data...");
        initializeTreeSetWithColors(colorSet);

        System.out.println("\nInitial TreeSet contents (automatically sorted):");
        displayTreeSet(colorSet);

        Scanner scanner = new Scanner(System.in);
        System.out.println("\nWould you like to add more colors to the TreeSet? (y/n)");
        String response = scanner.nextLine();

        while (response.equalsIgnoreCase("y")) {
            addUserColor(colorSet, scanner);

            System.out.println("\nUpdated TreeSet contents:");
            displayTreeSet(colorSet);

            System.out.println("\nWould you like to add another color? (y/n)");
            response = scanner.nextLine();
        }

        demonstrateTreeSetProperties(colorSet);

        scanner.close();
    }

    private static void initializeTreeSetWithColors(TreeSet<String> set) {
        set.add("Red");
        set.add("Blue");
        set.add("Green");
        set.add("Yellow");
        set.add("Purple");

        System.out.println("Added initial colors: Red, Blue, Green, Yellow, Purple");
    }

    private static void displayTreeSet(TreeSet<String> set) {
        if (set.isEmpty()) {
            System.out.println("The TreeSet is empty.");
            return;
        }

        int count = 1;
        for (String color : set) {
            System.out.println(count + ". " + color);
            count++;
        }
    }

    private static void addUserColor(TreeSet<String> set, Scanner scanner) {
        System.out.print("Enter a color to add to the TreeSet: ");
        String color = scanner.nextLine();

        boolean added = set.add(color);

        if (added) {
            System.out.println("Color '" + color + "' added successfully!");
        } else {
            System.out.println("Color '" + color + "' already exists in the TreeSet. TreeSet cannot contain duplicates.");
        }
    }

    private static void demonstrateTreeSetProperties(TreeSet<String> set) {
        System.out.println("\n----- TreeSet Properties Demonstration -----");

        System.out.println("\nColors in ascending order:");
        for (String color : set) {
            System.out.print(color + " ");
        }

        System.out.println("\n\nColors in descending order:");
        for (String color : set.descendingSet()) {
            System.out.print(color + " ");
        }

        if (!set.isEmpty()) {
            System.out.println("\n\nFirst color (alphabetically): " + set.first());
            System.out.println("Last color (alphabetically): " + set.last());

            if (set.size() >= 3) {
                String lastElement = set.last();

                System.out.println("\nSubset between first and last (exclusive of last):");
                for (String color : set.headSet(lastElement)) {
                    System.out.print(color + " ");
                }

                System.out.println("\n\nSubset from second element to the end:");
                Iterator<String> iterator = set.iterator();
                iterator.next(); // Skip first element
                String secondElement = iterator.next();
                for (String color : set.tailSet(secondElement)) {
                    System.out.print(color + " ");
                }
            }
        }

        System.out.println("\n\nTotal number of colors in the TreeSet: " + set.size());
    }
}