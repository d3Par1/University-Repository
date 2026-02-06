// Лабораторна робота 4 - Завдання 2
// Map: зберігання замовлень з продуктами
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.*;
import java.io.*;
import java.text.SimpleDateFormat;

// Клас продукту
class Product {
    private String name;
    private double quantity;
    private String unit; // штуки, кг, л, г, тощо
    private double price; // ціна за одиницю

    public Product(String name, double quantity, String unit, double price) {
        this.name = name;
        this.quantity = quantity;
        this.unit = unit;
        this.price = price;
    }

    // Геттери
    public String getName() { return name; }
    public double getQuantity() { return quantity; }
    public String getUnit() { return unit; }
    public double getPrice() { return price; }

    // Сеттери
    public void setName(String name) { this.name = name; }
    public void setQuantity(double quantity) { this.quantity = quantity; }
    public void setUnit(String unit) { this.unit = unit; }
    public void setPrice(double price) { this.price = price; }

    // Розрахунок загальної вартості продукту
    public double getTotalCost() {
        return quantity * price;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Product product = (Product) obj;
        return Objects.equals(name, product.name) && Objects.equals(unit, product.unit);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, unit);
    }

    @Override
    public String toString() {
        return String.format("%-20s | %8.2f %-5s | %8.2f грн/%-5s | %10.2f грн",
            name, quantity, unit, price, unit, getTotalCost());
    }

    public String getDetailedInfo() {
        return String.format(
            "Назва: %s\nКількість: %.2f %s\nЦіна за одиницю: %.2f грн/%s\nЗагальна вартість: %.2f грн\n",
            name, quantity, unit, price, unit, getTotalCost());
    }
}

// Клас замовлення
class Order {
    private Integer orderNumber;
    private List<Product> products;
    private Date orderDate;
    private String customerName;
    private String status;

    public Order(Integer orderNumber, String customerName) {
        this.orderNumber = orderNumber;
        this.customerName = customerName;
        this.products = new ArrayList<>();
        this.orderDate = new Date();
        this.status = "Нове";
    }

    // Геттери
    public Integer getOrderNumber() { return orderNumber; }
    public List<Product> getProducts() { return new ArrayList<>(products); }
    public Date getOrderDate() { return orderDate; }
    public String getCustomerName() { return customerName; }
    public String getStatus() { return status; }

    // Сеттери
    public void setCustomerName(String customerName) { this.customerName = customerName; }
    public void setStatus(String status) { this.status = status; }

    // Додавання продукту
    public void addProduct(Product product) {
        // Перевіряємо, чи є вже такий продукт в замовленні
        for (int i = 0; i < products.size(); i++) {
            Product existing = products.get(i);
            if (existing.equals(product)) {
                // Якщо продукт вже є, збільшуємо кількість
                existing.setQuantity(existing.getQuantity() + product.getQuantity());
                return;
            }
        }
        // Якщо продукту немає, додаємо новий
        products.add(product);
    }

    // Видалення продукту
    public boolean removeProduct(String productName) {
        return products.removeIf(p -> p.getName().equalsIgnoreCase(productName));
    }

    // Розрахунок загальної вартості замовлення
    public double getTotalOrderCost() {
        return products.stream().mapToDouble(Product::getTotalCost).sum();
    }

    // Кількість різних продуктів
    public int getProductCount() {
        return products.size();
    }

    // Загальна кількість одиниць товару
    public double getTotalQuantity() {
        return products.stream().mapToDouble(Product::getQuantity).sum();
    }

    @Override
    public String toString() {
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd.MM.yyyy HH:mm");
        return String.format("Замовлення #%d | %s | %s | %d продуктів | %.2f грн | %s",
            orderNumber, customerName, dateFormat.format(orderDate),
            getProductCount(), getTotalOrderCost(), status);
    }

    public String getDetailedInfo() {
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd.MM.yyyy HH:mm:ss");
        StringBuilder sb = new StringBuilder();

        sb.append(String.format("=== ЗАМОВЛЕННЯ #%d ===\n", orderNumber));
        sb.append(String.format("Замовник: %s\n", customerName));
        sb.append(String.format("Дата: %s\n", dateFormat.format(orderDate)));
        sb.append(String.format("Статус: %s\n", status));
        sb.append(String.format("Кількість продуктів: %d\n", getProductCount()));
        sb.append("\nПРОДУКТИ:\n");
        sb.append("-".repeat(70)).append("\n");
        sb.append("Назва                | Кількість  | Ціна           | Сума\n");
        sb.append("-".repeat(70)).append("\n");

        for (Product product : products) {
            sb.append(product).append("\n");
        }

        sb.append("-".repeat(70)).append("\n");
        sb.append(String.format("ЗАГАЛЬНА ВАРТІСТЬ: %.2f грн\n", getTotalOrderCost()));
        sb.append("=".repeat(30)).append("\n");

        return sb.toString();
    }
}

