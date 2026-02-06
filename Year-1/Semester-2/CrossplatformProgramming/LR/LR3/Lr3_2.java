// Лабораторна робота 3 - Завдання 2
// HashSet: додавання зазначеного елемента в кінець HashSet
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.*;

public class Lr3_2 {
    private HashSet<String> colors;
    private HashSet<Integer> numbers;
    private HashSet<Double> prices;

    public  Lr3_2() {
        initializeHashSets();
    }

    // Ініціалізація HashSet-ів з тестовими даними
    private void initializeHashSets() {
        // HashSet з кольорами
        colors = new HashSet<>();
        colors.add("Червоний");
        colors.add("Синій");
        colors.add("Зелений");
        colors.add("Жовтий");
        colors.add("Білий");
        colors.add("Чорний");
        colors.add("Фіолетовий");
        colors.add("Помаранчевий");

        // HashSet з числами
        numbers = new HashSet<>();
        numbers.add(10);
        numbers.add(25);
        numbers.add(5);
        numbers.add(30);
        numbers.add(15);
        numbers.add(40);
        numbers.add(20);

        // HashSet з цінами
        prices = new HashSet<>();
        prices.add(15.50);
        prices.add(25.99);
        prices.add(100.00);
        prices.add(75.25);
        prices.add(50.80);
        prices.add(200.15);
    }

    // Метод для додавання елемента в HashSet
    // Оскільки HashSet не має поняття "кінець" (немає порядку), просто додаємо елемент
    public <T> void addElementToHashSet(HashSet<T> hashSet, T element, String setName) {
        System.out.println("\n📝 Додавання елемента до " + setName);
        System.out.println("Елемент для додавання: " + element);
        System.out.println("Стан HashSet до додавання: " + hashSet);
        System.out.println("Розмір до додавання: " + hashSet.size());

        boolean added = hashSet.add(element);

        if (added) {
            System.out.println("✅ Елемент успішно додано!");
        } else {
            System.out.println("⚠️ Елемент не додано (вже існує в множині)");
        }

        System.out.println("Стан HashSet після додавання: " + hashSet);
        System.out.println("Розмір після додавання: " + hashSet.size());

    }

    // Демонстрація роботи з кольорами
    public void demonstrateColors() {
        System.out.println("\n🎨 ДЕМОНСТРАЦІЯ: Робота з кольорами");
        System.out.println("=" .repeat(50));

        // Початковий стан
        System.out.println("Початковий HashSet кольорів:");
        displayHashSetInfo(colors, "Кольори");

        // Додавання нових кольорів
        String[] newColors = {"Рожевий", "Сірий", "Коричневий", "Червоний", "Золотий"};

        for (String color : newColors) {
            addElementToHashSet(colors, color, "HashSet кольорів");
        }

        System.out.println("\n🔍 Фінальний стан HashSet кольорів:");
        displayHashSetInfo(colors, "Кольори");
    }

    // Демонстрація роботи з числами
    public void demonstrateNumbers() {
        System.out.println("\n🔢 ДЕМОНСТРАЦІЯ: Робота з числами");
        System.out.println("=" .repeat(50));

        // Початковий стан
        System.out.println("Початковий HashSet чисел:");
        displayHashSetInfo(numbers, "Числа");

        // Додавання нових чисел
        Integer[] newNumbers = {35, 50, 10, 45, 60, 25, 80};

        for (Integer number : newNumbers) {
            addElementToHashSet(numbers, number, "HashSet чисел");
        }

        System.out.println("\n🔍 Фінальний стан HashSet чисел:");
        displayHashSetInfo(numbers, "Числа");

        // Додаткова статистика
        showNumberStatistics();
    }

    // Демонстрація роботи з цінами
    public void demonstratePrices() {
        System.out.println("\n💰 ДЕМОНСТРАЦІЯ: Робота з цінами");
        System.out.println("=" .repeat(50));

        // Початковий стан
        System.out.println("Початковий HashSet цін:");
        displayHashSetInfo(prices, "Ціни");

        // Додавання нових цін
        Double[] newPrices = {125.75, 300.00, 75.25, 45.99, 500.50, 15.50};

        for (Double price : newPrices) {
            addElementToHashSet(prices, price, "HashSet цін");
        }

        System.out.println("\n🔍 Фінальний стан HashSet цін:");
        displayHashSetInfo(prices, "Ціни");

        // Додаткова статистика
        showPriceStatistics();
    }

    // Відображення інформації про HashSet
    private <T> void displayHashSetInfo(HashSet<T> hashSet, String name) {
        System.out.println(name + ":");
        System.out.println("  Розмір: " + hashSet.size());
        System.out.println("  Елементи: " + hashSet);
        System.out.println("  Порожній: " + (hashSet.isEmpty() ? "Так" : "Ні"));
    }

