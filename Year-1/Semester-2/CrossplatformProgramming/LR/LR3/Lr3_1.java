// Лабораторна робота 3 - Завдання 1
// TreeMap: отримання відображення ключ-значення з найбільшим ключем, строго меншим за даний
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.*;

public class Lr3_1 {
    private TreeMap<Integer, String> studentGrades;
    private TreeMap<String, Double> productPrices;
    private TreeMap<Date, String> events;

    public Lr3_1() {
        initializeCollections();
    }

    // Ініціалізація тестових даних
    private void initializeCollections() {
        // TreeMap з оцінками студентів (ключ - оцінка, значення - ім'я студента)
        studentGrades = new TreeMap<>();
        studentGrades.put(100, "Марія Відмінниця");
        studentGrades.put(95, "Олександр Старанний");
        studentGrades.put(90, "Катерина Розумна");
        studentGrades.put(85, "Дмитро Здібний");
        studentGrades.put(80, "Анна Працьовита");
        studentGrades.put(75, "Володимир Намагається");
        studentGrades.put(70, "Ольга Стандартна");
        studentGrades.put(65, "Сергій Мінімальний");
        studentGrades.put(60, "Тетяна Межова");
        studentGrades.put(55, "Андрій Слабкий");

        // TreeMap з цінами товарів (ключ - ціна, значення - назва товару)
        productPrices = new TreeMap<>();
        productPrices.put("Молоко", 25.50);
        productPrices.put("Хліб", 15.00);
        productPrices.put("Яйця", 35.80);
        productPrices.put("Масло", 89.99);
        productPrices.put("Сир", 120.00);
        productPrices.put("М'ясо", 250.75);
        productPrices.put("Риба", 180.40);
        productPrices.put("Овочі", 45.30);
        productPrices.put("Фрукти", 68.20);
        productPrices.put("Напої", 32.10);

        // TreeMap з подіями (ключ - дата, значення - опис події)
        events = new TreeMap<>();
        Calendar cal = Calendar.getInstance();

        cal.set(2024, Calendar.JANUARY, 15);
        events.put(cal.getTime(), "Новорічні канікули");

        cal.set(2024, Calendar.FEBRUARY, 14);
        events.put(cal.getTime(), "День Святого Валентина");

        cal.set(2024, Calendar.MARCH, 8);
        events.put(cal.getTime(), "Міжнародний жіночий день");

        cal.set(2024, Calendar.MAY, 1);
        events.put(cal.getTime(), "День праці");

        cal.set(2024, Calendar.MAY, 9);
        events.put(cal.getTime(), "День Перемоги");

        cal.set(2024, Calendar.AUGUST, 24);
        events.put(cal.getTime(), "День Незалежності України");

        cal.set(2024, Calendar.OCTOBER, 31);
        events.put(cal.getTime(), "Хелловін");

        cal.set(2024, Calendar.DECEMBER, 25);
        events.put(cal.getTime(), "Різдво");

        cal.set(2024, Calendar.DECEMBER, 31);
        events.put(cal.getTime(), "Новий рік");
    }

    // Основний метод для пошуку відображення ключ-значення з найбільшим ключем, строго меншим за даний
    public <K, V> Map.Entry<K, V> getLowerEntry(TreeMap<K, V> map, K key) {
        if (map == null || key == null) {
            System.out.println("❌ Помилка: TreeMap або ключ не можуть бути null");
            return null;
        }

        Map.Entry<K, V> result = map.lowerEntry(key);

        if (result == null) {
            System.out.println("⚠️ Не знайдено ключа, строго меншого за " + key);
            return null;
        }

        System.out.printf("✅ Знайдено: ключ = %s, значення = %s\n", result.getKey(), result.getValue());
        return result;
    }

