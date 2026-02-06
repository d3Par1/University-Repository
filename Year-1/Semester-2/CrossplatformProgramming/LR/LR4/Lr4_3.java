// Лабораторна робота 4 - Завдання 3
// TreeMap: система управління товарами в магазині з перевіркою унікальності
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.*;
import java.io.*;
import java.text.SimpleDateFormat;

// Клас товару
class Article {
    private String name;
    private float price;
    private String category;
    private String description;
    private boolean inStock;
    private Date addedDate;

    public Article(String name, float price) {
        this.name = name;
        this.price = price;
        this.category = "Загальна";
        this.description = "";
        this.inStock = true;
        this.addedDate = new Date();
    }

    public Article(String name, float price, String category, String description) {
        this.name = name;
        this.price = price;
        this.category = category;
        this.description = description;
        this.inStock = true;
        this.addedDate = new Date();
    }

    // Геттери
    public String getName() { return name; }
    public float getPrice() { return price; }
    public String getCategory() { return category; }
    public String getDescription() { return description; }
    public boolean getInStock() { return inStock; }
    public Date getAddedDate() { return addedDate; }

    // Сеттери
    public void setName(String name) { this.name = name; }
    public void setPrice(float price) { this.price = price; }
    public void setCategory(String category) { this.category = category; }
    public void setDescription(String description) { this.description = description; }
    public void setInStock(boolean inStock) { this.inStock = inStock; }
    public void setAddedDate(Date addedDate) { this.addedDate = addedDate; }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Article article = (Article) obj;
        return Objects.equals(name, article.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name);
    }

    @Override
    public String toString() {
        return String.format("%-25s | %8.2f грн | %-15s | %-10s | %s",
            name, price, category, inStock ? "В наявності" : "Відсутній",
            new SimpleDateFormat("dd.MM.yyyy").format(addedDate));
    }

    public String getDetailedInfo() {
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd.MM.yyyy HH:mm:ss");
        return String.format(
            "Назва: %s\nЦіна: %.2f грн\nКатегорія: %s\nОпис: %s\nВ наявності: %s\nДата додавання: %s\n",
            name, price, category, description.isEmpty() ? "Не вказано" : description,
            inStock ? "Так" : "Ні", dateFormat.format(addedDate));
    }
}

// Головний клас для управління товарами
public class Lr4_3 {
    private TreeMap<Integer, Article> products;
    private HashSet<String> productNames; // для перевірки унікальності назв
    private Random random;
    private int nextArticleId;

    public Lr4_3() {
        products = new TreeMap<>();
        productNames = new HashSet<>();
        random = new Random();
        nextArticleId = 1001;
        initializeWithSampleData();
    }

    // Ініціалізація з тестовими даними
    private void initializeWithSampleData() {
        // Продукти харчування
        addProduct(new Article("Хліб білий", 15.50f, "Хлібобулочні", "Свіжий білий хліб"));
        addProduct(new Article("Молоко 2.5%", 28.90f, "Молочні", "Пастеризоване молоко"));
        addProduct(new Article("Яйця курячі С1", 32.00f, "Молочні", "Десяток курячих яєць"));

        // Електроніка
        addProduct(new Article("Смартфон Samsung A54", 12500.00f, "Електроніка", "Android смартфон"));
        addProduct(new Article("Навушники JBL", 2800.00f, "Електроніка", "Бездротові навушники"));
        addProduct(new Article("Зарядний пристрій", 450.00f, "Електроніка", "USB-C зарядка"));

        // Одяг
        addProduct(new Article("Футболка чоловіча", 350.00f, "Одяг", "100% бавовна"));
        addProduct(new Article("Джинси жіночі", 1200.00f, "Одяг", "Класичні сині джинси"));
        addProduct(new Article("Кросівки Nike", 3200.00f, "Одяг", "Спортивні кросівки"));

        // Побутова хімія
        addProduct(new Article("Порошок Ariel", 180.00f, "Побутова хімія", "Пральний порошок"));
        addProduct(new Article("Рідина для миття посуду", 65.00f, "Побутова хімія", "Концентрат"));

        // Канцелярія
        addProduct(new Article("Ручка кулькова", 25.00f, "Канцелярія", "Сині чорнила"));
        addProduct(new Article("Зошит 48 аркушів", 35.00f, "Канцелярія", "В клітинку"));

        System.out.println("✅ Початкові дані завантажено: " + products.size() + " товарів");
        System.out.println("✅ Унікальні назви: " + productNames.size() + " найменувань");
    }