    // Статистика по числах
    private void showNumberStatistics() {
        System.out.println("\n📊 Статистика по числах:");

        if (numbers.isEmpty()) {
            System.out.println("HashSet порожній");
            return;
        }

        int min = Collections.min(numbers);
        int max = Collections.max(numbers);
        double average = numbers.stream().mapToInt(Integer::intValue).average().orElse(0.0);
        int sum = numbers.stream().mapToInt(Integer::intValue).sum();

        System.out.printf("  Мінімальне значення: %d\n", min);
        System.out.printf("  Максимальне значення: %d\n", max);
        System.out.printf("  Середнє значення: %.2f\n", average);
        System.out.printf("  Сума всіх елементів: %d\n", sum);

        // Числа за категоріями
        long evenCount = numbers.stream().filter(n -> n % 2 == 0).count();
        long oddCount = numbers.size() - evenCount;

        System.out.printf("  Парних чисел: %d\n", evenCount);
        System.out.printf("  Непарних чисел: %d\n", oddCount);
    }

    // Статистика по цінах
    private void showPriceStatistics() {
        System.out.println("\n📊 Статистика по цінах:");

        if (prices.isEmpty()) {
            System.out.println("HashSet порожній");
            return;
        }

        double min = Collections.min(prices);
        double max = Collections.max(prices);
        double average = prices.stream().mapToDouble(Double::doubleValue).average().orElse(0.0);
        double sum = prices.stream().mapToDouble(Double::doubleValue).sum();

        System.out.printf("  Мінімальна ціна: %.2f грн\n", min);
        System.out.printf("  Максимальна ціна: %.2f грн\n", max);
        System.out.printf("  Середня ціна: %.2f грн\n", average);
        System.out.printf("  Загальна вартість: %.2f грн\n", sum);

        // Ціни за категоріями
        long cheapCount = prices.stream().filter(p -> p < 50.0).count();
        long moderateCount = prices.stream().filter(p -> p >= 50.0 && p < 150.0).count();
        long expensiveCount = prices.stream().filter(p -> p >= 150.0).count();

        System.out.printf("  Дешевих товарів (< 50 грн): %d\n", cheapCount);
        System.out.printf("  Помірних товарів (50-150 грн): %d\n", moderateCount);
        System.out.printf("  Дорогих товарів (> 150 грн): %d\n", expensiveCount);
    }

    // Демонстрація основних операцій HashSet
    public void demonstrateHashSetOperations() {
        System.out.println("\n🛠️ ДЕМОНСТРАЦІЯ: Основні операції HashSet");
        System.out.println("=" .repeat(50));

        // Створення нового HashSet
        HashSet<String> fruits = new HashSet<>();

        System.out.println("1. Створення порожнього HashSet:");
        displayHashSetInfo(fruits, "Фрукти");

        // Додавання елементів
        System.out.println("\n2. Додавання елементів:");
        String[] fruitList = {"Яблуко", "Банан", "Апельсин", "Груша", "Киви"};
        for (String fruit : fruitList) {
            fruits.add(fruit);
            System.out.printf("   Додано: %s (розмір тепер: %d)\n", fruit, fruits.size());
        }

        // Перевірка наявності елементів
        System.out.println("\n3. Перевірка наявності елементів:");
        String[] checkFruits = {"Яблуко", "Манго", "Банан", "Ананас"};
        for (String fruit : checkFruits) {
            boolean contains = fruits.contains(fruit);
            System.out.printf("   %s: %s\n", fruit, contains ? "✅ Є" : "❌ Немає");
        }

        // Видалення елементів
        System.out.println("\n4. Видалення елементів:");
        String[] removeList = {"Груша", "Манго", "Киви"};
        for (String fruit : removeList) {
            boolean removed = fruits.remove(fruit);
            System.out.printf("   %s: %s (розмір тепер: %d)\n",
                fruit, removed ? "✅ Видалено" : "❌ Не знайдено", fruits.size());
        }

        System.out.println("\n5. Фінальний стан:");
        displayHashSetInfo(fruits, "Фрукти");
    }

    // Демонстрація операцій з множинами
    public void demonstrateSetOperations() {
        System.out.println("\n🔗 ДЕМОНСТРАЦІЯ: Операції з множинами");
        System.out.println("=" .repeat(50));

        // Створення двох множин
        HashSet<Integer> set1 = new HashSet<>(Arrays.asList(1, 2, 3, 4, 5, 6));
        HashSet<Integer> set2 = new HashSet<>(Arrays.asList(4, 5, 6, 7, 8, 9));

        System.out.println("Множина 1: " + set1);
        System.out.println("Множина 2: " + set2);

        // Об'єднання (union)
        HashSet<Integer> union = new HashSet<>(set1);
        union.addAll(set2);
        System.out.println("\nОб'єднання (union): " + union);

        // Перетин (intersection)
        HashSet<Integer> intersection = new HashSet<>(set1);
        intersection.retainAll(set2);
        System.out.println("Перетин (intersection): " + intersection);

        // Різниця (difference)
        HashSet<Integer> difference = new HashSet<>(set1);
        difference.removeAll(set2);
        System.out.println("Різниця (set1 - set2): " + difference);

        // Симетрична різниця
        HashSet<Integer> symmetricDiff = new HashSet<>(union);
        symmetricDiff.removeAll(intersection);
        System.out.println("Симетрична різниця: " + symmetricDiff);
    }