// Головний клас для управління замовленнями
public class Lr4_2 {
    private Map<Integer, Order> orders;
    private Random random;
    private int nextOrderNumber;

    public Lr4_2() {
        orders = new HashMap<>();
        random = new Random();
        nextOrderNumber = 1001;
        initializeWithSampleData();
    }

    // Ініціалізація з тестовими даними
    private void initializeWithSampleData() {
        // Замовлення 1
        Order order1 = createOrder("Олександр Петренко");
        order1.addProduct(new Product("Хліб білий", 2, "шт", 15.50));
        order1.addProduct(new Product("Молоко", 1, "л", 28.90));
        order1.addProduct(new Product("Яйця", 10, "шт", 3.20));
        order1.addProduct(new Product("Масло", 0.5, "кг", 89.00));

        // Замовлення 2
        Order order2 = createOrder("Марія Коваленко");
        order2.addProduct(new Product("М'ясо яловиче", 1.2, "кг", 280.00));
        order2.addProduct(new Product("Картопля", 3, "кг", 12.50));
        order2.addProduct(new Product("Цибуля", 1, "кг", 18.00));
        order2.addProduct(new Product("Морква", 0.8, "кг", 22.00));
        order2.addProduct(new Product("Сметана", 1, "шт", 35.60));

        // Замовлення 3
        Order order3 = createOrder("Дмитро Шевченко");
        order3.addProduct(new Product("Курка", 1.5, "кг", 95.00));
        order3.addProduct(new Product("Рис", 1, "кг", 45.00));
        order3.addProduct(new Product("Олія соняшникова", 1, "л", 52.30));
        order3.addProduct(new Product("Сіль", 1, "кг", 8.50));
        order3.addProduct(new Product("Перець чорний", 0.1, "кг", 180.00));

        System.out.println("✅ Початкові дані завантажено: " + orders.size() + " замовлень");
    }

    // Створення нового замовлення
    public Order createOrder(String customerName) {
        Order order = new Order(nextOrderNumber++, customerName);
        orders.put(order.getOrderNumber(), order);
        System.out.println("✅ Створено замовлення #" + order.getOrderNumber() +
                          " для " + customerName);
        return order;
    }

    // Додавання замовлення
    public void addOrder(Order order) {
        if (order == null) {
            System.out.println("❌ Помилка: замовлення не може бути null");
            return;
        }

        orders.put(order.getOrderNumber(), order);
        System.out.println("✅ Замовлення додано: #" + order.getOrderNumber());
    }

    // Видалення замовлення
    public boolean removeOrder(Integer orderNumber) {
        Order removed = orders.remove(orderNumber);
        if (removed != null) {
            System.out.println("✅ Замовлення #" + orderNumber + " видалено");
            return true;
        } else {
            System.out.println("❌ Замовлення #" + orderNumber + " не знайдено");
            return false;
        }
    }

    // Пошук замовлення за номером
    public Order findOrderByNumber(Integer orderNumber) {
        return orders.get(orderNumber);
    }

    // Пошук замовлень за ім'ям замовника
    public List<Order> findOrdersByCustomer(String customerName) {
        List<Order> result = new ArrayList<>();
        for (Order order : orders.values()) {
            if (order.getCustomerName().toLowerCase().contains(customerName.toLowerCase())) {
                result.add(order);
            }
        }
        return result;
    }

    // Пошук замовлень за статусом
    public List<Order> findOrdersByStatus(String status) {
        List<Order> result = new ArrayList<>();
        for (Order order : orders.values()) {
            if (order.getStatus().equalsIgnoreCase(status)) {
                result.add(order);
            }
        }
        return result;
    }

    // Пошук замовлень за сумою
    public List<Order> findOrdersByPriceRange(double minPrice, double maxPrice) {
        List<Order> result = new ArrayList<>();
        for (Order order : orders.values()) {
            double total = order.getTotalOrderCost();
            if (total >= minPrice && total <= maxPrice) {
                result.add(order);
            }
        }
        return result;
    }

