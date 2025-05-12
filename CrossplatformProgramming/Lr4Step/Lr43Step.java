// Main class Lr43Petr
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;

public class Lr43Step {
    public static void main(String[] args) {
        StoreItemManager manager = new StoreItemManager();
        
        // Method 1: Adding items programmatically (3 data sets)
        // Data set 1
        manager.addItem(new StoreItem(1001, "Ноутбук HP", 18599.99f));
        manager.addItem(new StoreItem(1002, "Смартфон Samsung", 12499.99f));
        manager.addItem(new StoreItem(1003, "Планшет Apple", 16899.99f));
        manager.addItem(new StoreItem(1004, "Монітор LG", 5299.99f));
        manager.addItem(new StoreItem(1005, "Клавіатура Logitech", 1299.99f));
        
        // Data set 2
        manager.addItem(new StoreItem(2001, "Навушники Sony", 3499.99f));
        manager.addItem(new StoreItem(2002, "Миша бездротова", 699.99f));
        manager.addItem(new StoreItem(2003, "Принтер Canon", 4599.99f));
        manager.addItem(new StoreItem(2004, "Веб-камера", 899.99f));
        manager.addItem(new StoreItem(2005, "Зовнішній жорсткий диск", 2799.99f));
        
        // Data set 3
        manager.addItem(new StoreItem(3001, "USB флеш-накопичувач", 399.99f));
        manager.addItem(new StoreItem(3002, "Акустична система", 1899.99f));
        manager.addItem(new StoreItem(3003, "Графічний планшет", 5999.99f));
        manager.addItem(new StoreItem(3004, "Роутер Wi-Fi", 1299.99f));
        manager.addItem(new StoreItem(3005, "Гарнітура ігрова", 2499.99f));
        
        // Output to console and file
        System.out.println("=== Товари, додані програмно ===");
        String initialData = manager.displayItems();
        System.out.println(initialData);
        
        // Save to file
        try {
            saveToFile("store_items_output.txt", "=== Початковий список товарів ===\n" + initialData);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }
        
        // Method 2: Interactive user mode
        Scanner scanner = new Scanner(System.in);
        boolean exit = false;
        
        while (!exit) {
            System.out.println("\nМеню управління товарами:");
            System.out.println("1. Додати новий товар");
            System.out.println("2. Переглянути всі товари");
            System.out.println("3. Змінити ціну товару");
            System.out.println("4. Видалити товар");
            System.out.println("5. Пошук товару за артикулом");
            System.out.println("6. Вихід");
            System.out.print("Оберіть опцію (1-6): ");
            
            int choice = 0;
            try {
                choice = Integer.parseInt(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть число від 1 до 6.");
                continue;
            }
            
            switch (choice) {
                case 1:
                    addItemInteractive(scanner, manager);
                    break;
                case 2:
                    String items = manager.displayItems();
                    System.out.println("=== Список товарів ===");
                    System.out.println(items);
                    
                    // Save to file
                    try {
                        appendToFile("store_items_output.txt", "\n\n=== Оновлений список товарів ===\n" + items);
                    } catch (IOException e) {
                        System.out.println("Помилка при збереженні у файл: " + e.getMessage());
                    }
                    break;
                case 3:
                    changeItemPriceInteractive(scanner, manager);
                    break;
                case 4:
                    removeItemInteractive(scanner, manager);
                    break;
                case 5:
                    findItemInteractive(scanner, manager);
                    break;
                case 6:
                    exit = true;
                    System.out.println("Дякуємо за використання програми!");
                    break;
                default:
                    System.out.println("Невірний вибір. Будь ласка, введіть число від 1 до 6.");
            }
        }
        
        scanner.close();
    }
    
    private static void addItemInteractive(Scanner scanner, StoreItemManager manager) {
        System.out.println("=== Додавання нового товару ===");
        
        int article = 0;
        boolean validArticle = false;
        
        while (!validArticle) {
            System.out.print("Введіть артикул товару: ");
            try {
                article = Integer.parseInt(scanner.nextLine());
                if (manager.itemExists(article)) {
                    System.out.println("Товар з таким артикулом вже існує!");
                } else {
                    validArticle = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        String name = "";
        boolean validName = false;
        
        while (!validName) {
            System.out.print("Введіть назву товару: ");
            name = scanner.nextLine().trim();
            
            if (name.isEmpty()) {
                System.out.println("Назва товару не може бути порожньою!");
            } else if (manager.itemNameExists(name)) {
                System.out.println("Товар з такою назвою вже існує!");
            } else {
                validName = true;
            }
        }
        
        float price = 0;
        boolean validPrice = false;
        
        while (!validPrice) {
            System.out.print("Введіть ціну товару: ");
            try {
                price = Float.parseFloat(scanner.nextLine());
                if (price <= 0) {
                    System.out.println("Ціна має бути більше 0!");
                } else {
                    validPrice = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        StoreItem item = new StoreItem(article, name, price);
        manager.addItem(item);
        
        System.out.println("Товар успішно додано!");
        
        // Save to file
        try {
            appendToFile("store_items_output.txt", "\n\n=== Додано новий товар ===\n" + item);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }
    }
    
    private static void changeItemPriceInteractive(Scanner scanner, StoreItemManager manager) {
        System.out.println("=== Зміна ціни товару ===");
        
        int article = 0;
        boolean validArticle = false;
        
        while (!validArticle) {
            System.out.print("Введіть артикул товару: ");
            try {
                article = Integer.parseInt(scanner.nextLine());
                if (!manager.itemExists(article)) {
                    System.out.println("Товар з таким артикулом не існує!");
                } else {
                    validArticle = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        StoreItem item = manager.getItem(article);
        System.out.println("Поточна інформація про товар:");
        System.out.println(item);
        
        float newPrice = 0;
        boolean validPrice = false;
        
        while (!validPrice) {
            System.out.print("Введіть нову ціну товару: ");
            try {
                newPrice = Float.parseFloat(scanner.nextLine());
                if (newPrice <= 0) {
                    System.out.println("Ціна має бути більше 0!");
                } else {
                    validPrice = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        float oldPrice = item.getPrice();
        manager.changeItemPrice(article, newPrice);
        
        System.out.println("Ціна товару успішно змінена!");
        
        // Save to file
        try {
            appendToFile("store_items_output.txt", "\n\n=== Змінено ціну товару ===\n" + 
                        "Артикул: " + article + " | Назва: " + item.getName() + 
                        "\nСтара ціна: " + oldPrice + " грн | Нова ціна: " + newPrice + " грн");
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }
    }
    
    private static void removeItemInteractive(Scanner scanner, StoreItemManager manager) {
        System.out.println("=== Видалення товару ===");
        
        int article = 0;
        boolean validArticle = false;
        
        while (!validArticle) {
            System.out.print("Введіть артикул товару для видалення: ");
            try {
                article = Integer.parseInt(scanner.nextLine());
                if (!manager.itemExists(article)) {
                    System.out.println("Товар з таким артикулом не існує!");
                } else {
                    validArticle = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        StoreItem item = manager.getItem(article);
        System.out.println("Товар для видалення:");
        System.out.println(item);
        
        System.out.print("Ви впевнені, що хочете видалити цей товар? (так/ні): ");
        String confirm = scanner.nextLine().toLowerCase();
        
        if (confirm.equals("так")) {
            manager.removeItem(article);
            System.out.println("Товар успішно видалено!");
            
            // Save to file
            try {
                appendToFile("store_items_output.txt", "\n\n=== Видалено товар ===\n" + item);
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
        } else {
            System.out.println("Видалення скасовано.");
        }
    }
    
    private static void findItemInteractive(Scanner scanner, StoreItemManager manager) {
        System.out.println("=== Пошук товару за артикулом ===");
        
        int article = 0;
        boolean validArticle = false;
        
        while (!validArticle) {
            System.out.print("Введіть артикул товару: ");
            try {
                article = Integer.parseInt(scanner.nextLine());
                validArticle = true;
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        if (manager.itemExists(article)) {
            StoreItem item = manager.getItem(article);
            System.out.println("Знайдений товар:");
            System.out.println(item);
            
            // Save to file
            try {
                appendToFile("store_items_output.txt", "\n\n=== Пошук товару за артикулом ===\n" + 
                            "Артикул: " + article + "\nЗнайдений товар:\n" + item);
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
        } else {
            System.out.println("Товар з артикулом " + article + " не знайдено.");
            
            // Save to file
            try {
                appendToFile("store_items_output.txt", "\n\n=== Пошук товару за артикулом ===\n" + 
                            "Артикул: " + article + "\nТовар не знайдено.");
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
        }
    }
    
    private static void saveToFile(String fileName, String content) throws IOException {
        try (PrintWriter writer = new PrintWriter(new FileWriter(fileName))) {
            writer.println(content);
        }
    }
    
    private static void appendToFile(String fileName, String content) throws IOException {
        try (PrintWriter writer = new PrintWriter(new FileWriter(fileName, true))) {
            writer.println(content);
        }
    }
}

// StoreItem class
class StoreItem {
    private int article;
    private String name;
    private float price;
    
    public StoreItem(int article, String name, float price) {
        this.article = article;
        this.name = name;
        this.price = price;
    }
    
    public int getArticle() {
        return article;
    }
    
    public String getName() {
        return name;
    }
    
    public float getPrice() {
        return price;
    }
    
    public void setPrice(float price) {
        this.price = price;
    }
    
    @Override
    public String toString() {
        return String.format("Артикул: %-5d | Назва: %-30s | Ціна: %.2f грн", article, name, price);
    }
}

// StoreItemManager class
class StoreItemManager {
    private TreeMap<Integer, StoreItem> items;
    private HashSet<String> itemNames;
    
    public StoreItemManager() {
        this.items = new TreeMap<>();
        this.itemNames = new HashSet<>();
    }
    
    public boolean itemExists(int article) {
        return items.containsKey(article);
    }
    
    public boolean itemNameExists(String name) {
        return itemNames.contains(name.toLowerCase());
    }
    
    public void addItem(StoreItem item) {
        items.put(item.getArticle(), item);
        itemNames.add(item.getName().toLowerCase());
    }
    
    public void removeItem(int article) {
        if (items.containsKey(article)) {
            itemNames.remove(items.get(article).getName().toLowerCase());
            items.remove(article);
        }
    }
    
    public StoreItem getItem(int article) {
        return items.get(article);
    }
    
    public void changeItemPrice(int article, float newPrice) {
        if (items.containsKey(article)) {
            items.get(article).setPrice(newPrice);
        }
    }
    
    public String displayItems() {
        if (items.isEmpty()) {
            return "Немає товарів.";
        }
        
        StringBuilder sb = new StringBuilder();
        
        for (StoreItem item : items.values()) {
            sb.append(item).append("\n");
        }
        
        return sb.toString();
    }
}