import java.util.*;

public class Lr32Step{
    public static void main(String[] args) {
        Set<String> hashSet = new HashSet<>();

        OrderedHashSet<String> orderedHashSet = new OrderedHashSet<>();

        System.out.println("Initializing with programmatic data...");
        initializeSet(orderedHashSet);

        System.out.println("\nInitial OrderedHashSet contents:");
        displaySet(orderedHashSet);

        Scanner scanner = new Scanner(System.in);
        System.out.print("\nEnter an element to add to the end of the set: ");
        String element = scanner.nextLine();

        boolean added = orderedHashSet.addToEnd(element);

        if (added) {
            System.out.println("Element added successfully!");
        } else {
            System.out.println("Element already exists in the set. HashSet cannot contain duplicates.");
        }

        System.out.println("\nUpdated OrderedHashSet contents:");
        displaySet(orderedHashSet);

        System.out.println("\n--- Comparison with regular HashSet ---");
        hashSet.clear();
        hashSet.addAll(Arrays.asList("Apple", "Banana", "Cherry", "Date"));
        System.out.println("Regular HashSet contents (order not guaranteed):");
        displaySet(hashSet);

        hashSet.add(element);
        System.out.println("\nAfter adding '" + element + "' (position not guaranteed):");
        displaySet(hashSet);

        System.out.println("\nWould you like to add more elements? (y/n)");
        String response = scanner.nextLine();

        while (response.equalsIgnoreCase("y")) {
            System.out.print("\nEnter another element to add to the end: ");
            element = scanner.nextLine();

            added = orderedHashSet.addToEnd(element);

            if (added) {
                System.out.println("Element added successfully!");
            } else {
                System.out.println("Element already exists in the set. HashSet cannot contain duplicates.");
            }

            System.out.println("\nUpdated OrderedHashSet contents:");
            displaySet(orderedHashSet);

            System.out.println("\nWould you like to add more elements? (y/n)");
            response = scanner.nextLine();
        }

        scanner.close();
    }

    private static void initializeSet(OrderedHashSet<String> set) {
        set.addToEnd("Apple");
        set.addToEnd("Banana");
        set.addToEnd("Cherry");
        set.addToEnd("Date");
    }

    private static <T> void displaySet(Set<T> set) {
        int count = 1;
        for (T element : set) {
            System.out.println(count + ". " + element);
            count++;
        }
    }
}

class OrderedHashSet<E> extends LinkedHashSet<E> {

    public boolean addToEnd(E element) {
        return add(element);
    }
}