    // Додавання товару з перевіркою унікальності назви
    public boolean addProduct(Article article) {
        if (article == null) {
            System.out.println("❌ Помилка: товар не може бути null");
            return false;
        }

        // Перевіряємо унікальність назви
        if (productNames.contains(article.getName().toLowerCase())) {
            System.out.println("⚠️ Товар з назвою \"" + article.getName() + "\" вже існує!");
            return false;
        }

        Integer articleId = nextArticleId++;
        products.put(articleId, article);
        productNames.add(article.getName().toLowerCase());

        System.out.println("✅ Товар додано з артикулом #" + articleId + ": " + article.getName());
        return true;
    }

    // Видалення товару
    public boolean removeProduct(Integer articleId) {
        Article removed = products.remove(articleId);
        if (removed != null) {
            productNames.remove(removed.getName().toLowerCase());
            System.out.println("✅ Товар #" + articleId + " видалено: " + removed.getName());
            return true;
        } else {
            System.out.println("❌ Товар з артикулом #" + articleId + " не знайдено");
            return false;
        }
    }

    // Зміна ціни товару
    public boolean changePrice(Integer articleId, float newPrice) {
        Article article = products.get(articleId);
        if (article != null) {
            float oldPrice = article.getPrice();
            article.setPrice(newPrice);
            System.out.printf("✅ Ціна товару #%d \"%s\" змінена: %.2f грн → %.2f грн\n",
                articleId, article.getName(), oldPrice, newPrice);
            return true;
        } else {
            System.out.println("❌ Товар з артикулом #" + articleId + " не знайдено");
            return false;
        }
    }

    // Пошук товару за артикулом
    public Article findProductById(Integer articleId) {
        return products.get(articleId);
    }

    // Пошук товарів за назвою (частковий збіг)
    public List<Map.Entry<Integer, Article>> findProductsByName(String name) {
        List<Map.Entry<Integer, Article>> result = new ArrayList<>();
        String searchName = name.toLowerCase();

        for (Map.Entry<Integer, Article> entry : products.entrySet()) {
            if (entry.getValue().getName().toLowerCase().contains(searchName)) {
                result.add(entry);
            }
        }
        return result;
    }

    // Пошук товарів за категорією
    public List<Map.Entry<Integer, Article>> findProductsByCategory(String category) {
        List<Map.Entry<Integer, Article>> result = new ArrayList<>();

        for (Map.Entry<Integer, Article> entry : products.entrySet()) {
            if (entry.getValue().getCategory().equalsIgnoreCase(category)) {
                result.add(entry);
            }
        }
        return result;
    }

    // Пошук товарів за ціновим діапазоном
    public List<Map.Entry<Integer, Article>> findProductsByPriceRange(float minPrice, float maxPrice) {
        List<Map.Entry<Integer, Article>> result = new ArrayList<>();

        for (Map.Entry<Integer, Article> entry : products.entrySet()) {
            float price = entry.getValue().getPrice();
            if (price >= minPrice && price <= maxPrice) {
                result.add(entry);
            }
        }
        return result;
    }

    // Отримання товарів в наявності
    public List<Map.Entry<Integer, Article>> getAvailableProducts() {
        List<Map.Entry<Integer, Article>> result = new ArrayList<>();

        for (Map.Entry<Integer, Article> entry : products.entrySet()) {
            if (entry.getValue().getInStock()) {
                result.add(entry);
            }
        }
        return result;
    }