    // Демонстрація роботи з оцінками студентів
    public void demonstrateStudentGrades() {
        System.out.println("\n🎓 ДЕМОНСТРАЦІЯ: Оцінки студентів");
        System.out.println("=" .repeat(50));

        // Відображення всієї TreeMap
        System.out.println("Всі студенти та їх оцінки:");
        for (Map.Entry<Integer, String> entry : studentGrades.entrySet()) {
            System.out.printf("  %d балів: %s\n", entry.getKey(), entry.getValue());
        }

        // Тестування різних випадків
        int[] testGrades = {92, 85, 55, 101, 50};

        System.out.println("\nПошук студентів з найвищою оцінкою, меншою за задану:");
        for (int grade : testGrades) {
            System.out.printf("\nПошук для оцінки %d:\n", grade);
            Map.Entry<Integer, String> result = getLowerEntry(studentGrades, grade);

            if (result != null) {
                System.out.printf("→ Найближчий кращий результат: %s з оцінкою %d\n",
                    result.getValue(), result.getKey());
            }
        }
    }

    // Демонстрація роботи з цінами товарів
    public void demonstrateProductPrices() {
        System.out.println("\n🛒 ДЕМОНСТРАЦІЯ: Ціни товарів");
        System.out.println("=" .repeat(50));

        // Створюємо TreeMap з цінами як ключами для демонстрації
        TreeMap<Double, String> priceMap = new TreeMap<>();
        for (Map.Entry<String, Double> entry : productPrices.entrySet()) {
            priceMap.put(entry.getValue(), entry.getKey());
        }

        // Відображення всієї TreeMap
        System.out.println("Товари, відсортовані за ціною:");
        for (Map.Entry<Double, String> entry : priceMap.entrySet()) {
            System.out.printf("  %.2f грн: %s\n", entry.getKey(), entry.getValue());
        }

        // Тестування різних бюджетів
        double[] budgets = {50.0, 100.0, 200.0, 300.0, 10.0};

        System.out.println("\nПошук найдорожчого товару в межах бюджету:");
        for (double budget : budgets) {
            System.out.printf("\nБюджет: %.2f грн:\n", budget);
            Map.Entry<Double, String> result = getLowerEntry(priceMap, budget);

            if (result != null) {
                System.out.printf("→ Рекомендований товар: %s за %.2f грн\n",
                    result.getValue(), result.getKey());
            } else {
                System.out.println("→ Жодного товару в межах бюджету не знайдено");
            }
        }
    }

    // Демонстрація роботи з подіями
    public void demonstrateEvents() {
        System.out.println("\n📅 ДЕМОНСТРАЦІЯ: Події календаря");
        System.out.println("=" .repeat(50));

        // Відображення всіх подій
        System.out.println("Всі події календаря:");
        for (Map.Entry<Date, String> entry : events.entrySet()) {
            System.out.printf("  %s: %s\n",
                new java.text.SimpleDateFormat("dd.MM.yyyy").format(entry.getKey()),
                entry.getValue());
        }

        // Створення тестових дат
        Calendar cal = Calendar.getInstance();
        Date[] testDates = new Date[5];

        cal.set(2024, Calendar.MARCH, 1);
        testDates[0] = cal.getTime();

        cal.set(2024, Calendar.JUNE, 15);
        testDates[1] = cal.getTime();

        cal.set(2024, Calendar.SEPTEMBER, 30);
        testDates[2] = cal.getTime();

        cal.set(2024, Calendar.NOVEMBER, 15);
        testDates[3] = cal.getTime();

        cal.set(2025, Calendar.JANUARY, 1);
        testDates[4] = cal.getTime();

        System.out.println("\nПошук останньої події перед заданою датою:");
        java.text.SimpleDateFormat dateFormat = new java.text.SimpleDateFormat("dd.MM.yyyy");

        for (Date testDate : testDates) {
            System.out.printf("\nДата: %s:\n", dateFormat.format(testDate));
            Map.Entry<Date, String> result = getLowerEntry(events, testDate);

            if (result != null) {
                System.out.printf("→ Остання подія перед цією датою: %s (%s)\n",
                    result.getValue(), dateFormat.format(result.getKey()));
            } else {
                System.out.println("→ Немає подій перед цією датою");
            }
        }
    }

