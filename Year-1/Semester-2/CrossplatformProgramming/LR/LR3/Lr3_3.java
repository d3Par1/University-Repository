// Лабораторна робота 3 - Завдання 3
// TreeSet: створення нового TreeSet, додавання кольорів і виведення
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.*;

// Клас для представлення кольору з додатковими властивостями
class Color implements Comparable<Color> {
    private String name;
    private String hexCode;
    private String category;
    private int intensity; // інтенсивність від 1 до 10

    public Color(String name, String hexCode, String category, int intensity) {
        this.name = name;
        this.hexCode = hexCode;
        this.category = category;
        this.intensity = Math.max(1, Math.min(10, intensity)); // обмежуємо 1-10
    }

    // Геттери
    public String getName() { return name; }
    public String getHexCode() { return hexCode; }
    public String getCategory() { return category; }
    public int getIntensity() { return intensity; }

    // Сеттери
    public void setName(String name) { this.name = name; }
    public void setHexCode(String hexCode) { this.hexCode = hexCode; }
    public void setCategory(String category) { this.category = category; }
    public void setIntensity(int intensity) {
        this.intensity = Math.max(1, Math.min(10, intensity));
    }

    // Реалізація Comparable для автоматичного сортування в TreeSet
    @Override
    public int compareTo(Color other) {
        // Спочатку сортуємо за категорією
        int categoryComparison = this.category.compareTo(other.category);
        if (categoryComparison != 0) {
            return categoryComparison;
        }

        // Потім за інтенсивністю (в порядку спадання)
        int intensityComparison = Integer.compare(other.intensity, this.intensity);
        if (intensityComparison != 0) {
            return intensityComparison;
        }

        // Нарешті за назвою
        return this.name.compareTo(other.name);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Color color = (Color) obj;
        return Objects.equals(name, color.name) &&
               Objects.equals(hexCode, color.hexCode);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, hexCode);
    }

    @Override
    public String toString() {
        return String.format("%s (%s) - %s, інтенсивність: %d",
            name, hexCode, category, intensity);
    }
}

public class Lr3_3 {
    private TreeSet<String> simpleColors;
    private TreeSet<Color> complexColors;
    private TreeSet<String> customSortedColors;

    public Lr3_3() {
        initializeTreeSets();
    }

    // Ініціалізація TreeSet-ів
    private void initializeTreeSets() {
        // Простий TreeSet з назвами кольорів (автоматичне сортування за алфавітом)
        simpleColors = new TreeSet<>();

        // TreeSet з об'єктами Color (сортування за власним Comparable)
        complexColors = new TreeSet<>();

        // TreeSet з власним компаратором (сортування за довжиною назви)
        customSortedColors = new TreeSet<>(new Comparator<String>() {
            @Override
            public int compare(String s1, String s2) {
                // Спочатку за довжиною
                int lengthComparison = Integer.compare(s1.length(), s2.length());
                if (lengthComparison != 0) {
                    return lengthComparison;
                }
                // Потім за алфавітом
                return s1.compareTo(s2);
            }
        });
    }

    // Демонстрація простого TreeSet з кольорами
    public void demonstrateSimpleColors() {
        System.out.println("\n🎨 ДЕМОНСТРАЦІЯ: Простий TreeSet з кольорами");
        System.out.println("=" .repeat(60));

        // Додавання кольорів у довільному порядку
        String[] colorsToAdd = {
            "Зелений", "Червоний", "Синій", "Жовтий", "Фіолетовий",
            "Помаранчевий", "Рожевий", "Коричневий", "Сірий", "Білий",
            "Чорний", "Бірюзовий", "Лиловий", "Золотий", "Срібний"
        };

        System.out.println("Додавання кольорів у довільному порядку:");
        for (String color : colorsToAdd) {
            boolean added = simpleColors.add(color);
            System.out.printf("  %s: %s\n", color, added ? "✅ Додано" : "⚠️ Вже існує");
        }

        // Спроба додати дублікат
        System.out.println("\nСпроба додати дублікат:");
        boolean addDuplicate = simpleColors.add("Червоний");
        System.out.printf("  Червоний: %s\n", addDuplicate ? "✅ Додано" : "⚠️ Дублікат не додано");

        System.out.println("\n📋 Фінальний TreeSet (автоматично відсортований за алфавітом):");
        displayTreeSetInfo(simpleColors, "Прості кольори");

        // Демонстрація навігаційних методів
        demonstrateNavigationMethods();
    }