    // Пошук замовлень з конкретним продуктом
    public List<Order> findOrdersWithProduct(String productName) {
        List<Order> result = new ArrayList<>();
        for (Order order : orders.values()) {
            for (Product product : order.getProducts()) {
                if (product.getName().toLowerCase().contains(productName.toLowerCase())) {
                    result.add(order);
                    break;
                }
            }
        }
        return result;
    }

    // Виведення всіх замовлень
    public void displayAllOrders() {
        System.out.println("\n📋 СПИСОК ЗАМОВЛЕНЬ (" + orders.size() + " шт)");
        System.out.println("=" .repeat(80));

        if (orders.isEmpty()) {
            System.out.println("Список замовлень порожній");
            return;
        }

        // Сортуємо замовлення за номером
        List<Order> sortedOrders = new ArrayList<>(orders.values());
        sortedOrders.sort(Comparator.comparing(Order::getOrderNumber));

        for (Order order : sortedOrders) {
            System.out.println(order);
        }

        System.out.println("-" .repeat(80));
        double totalValue = orders.values().stream()
                .mapToDouble(Order::getTotalOrderCost).sum();
        System.out.printf("Загальна вартість всіх замовлень: %.2f грн\n", totalValue);
    }

    // Детальний перегляд замовлення
    public void displayOrderDetails(Integer orderNumber) {
        Order order = orders.get(orderNumber);
        if (order != null) {
            System.out.println("\n" + order.getDetailedInfo());
        } else {
            System.out.println("❌ Замовлення #" + orderNumber + " не знайдено");
        }
    }

    // Статистика замовлень
    public void displayOrderStatistics() {
        System.out.println("\n📊 СТАТИСТИКА ЗАМОВЛЕНЬ");
        System.out.println("=" .repeat(40));

        if (orders.isEmpty()) {
            System.out.println("Немає даних для статистики");
            return;
        }

        // Загальна статистика
        int totalOrders = orders.size();
        double totalValue = orders.values().stream()
                .mapToDouble(Order::getTotalOrderCost).sum();
        double avgOrderValue = totalValue / totalOrders;

        double maxOrderValue = orders.values().stream()
                .mapToDouble(Order::getTotalOrderCost).max().orElse(0);
        double minOrderValue = orders.values().stream()
                .mapToDouble(Order::getTotalOrderCost).min().orElse(0);

        int totalProducts = orders.values().stream()
                .mapToInt(Order::getProductCount).sum();
        double avgProductsPerOrder = (double) totalProducts / totalOrders;

        System.out.printf("Загальна кількість замовлень: %d\n", totalOrders);
        System.out.printf("Загальна вартість: %.2f грн\n", totalValue);
        System.out.printf("Середня вартість замовлення: %.2f грн\n", avgOrderValue);
        System.out.printf("Мінімальна вартість: %.2f грн\n", minOrderValue);
        System.out.printf("Максимальна вартість: %.2f грн\n", maxOrderValue);
        System.out.printf("Загальна кількість продуктів: %d\n", totalProducts);
        System.out.printf("Середня к-сть продуктів в замовленні: %.1f\n", avgProductsPerOrder);

        // Статистика по статусах
        System.out.println("\nСтатистика по статусах:");
        Map<String, Long> statusCount = orders.values().stream()
                .collect(java.util.stream.Collectors.groupingBy(
                    Order::getStatus,
                    java.util.stream.Collectors.counting()));

        for (Map.Entry<String, Long> entry : statusCount.entrySet()) {
            System.out.printf("  %s: %d замовлень\n", entry.getKey(), entry.getValue());
        }

        // Топ продуктів
        System.out.println("\nНайпопулярніші продукти:");
        Map<String, Integer> productCount = new HashMap<>();
        for (Order order : orders.values()) {
            for (Product product : order.getProducts()) {
                productCount.put(product.getName(),
                    productCount.getOrDefault(product.getName(), 0) + 1);
            }
        }

        productCount.entrySet().stream()
            .sorted(Map.Entry.<String, Integer>comparingByValue().reversed())
            .limit(5)
            .forEach(entry ->
                System.out.printf("  %s: %d замовлень\n", entry.getKey(), entry.getValue()));
    }