    // Інтерактивний режим
    public void interactiveMode() {
        Scanner scanner = new Scanner(System.in);

        System.out.println("\n🎮 ІНТЕРАКТИВНИЙ РЕЖИМ");
        System.out.println("=" .repeat(30));

        while (true) {
            System.out.println("\nОберіть опцію:");
            System.out.println("1. Додати колір");
            System.out.println("2. Додати число");
            System.out.println("3. Додати ціну");
            System.out.println("4. Показати всі HashSet-и");
            System.out.println("5. Перевірити наявність елемента");
            System.out.println("6. Видалити елемент");
            System.out.println("0. Вийти");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();
                scanner.nextLine(); // споживаємо новий рядок

                switch (choice) {
                    case 1:
                        System.out.print("Введіть назву кольору: ");
                        String color = scanner.nextLine();
                        addElementToHashSet(colors, color, "HashSet кольорів");
                        break;

                    case 2:
                        System.out.print("Введіть число: ");
                        int number = scanner.nextInt();
                        addElementToHashSet(numbers, number, "HashSet чисел");
                        break;

                    case 3:
                        System.out.print("Введіть ціну: ");
                        double price = scanner.nextDouble();
                        addElementToHashSet(prices, price, "HashSet цін");
                        break;

                    case 4:
                        System.out.println("\n📋 Всі HashSet-и:");
                        displayHashSetInfo(colors, "Кольори");
                        displayHashSetInfo(numbers, "Числа");
                        displayHashSetInfo(prices, "Ціни");
                        break;

                    case 5:
                        System.out.println("Перевірити наявність в якому HashSet?");
                        System.out.println("1 - Кольори, 2 - Числа, 3 - Ціни");
                        int setChoice = scanner.nextInt();
                        scanner.nextLine();

                        if (setChoice == 1) {
                            System.out.print("Введіть колір: ");
                            String checkColor = scanner.nextLine();
                            boolean exists = colors.contains(checkColor);
                            System.out.printf("Колір \"%s\": %s\n", checkColor,
                                exists ? "✅ Існує" : "❌ Не існує");
                        } else if (setChoice == 2) {
                            System.out.print("Введіть число: ");
                            int checkNumber = scanner.nextInt();
                            boolean exists = numbers.contains(checkNumber);
                            System.out.printf("Число %d: %s\n", checkNumber,
                                exists ? "✅ Існує" : "❌ Не існує");
                        } else if (setChoice == 3) {
                            System.out.print("Введіть ціну: ");
                            double checkPrice = scanner.nextDouble();
                            boolean exists = prices.contains(checkPrice);
                            System.out.printf("Ціна %.2f: %s\n", checkPrice,
                                exists ? "✅ Існує" : "❌ Не існує");
                        }
                        break;

                    case 6:
                        System.out.println("Видалити з якого HashSet?");
                        System.out.println("1 - Кольори, 2 - Числа, 3 - Ціни");
                        int removeChoice = scanner.nextInt();
                        scanner.nextLine();

                        if (removeChoice == 1) {
                            System.out.print("Введіть колір для видалення: ");
                            String removeColor = scanner.nextLine();
                            boolean removed = colors.remove(removeColor);
                            System.out.printf("Колір \"%s\": %s\n", removeColor,
                                removed ? "✅ Видалено" : "❌ Не знайдено");
                        } else if (removeChoice == 2) {
                            System.out.print("Введіть число для видалення: ");
                            int removeNumber = scanner.nextInt();
                            boolean removed = numbers.remove(removeNumber);
                            System.out.printf("Число %d: %s\n", removeNumber,
                                removed ? "✅ Видалено" : "❌ Не знайдено");
                        } else if (removeChoice == 3) {
                            System.out.print("Введіть ціну для видалення: ");
                            double removePrice = scanner.nextDouble();
                            boolean removed = prices.remove(removePrice);
                            System.out.printf("Ціна %.2f: %s\n", removePrice,
                                removed ? "✅ Видалено" : "❌ Не знайдено");
                        }
                        break;

                    case 0:
                        return;

                    default:
                        System.out.println("❌ Невірний вибір!");
                }
            } catch (Exception e) {
                System.out.println("❌ Помилка введення!");
                scanner.nextLine(); // очищаємо буфер
            }
        }
    }

    public static void main(String[] args) {
        System.out.println("=== Лабораторна робота 3 - Завдання 2 ===");
        System.out.println("HashSet: додавання зазначеного елемента");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        Lr3_2 program = new Lr3_2();

// Demonstration of main functionality
        program.demonstrateColors();
        program.demonstrateNumbers();
        program.demonstratePrices();
        program.demonstrateHashSetOperations();
        program.demonstrateSetOperations();

// Interactive mode
        program.interactiveMode();

        System.out.println("\n✅ Програма завершена успішно!");
    }
}