    // Демонстрація граничних випадків
    public void demonstrateEdgeCases() {
        System.out.println("\n⚠️ ДЕМОНСТРАЦІЯ: Граничні випадки");
        System.out.println("=" .repeat(50));

        // Порожня TreeMap
        TreeMap<Integer, String> emptyMap = new TreeMap<>();
        System.out.println("1. Пошук в порожній TreeMap:");
        getLowerEntry(emptyMap, 50);

        // TreeMap з одним елементом
        TreeMap<Integer, String> singleMap = new TreeMap<>();
        singleMap.put(100, "Єдиний елемент");
        System.out.println("\n2. Пошук в TreeMap з одним елементом:");
        System.out.println("   Елемент: 100 -> Єдиний елемент");
        System.out.println("   Пошук для ключа 50:");
        getLowerEntry(singleMap, 50);
        System.out.println("   Пошук для ключа 150:");
        getLowerEntry(singleMap, 150);

        // Пошук мінімального ключа
        System.out.println("\n3. Пошук ключа, меншого за мінімальний:");
        Integer minKey = studentGrades.firstKey();
        System.out.printf("   Мінімальний ключ: %d\n", minKey);
        System.out.printf("   Пошук для ключа %d:\n", minKey);
        getLowerEntry(studentGrades, minKey);

        // Null ключ
        System.out.println("\n4. Пошук з null ключем:");
        getLowerEntry(studentGrades, null);
    }

    // Інтерактивний режим
    public void interactiveMode() {
        Scanner scanner = new Scanner(System.in);

        System.out.println("\n🎮 ІНТЕРАКТИВНИЙ РЕЖИМ");
        System.out.println("=" .repeat(30));

        while (true) {
            System.out.println("\nОберіть опцію:");
            System.out.println("1. Пошук студента з оцінкою");
            System.out.println("2. Пошук товару за бюджетом");
            System.out.println("3. Пошук події за датою");
            System.out.println("4. Показати всі дані");
            System.out.println("0. Вийти");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();

                switch (choice) {
                    case 1:
                        System.out.print("Введіть оцінку (0-100): ");
                        int grade = scanner.nextInt();
                        System.out.printf("Пошук студента з найвищою оцінкою, меншою за %d:\n", grade);
                        getLowerEntry(studentGrades, grade);
                        break;

                    case 2:
                        System.out.print("Введіть бюджет (грн): ");
                        double budget = scanner.nextDouble();
                        TreeMap<Double, String> priceMap = new TreeMap<>();
                        for (Map.Entry<String, Double> entry : productPrices.entrySet()) {
                            priceMap.put(entry.getValue(), entry.getKey());
                        }
                        System.out.printf("Пошук товару в межах бюджету %.2f грн:\n", budget);
                        getLowerEntry(priceMap, budget);
                        break;

                    case 3:
                        System.out.print("Введіть рік (наприклад, 2024): ");
                        int year = scanner.nextInt();
                        System.out.print("Введіть місяць (1-12): ");
                        int month = scanner.nextInt();
                        System.out.print("Введіть день (1-31): ");
                        int day = scanner.nextInt();

                        Calendar cal = Calendar.getInstance();
                        cal.set(year, month - 1, day); // місяці в Calendar починаються з 0
                        Date searchDate = cal.getTime();

                        System.out.printf("Пошук останньої події перед %02d.%02d.%04d:\n", day, month, year);
                        getLowerEntry(events, searchDate);
                        break;

                    case 4:
                        demonstrateStudentGrades();
                        demonstrateProductPrices();
                        demonstrateEvents();
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
        System.out.println("=== Лабораторна робота 3 - Завдання 1 ===");
        System.out.println("TreeMap: пошук найбільшого ключа, строго меншого за даний");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        Lr3_1 program = new Lr3_1();

// Demonstration of main functionality
        program.demonstrateStudentGrades();
        program.demonstrateProductPrices();
        program.demonstrateEvents();
        program.demonstrateEdgeCases();

// Interactive mode
        program.interactiveMode();

        System.out.println("\n✅ Програма завершена успішно!");
    }
}