    // Перегляд всіх товарів
    public void displayAllProducts() {
        System.out.println("\n🏪 КАТАЛОГ ТОВАРІВ (" + products.size() + " позицій)");
        System.out.println("=" .repeat(85));

        if (products.isEmpty()) {
            System.out.println("Каталог товарів порожній");
            return;
        }

        System.out.println("Артикул | Назва                     | Ціна      | Категорія       | Статус     | Дата");
        System.out.println("-" .repeat(85));

        for (Map.Entry<Integer, Article> entry : products.entrySet()) {
            System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
        }

        System.out.println("-" .repeat(85));
        System.out.printf("Загальна кількість: %d товарів\n", products.size());
        System.out.printf("Унікальних назв: %d\n", productNames.size());
    }

    // Детальний перегляд товару
    public void displayProductDetails(Integer articleId) {
        Article article = products.get(articleId);
        if (article != null) {
            System.out.println("\n📦 ДЕТАЛЬНА ІНФОРМАЦІЯ ПРО ТОВАР");
            System.out.println("=" .repeat(40));
            System.out.printf("Артикул: #%d\n", articleId);
            System.out.println(article.getDetailedInfo());
        } else {
            System.out.println("❌ Товар з артикулом #" + articleId + " не знайдено");
        }
    }

    // Статистика товарів
    public void displayProductStatistics() {
        System.out.println("\n📊 СТАТИСТИКА ТОВАРІВ");
        System.out.println("=" .repeat(40));

        if (products.isEmpty()) {
            System.out.println("Немає даних для статистики");
            return;
        }

        // Загальна статистика
        int totalProducts = products.size();
        long availableCount = products.values().stream().filter(Article::getInStock).count();
        long unavailableCount = totalProducts - availableCount;

        OptionalDouble avgPrice = products.values().stream()
                .mapToDouble(Article::getPrice).average();
        Optional<Article> mostExpensive = products.values().stream()
                .max(Comparator.comparing(Article::getPrice));
        Optional<Article> cheapest = products.values().stream()
                .min(Comparator.comparing(Article::getPrice));

        System.out.printf("Загальна кількість товарів: %d\n", totalProducts);
        System.out.printf("В наявності: %d (%.1f%%)\n", availableCount,
            (availableCount * 100.0) / totalProducts);
        System.out.printf("Відсутні: %d (%.1f%%)\n", unavailableCount,
            (unavailableCount * 100.0) / totalProducts);

        if (avgPrice.isPresent()) {
            System.out.printf("Середня ціна: %.2f грн\n", avgPrice.getAsDouble());
        }

        if (mostExpensive.isPresent()) {
            System.out.printf("Найдорожчий товар: %s (%.2f грн)\n",
                mostExpensive.get().getName(), mostExpensive.get().getPrice());
        }

        if (cheapest.isPresent()) {
            System.out.printf("Найдешевший товар: %s (%.2f грн)\n",
                cheapest.get().getName(), cheapest.get().getPrice());
        }

        // Статистика по категоріях
        System.out.println("\nСтатистика по категоріях:");
        Map<String, List<Article>> byCategory = new TreeMap<>();
        for (Article article : products.values()) {
            byCategory.computeIfAbsent(article.getCategory(), k -> new ArrayList<>()).add(article);
        }

        for (Map.Entry<String, List<Article>> entry : byCategory.entrySet()) {
            String category = entry.getKey();
            List<Article> categoryProducts = entry.getValue();
            double avgCategoryPrice = categoryProducts.stream()
                    .mapToDouble(Article::getPrice).average().orElse(0);
            long availableInCategory = categoryProducts.stream()
                    .filter(Article::getInStock).count();

            System.out.printf("  %s: %d товарів, середня ціна: %.2f грн, в наявності: %d\n",
                category, categoryProducts.size(), avgCategoryPrice, availableInCategory);
        }

        // Цінові діапазони
        System.out.println("\nРозподіл по ціновим діапазонам:");
        long cheap = products.values().stream().filter(p -> p.getPrice() < 100).count();
        long moderate = products.values().stream().filter(p -> p.getPrice() >= 100 && p.getPrice() < 1000).count();
        long expensive = products.values().stream().filter(p -> p.getPrice() >= 1000).count();

        System.out.printf("  Дешеві (< 100 грн): %d товарів\n", cheap);
        System.out.printf("  Помірні (100-1000 грн): %d товарів\n", moderate);
        System.out.printf("  Дорогі (> 1000 грн): %d товарів\n", expensive);
    }