    // Демонстрація складного TreeSet з об'єктами Color
    public void demonstrateComplexColors() {
        System.out.println("\n🌈 ДЕМОНСТРАЦІЯ: TreeSet з об'єктами Color");
        System.out.println("=" .repeat(60));

        // Створення та додавання кольорів
        Color[] colorsToAdd = {
            new Color("Червоний", "#FF0000", "Теплий", 9),
            new Color("Світло-червоний", "#FF6666", "Теплий", 5),
            new Color("Темно-червоний", "#990000", "Теплий", 10),
            new Color("Синій", "#0000FF", "Холодний", 8),
            new Color("Світло-синій", "#6666FF", "Холодний", 4),
            new Color("Темно-синій", "#000099", "Холодний", 10),
            new Color("Зелений", "#00FF00", "Природний", 7),
            new Color("Лісовий зелений", "#228B22", "Природний", 8),
            new Color("Жовтий", "#FFFF00", "Теплий", 6),
            new Color("Фіолетовий", "#800080", "Холодний", 7),
            new Color("Білий", "#FFFFFF", "Нейтральний", 1),
            new Color("Чорний", "#000000", "Нейтральний", 10),
            new Color("Сірий", "#808080", "Нейтральний", 5)
        };

        System.out.println("Додавання кольорів (будуть автоматично відсортовані):");
        for (Color color : colorsToAdd) {
            boolean added = complexColors.add(color);
            System.out.printf("  %s: %s\n", color.getName(), added ? "✅ Додано" : "⚠️ Дублікат");
        }

        System.out.println("\n📋 TreeSet відсортований за категорією → інтенсивність → назва:");
        displayComplexColorsInfo();

        // Статистика
        showComplexColorStatistics();
    }

    // Демонстрація TreeSet з власним компаратором
    public void demonstrateCustomSortedColors() {
        System.out.println("\n🔤 ДЕМОНСТРАЦІЯ: TreeSet з власним компаратором (за довжиною назви)");
        System.out.println("=" .repeat(70));

        String[] colorsToAdd = {
            "Червоний", "Синій", "Зелений", "Фіолетовий", "Помаранчевий",
            "Рожевий", "Коричневий", "Бірюзовий", "Золотий", "Срібний",
            "Блакитний", "Малиновий", "Оливковий", "Персиковий"
        };

        System.out.println("Додавання кольорів:");
        for (String color : colorsToAdd) {
            boolean added = customSortedColors.add(color);
            System.out.printf("  %s (%d символів): %s\n",
                color, color.length(), added ? "✅ Додано" : "⚠️ Дублікат");
        }

        System.out.println("\n📋 TreeSet відсортований за довжиною назви → алфавіт:");
        displayTreeSetInfo(customSortedColors, "Кольори за довжиною");

        // Групування за довжиною
        showLengthGrouping();
    }

    // Відображення інформації про TreeSet
    private <T> void displayTreeSetInfo(TreeSet<T> treeSet, String name) {
        System.out.println(name + ":");
        System.out.println("  Розмір: " + treeSet.size());
        System.out.println("  Порожній: " + (treeSet.isEmpty() ? "Так" : "Ні"));

        if (!treeSet.isEmpty()) {
            System.out.println("  Перший елемент: " + treeSet.first());
            System.out.println("  Останній елемент: " + treeSet.last());
            System.out.println("  Всі елементи:");
            int index = 1;
            for (T element : treeSet) {
                System.out.printf("    %d. %s\n", index++, element);
            }
        }
        System.out.println();
    }

    // Відображення інформації про складні кольори
    private void displayComplexColorsInfo() {
        if (complexColors.isEmpty()) {
            System.out.println("TreeSet порожній");
            return;
        }

        System.out.printf("Розмір: %d кольорів\n", complexColors.size());
        System.out.println("Кольори (відсортовані):");

        int index = 1;
        String currentCategory = "";
        for (Color color : complexColors) {
            if (!color.getCategory().equals(currentCategory)) {
                currentCategory = color.getCategory();
                System.out.printf("\n  === %s кольори ===\n", currentCategory);
            }
            System.out.printf("    %d. %s\n", index++, color);
        }
        System.out.println();
    }