    // Модифікація замовлення
    public void modifyOrder(Integer orderNumber) {
        Order order = orders.get(orderNumber);
        if (order == null) {
            System.out.println("❌ Замовлення #" + orderNumber + " не знайдено");
            return;
        }

        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n📝 МОДИФІКАЦІЯ ЗАМОВЛЕННЯ #" + orderNumber);
            System.out.println("1. Додати продукт");
            System.out.println("2. Видалити продукт");
            System.out.println("3. Змінити статус");
            System.out.println("4. Показати замовлення");
            System.out.println("0. Повернутись");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();
                scanner.nextLine();

                switch (choice) {
                    case 1:
                        System.out.print("Назва продукту: ");
                        String name = scanner.nextLine();
                        System.out.print("Кількість: ");
                        double quantity = scanner.nextDouble();
                        scanner.nextLine();
                        System.out.print("Одиниця виміру: ");
                        String unit = scanner.nextLine();
                        System.out.print("Ціна за одиницю: ");
                        double price = scanner.nextDouble();

                        Product product = new Product(name, quantity, unit, price);
                        order.addProduct(product);
                        System.out.println("✅ Продукт додано");
                        break;

                    case 2:
                        System.out.print("Назва продукту для видалення: ");
                        String removeName = scanner.nextLine();
                        boolean removed = order.removeProduct(removeName);
                        System.out.println(removed ? "✅ Продукт видалено" : "❌ Продукт не знайдено");
                        break;

                    case 3:
                        System.out.print("Новий статус (Нове/В обробці/Відправлено/Доставлено/Скасовано): ");
                        String newStatus = scanner.nextLine();
                        order.setStatus(newStatus);
                        System.out.println("✅ Статус змінено");
                        break;

                    case 4:
                        System.out.println(order.getDetailedInfo());
                        break;

                    case 0:
                        return;

                    default:
                        System.out.println("❌ Невірний вибір!");
                }
            } catch (Exception e) {
                System.out.println("❌ Помилка введення!");
                scanner.nextLine();
            }
        }
    }

    // Збереження у файл
    public void saveToFile(String filename) {
        try (PrintWriter writer = new PrintWriter(new FileWriter(filename))) {
            writer.println("=== ЗВІТ ПО ЗАМОВЛЕННЯХ ===");
            writer.println("Дата створення: " + new SimpleDateFormat("dd.MM.yyyy HH:mm:ss").format(new Date()));
            writer.println("Студент: групи ТВ-43, прізвище Step, варіант 16");
            writer.println();

            writer.println("ПОЧАТКОВІ ДАНІ:");
            List<Order> sortedOrders = new ArrayList<>(orders.values());
            sortedOrders.sort(Comparator.comparing(Order::getOrderNumber));

            for (Order order : sortedOrders) {
                writer.println(order.getDetailedInfo());
            }

            writer.println("РЕЗУЛЬТАТИ РОБОТИ ПРОГРАМИ:");
            if (!orders.isEmpty()) {
                double totalValue = orders.values().stream()
                        .mapToDouble(Order::getTotalOrderCost).sum();
                writer.printf("Загальна кількість замовлень: %d\n", orders.size());
                writer.printf("Загальна вартість: %.2f грн\n", totalValue);
            }

            System.out.println("✅ Дані збережено у файл: " + filename);
        } catch (IOException e) {
            System.out.println("❌ Помилка збереження файлу: " + e.getMessage());
        }
    }

    // Генерація випадкових замовлень
    public void generateRandomOrders(int count) {
        String[] customers = {
            "Іван Петров", "Ольга Сидорова", "Микола Іваненко", "Тетяна Коваль",
            "Андрій Мельник", "Світлана Шевченко", "Володимир Бондар", "Наталія Козлова"
        };

        String[] productNames = {
            "Хліб", "Молоко", "Яйця", "М'ясо", "Риба", "Сир", "Масло", "Картопля",
            "Цибуля", "Морква", "Капуста", "Помідори", "Огірки", "Яблука", "Банани"
        };

        String[] units = {"шт", "кг", "л", "г"};
        String[] statuses = {"Нове", "В обробці", "Відправлено", "Доставлено"};

        System.out.printf("Генерація %d випадкових замовлень...\n", count);

        for (int i = 0; i < count; i++) {
            String customer = customers[random.nextInt(customers.length)];
            Order order = createOrder(customer);

            // Додаємо випадкову кількість продуктів (1-5)
            int productCount = 1 + random.nextInt(5);
            Set<String> usedProducts = new HashSet<>();

            for (int j = 0; j < productCount; j++) {
                String productName;
                do {
                    productName = productNames[random.nextInt(productNames.length)];
                } while (usedProducts.contains(productName));

                usedProducts.add(productName);

                double quantity = 0.1 + random.nextDouble() * 3; // 0.1-3.1
                String unit = units[random.nextInt(units.length)];
                double price = 5 + random.nextDouble() * 295; // 5-300

                Product product = new Product(productName, quantity, unit, price);
                order.addProduct(product);
            }

            order.setStatus(statuses[random.nextInt(statuses.length)]);
        }
    }

    // Інтерактивне меню
    public void showInteractiveMenu() {
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n🎮 МЕНЮ УПРАВЛІННЯ ЗАМОВЛЕННЯМИ");
            System.out.println("=" .repeat(40));
            System.out.println("1. Показати всі замовлення");
            System.out.println("2. Створити замовлення");
            System.out.println("3. Детальний перегляд замовлення");
            System.out.println("4. Модифікувати замовлення");
            System.out.println("5. Видалити замовлення");
            System.out.println("6. Пошук замовлень");
            System.out.println("7. Статистика");
            System.out.println("8. Генерувати випадкові замовлення");
            System.out.println("9. Зберегти у файл");
            System.out.println("0. Вийти");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();
                scanner.nextLine();

                switch (choice) {
                    case 1:
                        displayAllOrders();
                        break;

                    case 2:
                        System.out.print("Ім'я замовника: ");
                        String customer = scanner.nextLine();
                        Order newOrder = createOrder(customer);
                        modifyOrder(newOrder.getOrderNumber());
                        break;

                    case 3:
                        System.out.print("Номер замовлення: ");
                        Integer orderNum = scanner.nextInt();
                        displayOrderDetails(orderNum);
                        break;

                    case 4:
                        System.out.print("Номер замовлення для модифікації: ");
                        Integer modifyNum = scanner.nextInt();
                        modifyOrder(modifyNum);
                        break;

                    case 5:
                        System.out.print("Номер замовлення для видалення: ");
                        Integer removeNum = scanner.nextInt();
                        removeOrder(removeNum);
                        break;

                    case 6:
                        System.out.println("Пошук за:");
                        System.out.println("1 - Замовником, 2 - Статусом, 3 - Продуктом");
                        int searchType = scanner.nextInt();
                        scanner.nextLine();

                        if (searchType == 1) {
                            System.out.print("Ім'я замовника: ");
                            String searchCustomer = scanner.nextLine();
                            List<Order> found = findOrdersByCustomer(searchCustomer);
                            if (!found.isEmpty()) {
                                System.out.println("✅ Знайдено " + found.size() + " замовлень:");
                                found.forEach(System.out::println);
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        } else if (searchType == 2) {
                            System.out.print("Статус: ");
                            String status = scanner.nextLine();
                            List<Order> found = findOrdersByStatus(status);
                            if (!found.isEmpty()) {
                                System.out.println("✅ Знайдено " + found.size() + " замовлень:");
                                found.forEach(System.out::println);
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        } else if (searchType == 3) {
                            System.out.print("Назва продукту: ");
                            String product = scanner.nextLine();
                            List<Order> found = findOrdersWithProduct(product);
                            if (!found.isEmpty()) {
                                System.out.println("✅ Знайдено " + found.size() + " замовлень:");
                                found.forEach(System.out::println);
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        }
                        break;

                    case 7:
                        displayOrderStatistics();
                        break;

                    case 8:
                        System.out.print("Кількість замовлень для генерації: ");
                        int count = scanner.nextInt();
                        generateRandomOrders(count);
                        break;

                    case 9:
                        System.out.print("Ім'я файлу (наприклад, orders.txt): ");
                        String filename = scanner.nextLine();
                        saveToFile(filename);
                        break;

                    case 0:
                        return;

                    default:
                        System.out.println("❌ Невірний вибір!");
                }
            } catch (Exception e) {
                System.out.println("❌ Помилка введення!");
                scanner.nextLine();
            }
        }
    }

    public static void main(String[] args) {
        System.out.println("=== Лабораторна робота 4 - Завдання 2 ===");
        System.out.println("Map: система управління замовленнями з продуктами");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        Lr4_2 program = new Lr4_2();

        // Демонстрація основного функціоналу
        program.displayAllOrders();
        program.displayOrderStatistics();

        // Демонстрація детального перегляду
        System.out.println("\n🔍 ДЕМОНСТРАЦІЯ: Детальний перегляд замовлення");
        program.displayOrderDetails(1001);

        // Демонстрація пошуку
        System.out.println("\n🔍 ДЕМОНСТРАЦІЯ: Пошук замовлень з м'ясом");
        List<Order> foundOrders = program.findOrdersWithProduct("м'ясо");
        foundOrders.forEach(System.out::println);

        // Збереження у файл
        program.saveToFile("orders_output.txt");

        // Інтерактивне меню
        program.showInteractiveMenu();

        System.out.println("\n✅ Програма завершена успішно!");
    }
}