    // Сортування товарів
    public void sortProductsBy(String criterion) {
        List<Map.Entry<Integer, Article>> sortedProducts = new ArrayList<>(products.entrySet());

        switch (criterion.toLowerCase()) {
            case "name":
            case "назва":
                sortedProducts.sort(Map.Entry.comparingByValue(
                    Comparator.comparing(Article::getName)));
                break;
            case "price":
            case "ціна":
                sortedProducts.sort(Map.Entry.comparingByValue(
                    Comparator.comparing(Article::getPrice)));
                break;
            case "price_desc":
            case "ціна_спад":
                sortedProducts.sort(Map.Entry.comparingByValue(
                    Comparator.comparing(Article::getPrice).reversed()));
                break;
            case "category":
            case "категорія":
                sortedProducts.sort(Map.Entry.comparingByValue(
                    Comparator.comparing(Article::getCategory)
                        .thenComparing(Article::getName)));
                break;
            case "date":
            case "дата":
                sortedProducts.sort(Map.Entry.comparingByValue(
                    Comparator.comparing(Article::getAddedDate).reversed()));
                break;
            default:
                System.out.println("❌ Невірний критерій сортування");
                return;
        }

        System.out.println("\n📋 Товари відсортовані за: " + criterion);
        System.out.println("=" .repeat(85));
        System.out.println("Артикул | Назва                     | Ціна      | Категорія       | Статус     | Дата");
        System.out.println("-" .repeat(85));

        for (Map.Entry<Integer, Article> entry : sortedProducts) {
            System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
        }
    }

    // Перевірка унікальності назви
    public boolean isNameUnique(String name) {
        return !productNames.contains(name.toLowerCase());
    }

    // Отримання всіх унікальних категорій
    public Set<String> getAllCategories() {
        Set<String> categories = new TreeSet<>();
        for (Article article : products.values()) {
            categories.add(article.getCategory());
        }
        return categories;
    }

    // Збереження у файл
    public void saveToFile(String filename) {
        try (PrintWriter writer = new PrintWriter(new FileWriter(filename))) {
            writer.println("=== КАТАЛОГ ТОВАРІВ МАГАЗИНУ ===");
            writer.println("Дата створення: " + new SimpleDateFormat("dd.MM.yyyy HH:mm:ss").format(new Date()));
            writer.println("Студент: групи ТВ-43, прізвище Step, варіант 16");
            writer.println();

            writer.println("ПОЧАТКОВІ ДАНІ:");
            writer.printf("Загальна кількість товарів: %d\n", products.size());
            writer.printf("Унікальних назв: %d\n", productNames.size());
            writer.println();

            for (Map.Entry<Integer, Article> entry : products.entrySet()) {
                writer.printf("Артикул: #%d\n", entry.getKey());
                writer.println(entry.getValue().getDetailedInfo());
                writer.println("-" .repeat(40));
            }

            writer.println("РЕЗУЛЬТАТИ РОБОТИ ПРОГРАМИ:");
            if (!products.isEmpty()) {
                OptionalDouble avgPrice = products.values().stream()
                        .mapToDouble(Article::getPrice).average();
                if (avgPrice.isPresent()) {
                    writer.printf("Середня ціна товарів: %.2f грн\n", avgPrice.getAsDouble());
                }
            }

            System.out.println("✅ Дані збережено у файл: " + filename);
        } catch (IOException e) {
            System.out.println("❌ Помилка збереження файлу: " + e.getMessage());
        }
    }