    // Демонстрація навігаційних методів TreeSet
    private void demonstrateNavigationMethods() {
        System.out.println("\n🧭 Навігаційні методи TreeSet:");

        if (simpleColors.isEmpty()) {
            System.out.println("TreeSet порожній");
            return;
        }

        String testColor = "Зелений";

        System.out.printf("Тестовий колір: %s\n", testColor);
        System.out.printf("  lower(\"%s\"): %s\n", testColor, simpleColors.lower(testColor));
        System.out.printf("  floor(\"%s\"): %s\n", testColor, simpleColors.floor(testColor));
        System.out.printf("  ceiling(\"%s\"): %s\n", testColor, simpleColors.ceiling(testColor));
        System.out.printf("  higher(\"%s\"): %s\n", testColor, simpleColors.higher(testColor));

        // Підмножини
        String fromColor = "Ж";
        String toColor = "С";
        System.out.printf("\nПідмножина від \"%s\" до \"%s\":\n", fromColor, toColor);
        TreeSet<String> subset = new TreeSet<>(simpleColors.subSet(fromColor, toColor));
        for (String color : subset) {
            System.out.println("    " + color);
        }

        // Головна та хвостова частини
        System.out.printf("\nГоловна частина (до \"%s\"):\n", "Ж");
        TreeSet<String> headSet = new TreeSet<>(simpleColors.headSet("Ж"));
        for (String color : headSet) {
            System.out.println("    " + color);
        }

        System.out.printf("\nХвостова частина (від \"%s\"):\n", "С");
        TreeSet<String> tailSet = new TreeSet<>(simpleColors.tailSet("С"));
        for (String color : tailSet) {
            System.out.println("    " + color);
        }
    }

    // Статистика складних кольорів
    private void showComplexColorStatistics() {
        System.out.println("📊 Статистика кольорів:");

        // Групування за категоріями
        Map<String, Long> categoryCount = complexColors.stream()
            .collect(java.util.stream.Collectors.groupingBy(
                Color::getCategory,
                java.util.stream.Collectors.counting()));

        System.out.println("За категоріями:");
        for (Map.Entry<String, Long> entry : categoryCount.entrySet()) {
            System.out.printf("  %s: %d кольорів\n", entry.getKey(), entry.getValue());
        }

        // Статистика інтенсивності
        OptionalDouble avgIntensity = complexColors.stream()
            .mapToInt(Color::getIntensity)
            .average();

        int minIntensity = complexColors.stream()
            .mapToInt(Color::getIntensity)
            .min().orElse(0);

        int maxIntensity = complexColors.stream()
            .mapToInt(Color::getIntensity)
            .max().orElse(0);

        System.out.printf("Інтенсивність: мін=%d, макс=%d, середня=%.1f\n",
            minIntensity, maxIntensity, avgIntensity.orElse(0.0));
    }

    // Групування за довжиною назви
    private void showLengthGrouping() {
        System.out.println("\n📏 Групування за довжиною назви:");

        Map<Integer, List<String>> lengthGroups = new TreeMap<>();

        for (String color : customSortedColors) {
            int length = color.length();
            lengthGroups.computeIfAbsent(length, k -> new ArrayList<>()).add(color);
        }

        for (Map.Entry<Integer, List<String>> entry : lengthGroups.entrySet()) {
            System.out.printf("  %d символів (%d кольорів): %s\n",
                entry.getKey(), entry.getValue().size(), entry.getValue());
        }
    }

    // Демонстрація операцій з TreeSet
    public void demonstrateTreeSetOperations() {
        System.out.println("\n🛠️ ДЕМОНСТРАЦІЯ: Операції з TreeSet");
        System.out.println("=" .repeat(50));

        TreeSet<Integer> numbers = new TreeSet<>();

        // Додавання чисел у довільному порядку
        Integer[] numbersToAdd = {5, 2, 8, 1, 9, 3, 7, 4, 6, 10};

        System.out.println("Додавання чисел у довільному порядку:");
        for (Integer num : numbersToAdd) {
            numbers.add(num);
            System.out.printf("  Додано %d, TreeSet тепер: %s\n", num, numbers);
        }

        System.out.println("\nОперації з числами:");
        System.out.printf("  Розмір: %d\n", numbers.size());
        System.out.printf("  Мінімум: %d\n", numbers.first());
        System.out.printf("  Максимум: %d\n", numbers.last());
        System.out.printf("  Містить 5: %s\n", numbers.contains(5) ? "Так" : "Ні");
        System.out.printf("  Містить 15: %s\n", numbers.contains(15) ? "Так" : "Ні");

        // Видалення елементів
        System.out.println("\nВидалення елементів:");
        System.out.printf("  Видалено 5: %s\n", numbers.remove(5) ? "Так" : "Ні");
        System.out.printf("  TreeSet після видалення: %s\n", numbers);

        // Видалення першого та останнього
        System.out.printf("  Видалено перший (%d): %s\n", numbers.first(), numbers.pollFirst());
        System.out.printf("  Видалено останній (%d): %s\n", numbers.last(), numbers.pollLast());
        System.out.printf("  TreeSet після видалення: %s\n", numbers);
    }