    // Генерація випадкових товарів
    public void generateRandomProducts(int count) {
        String[] productNames = {
            "Телефон", "Планшет", "Ноутбук", "Миша", "Клавіатура", "Монітор",
            "Книга", "Олівець", "Папір", "Калькулятор", "Рюкзак", "Сумка",
            "Сир", "Ковбаса", "Цукерки", "Печиво", "Сік", "Вода",
            "Сорочка", "Светр", "Штани", "Шкарпетки", "Капелюх", "Рукавички"
        };

        String[] categories = {
            "Електроніка", "Канцелярія", "Продукти", "Одяг", "Побутова хімія", "Іграшки"
        };

        System.out.printf("Генерація %d випадкових товарів...\n", count);

        for (int i = 0; i < count; i++) {
            String name;
            do {
                name = productNames[random.nextInt(productNames.length)] + " " +
                       (random.nextInt(999) + 1);
            } while (!isNameUnique(name));

            float price = 10 + random.nextFloat() * 4990; // 10-5000 грн
            String category = categories[random.nextInt(categories.length)];
            String description = "Автоматично згенерований товар";

            Article article = new Article(name, price, category, description);
            article.setInStock(random.nextBoolean());

            addProduct(article);
        }
    }

    // Інтерактивне меню
    public void showInteractiveMenu() {
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n🎮 МЕНЮ УПРАВЛІННЯ ТОВАРАМИ");
            System.out.println("=" .repeat(40));
            System.out.println("1. Переглянути всі товари");
            System.out.println("2. Додати товар");
            System.out.println("3. Змінити ціну товару");
            System.out.println("4. Видалити товар");
            System.out.println("5. Детальний перегляд товару");
            System.out.println("6. Пошук товарів");
            System.out.println("7. Сортування товарів");
            System.out.println("8. Статистика");
            System.out.println("9. Перевірка унікальності назви");
            System.out.println("10. Генерувати випадкові товари");
            System.out.println("11. Зберегти у файл");
            System.out.println("0. Вийти");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();
                scanner.nextLine();

                switch (choice) {
                    case 1:
                        displayAllProducts();
                        break;

                    case 2:
                        System.out.print("Назва товару: ");
                        String name = scanner.nextLine();

                        if (!isNameUnique(name)) {
                            System.out.println("❌ Товар з такою назвою вже існує!");
                            break;
                        }

                        System.out.print("Ціна: ");
                        float price = scanner.nextFloat();
                        scanner.nextLine();
                        System.out.print("Категорія: ");
                        String category = scanner.nextLine();
                        System.out.print("Опис: ");
                        String description = scanner.nextLine();

                        Article newArticle = new Article(name, price, category, description);
                        addProduct(newArticle);
                        break;

                    case 3:
                        System.out.print("Артикул товару: ");
                        Integer articleId = scanner.nextInt();
                        System.out.print("Нова ціна: ");
                        float newPrice = scanner.nextFloat();
                        changePrice(articleId, newPrice);
                        break;

                    case 4:
                        System.out.print("Артикул товару для видалення: ");
                        Integer removeId = scanner.nextInt();
                        removeProduct(removeId);
                        break;

                    case 5:
                        System.out.print("Артикул товару: ");
                        Integer detailId = scanner.nextInt();
                        displayProductDetails(detailId);
                        break;

                    case 6:
                        System.out.println("Пошук за:");
                        System.out.println("1 - Назвою, 2 - Категорією, 3 - Ціною, 4 - Наявністю");
                        int searchType = scanner.nextInt();
                        scanner.nextLine();

                        if (searchType == 1) {
                            System.out.print("Введіть частину назви: ");
                            String searchName = scanner.nextLine();
                            List<Map.Entry<Integer, Article>> found = findProductsByName(searchName);
                            if (!found.isEmpty()) {
                                System.out.println("✅ Знайдено " + found.size() + " товарів:");
                                for (Map.Entry<Integer, Article> entry : found) {
                                    System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
                                }
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        } else if (searchType == 2) {
                            System.out.println("Доступні категорії: " + getAllCategories());
                            System.out.print("Введіть категорію: ");
                            String searchCategory = scanner.nextLine();
                            List<Map.Entry<Integer, Article>> found = findProductsByCategory(searchCategory);
                            if (!found.isEmpty()) {
                                System.out.println("✅ Знайдено " + found.size() + " товарів:");
                                for (Map.Entry<Integer, Article> entry : found) {
                                    System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
                                }
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        } else if (searchType == 3) {
                            System.out.print("Мінімальна ціна: ");
                            float minPrice = scanner.nextFloat();
                            System.out.print("Максимальна ціна: ");
                            float maxPrice = scanner.nextFloat();
                            List<Map.Entry<Integer, Article>> found = findProductsByPriceRange(minPrice, maxPrice);
                            if (!found.isEmpty()) {
                                System.out.println("✅ Знайдено " + found.size() + " товарів:");
                                for (Map.Entry<Integer, Article> entry : found) {
                                    System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
                                }
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        } else if (searchType == 4) {
                            List<Map.Entry<Integer, Article>> available = getAvailableProducts();
                            System.out.println("✅ Товари в наявності (" + available.size() + "):");
                            for (Map.Entry<Integer, Article> entry : available) {
                                System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
                            }
                        }
                        break;

                    case 7:
                        System.out.println("Сортувати за: name/price/price_desc/category/date");
                        String sortBy = scanner.nextLine();
                        sortProductsBy(sortBy);
                        break;

                    case 8:
                        displayProductStatistics();
                        break;

                    case 9:
                        System.out.print("Введіть назву для перевірки: ");
                        String checkName = scanner.nextLine();
                        boolean unique = isNameUnique(checkName);
                        System.out.printf("Назва \"%s\": %s\n", checkName,
                            unique ? "✅ Унікальна" : "❌ Вже існує");
                        break;

                    case 10:
                        System.out.print("Кількість товарів для генерації: ");
                        int count = scanner.nextInt();
                        generateRandomProducts(count);
                        break;

                    case 11:
                        System.out.print("Ім'я файлу (наприклад, products.txt): ");
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
        System.out.println("=== Лабораторна робота 4 - Завдання 3 ===");
        System.out.println("TreeMap: система управління товарами в магазині");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        Lr4_3 program = new Lr4_3();

        // Демонстрація основного функціоналу
        program.displayAllProducts();
        program.displayProductStatistics();

        // Демонстрація пошуку
        System.out.println("\n🔍 ДЕМОНСТРАЦІЯ: Пошук товарів електроніки");
        List<Map.Entry<Integer, Article>> electronics = program.findProductsByCategory("Електроніка");
        for (Map.Entry<Integer, Article> entry : electronics) {
            System.out.printf("#%-6d | %s\n", entry.getKey(), entry.getValue());
        }

        // Демонстрація зміни ціни
        System.out.println("\n💰 ДЕМОНСТРАЦІЯ: Зміна ціни товару");
        Integer firstProductId = program.products.firstKey();
        if (firstProductId != null) {
            Article firstProduct = program.products.get(firstProductId);
            float oldPrice = firstProduct.getPrice();
            program.changePrice(firstProductId, oldPrice * 1.1f); // підвищуємо на 10%
        }

        // Демонстрація перевірки унікальності
        System.out.println("\n🔍 ДЕМОНСТРАЦІЯ: Перевірка унікальності назв");
        System.out.println("Назва 'Хліб білий': " +
            (program.isNameUnique("Хліб білий") ? "Унікальна" : "Вже існує"));
        System.out.println("Назва 'Новий товар': " +
            (program.isNameUnique("Новий товар") ? "Унікальна" : "Вже існує"));

        // Демонстрація сортування
        program.sortProductsBy("price_desc");

        // Збереження у файл
        program.saveToFile("products_output.txt");

        // Інтерактивне меню
        program.showInteractiveMenu();

        System.out.println("\n✅ Програма завершена успішно!");
    }
}