    // Інтерактивний режим
    public void interactiveMode() {
        Scanner scanner = new Scanner(System.in);

        System.out.println("\n🎮 ІНТЕРАКТИВНИЙ РЕЖИМ");
        System.out.println("=" .repeat(30));

        while (true) {
            System.out.println("\nОберіть опцію:");
            System.out.println("1. Додати простий колір");
            System.out.println("2. Додати складний колір");
            System.out.println("3. Показати всі TreeSet-и");
            System.out.println("4. Пошук кольору");
            System.out.println("5. Видалити колір");
            System.out.println("6. Статистика");
            System.out.println("0. Вийти");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();
                scanner.nextLine(); // споживаємо новий рядок

                switch (choice) {
                    case 1:
                        System.out.print("Введіть назву кольору: ");
                        String colorName = scanner.nextLine();
                        boolean added = simpleColors.add(colorName);
                        System.out.printf("Колір \"%s\": %s\n", colorName,
                            added ? "✅ Додано" : "⚠️ Вже існує");
                        break;

                    case 2:
                        System.out.print("Введіть назву кольору: ");
                        String name = scanner.nextLine();
                        System.out.print("Введіть hex код (наприклад, #FF0000): ");
                        String hex = scanner.nextLine();
                        System.out.print("Введіть категорію (Теплий/Холодний/Природний/Нейтральний): ");
                        String category = scanner.nextLine();
                        System.out.print("Введіть інтенсивність (1-10): ");
                        int intensity = scanner.nextInt();

                        Color newColor = new Color(name, hex, category, intensity);
                        boolean addedComplex = complexColors.add(newColor);
                        System.out.printf("Колір \"%s\": %s\n", name,
                            addedComplex ? "✅ Додано" : "⚠️ Вже існує");
                        break;

                    case 3:
                        displayTreeSetInfo(simpleColors, "Прості кольори");
                        displayComplexColorsInfo();
                        displayTreeSetInfo(customSortedColors, "Кольори за довжиною");
                        break;

                    case 4:
                        System.out.print("Введіть назву кольору для пошуку: ");
                        String searchColor = scanner.nextLine();

                        boolean foundSimple = simpleColors.contains(searchColor);
                        boolean foundCustom = customSortedColors.contains(searchColor);

                        System.out.printf("У простих кольорах: %s\n",
                            foundSimple ? "✅ Знайдено" : "❌ Не знайдено");
                        System.out.printf("У кольорах за довжиною: %s\n",
                            foundCustom ? "✅ Знайдено" : "❌ Не знайдено");

                        // Пошук в складних кольорах
                        boolean foundComplex = complexColors.stream()
                            .anyMatch(c -> c.getName().equalsIgnoreCase(searchColor));
                        System.out.printf("У складних кольорах: %s\n",
                            foundComplex ? "✅ Знайдено" : "❌ Не знайдено");
                        break;

                    case 5:
                        System.out.print("Введіть назву кольору для видалення: ");
                        String removeColor = scanner.nextLine();

                        boolean removedSimple = simpleColors.remove(removeColor);
                        boolean removedCustom = customSortedColors.remove(removeColor);

                        // Видалення зі складних кольорів
                        boolean removedComplex = complexColors.removeIf(
                            c -> c.getName().equalsIgnoreCase(removeColor));

                        System.out.printf("Результати видалення \"%s\":\n", removeColor);
                        System.out.printf("  Прості кольори: %s\n",
                            removedSimple ? "✅ Видалено" : "❌ Не знайдено");
                        System.out.printf("  Кольори за довжиною: %s\n",
                            removedCustom ? "✅ Видалено" : "❌ Не знайдено");
                        System.out.printf("  Складні кольори: %s\n",
                            removedComplex ? "✅ Видалено" : "❌ Не знайдено");
                        break;

                    case 6:
                        System.out.printf("📊 Загальна статистика:\n");
                        System.out.printf("  Простих кольорів: %d\n", simpleColors.size());
                        System.out.printf("  Складних кольорів: %d\n", complexColors.size());
                        System.out.printf("  Кольорів за довжиною: %d\n", customSortedColors.size());

                        if (!complexColors.isEmpty()) {
                            showComplexColorStatistics();
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
        System.out.println("=== Лабораторна робота 3 - Завдання 3 ===");
        System.out.println("TreeSet: створення нового TreeSet з кольорами");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        Lr3_3 program = new Lr3_3();

        // Демонстрація основного функціоналу
        program.demonstrateSimpleColors();
        program.demonstrateComplexColors();
        program.demonstrateCustomSortedColors();
        program.demonstrateTreeSetOperations();

        // Інтерактивний режим
        program.interactiveMode();

        System.out.println("\n✅ Програма завершена успішно!");
    }
}