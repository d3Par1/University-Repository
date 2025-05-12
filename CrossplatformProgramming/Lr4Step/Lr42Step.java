import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;

public class Lr42Step{
    public static void main(String[] args) {
        OrderManager manager = new OrderManager();
        
        Order order1 = new Order(1001);
        order1.addProduct(new Product("Хліб", 2, ProductUnit.PIECES, 22.50));
        order1.addProduct(new Product("Молоко", 3, ProductUnit.PIECES, 36.75));
        order1.addProduct(new Product("Яблука", 1.5, ProductUnit.KG, 27.80));
        order1.addProduct(new Product("Сир", 0.5, ProductUnit.KG, 172.50));
        manager.addOrder(order1);
        
        Order order2 = new Order(1002);
        order2.addProduct(new Product("Макарони", 2, ProductUnit.PIECES, 42.60));
        order2.addProduct(new Product("Помідори", 1.2, ProductUnit.KG, 65.90));
        order2.addProduct(new Product("Йогурт", 4, ProductUnit.PIECES, 22.75));
        order2.addProduct(new Product("Кава", 1, ProductUnit.PIECES, 189.99));
        order2.addProduct(new Product("Цукор", 1, ProductUnit.KG, 29.50));
        manager.addOrder(order2);
        
        // Data set 2
        Order order3 = new Order(1003);
        order3.addProduct(new Product("Олія", 1, ProductUnit.PIECES, 87.50));
        order3.addProduct(new Product("Картопля", 3, ProductUnit.KG, 18.90));
        order3.addProduct(new Product("Морква", 1, ProductUnit.KG, 22.75));
        order3.addProduct(new Product("Курятина", 1.5, ProductUnit.KG, 129.80));
        manager.addOrder(order3);
        
        Order order4 = new Order(1004);
        order4.addProduct(new Product("Чай", 2, ProductUnit.PIECES, 56.75));
        order4.addProduct(new Product("Печиво", 1, ProductUnit.PIECES, 42.99));
        order4.addProduct(new Product("Цибуля", 0.5, ProductUnit.KG, 19.50));
        order4.addProduct(new Product("Огірки", 1, ProductUnit.KG, 55.90));
        manager.addOrder(order4);
        
        // Data set 3
        Order order5 = new Order(1005);
        order5.addProduct(new Product("Риба", 1.2, ProductUnit.KG, 189.90));
        order5.addProduct(new Product("Яйця", 10, ProductUnit.PIECES, 39.50));
        order5.addProduct(new Product("Сіль", 1, ProductUnit.PIECES, 18.75));
        order5.addProduct(new Product("Перець", 1, ProductUnit.PIECES, 22.50));
        manager.addOrder(order5);
        
        // Output to console and file
        System.out.println("=== Замовлення, додані програмно ===");
        String initialData = manager.displayAllOrders();
        System.out.println(initialData);
        
        // Save to file
        try {
            saveToFile("orders_output.txt", "=== Початковий список замовлень ===\n" + initialData);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }
        
        // Method 2: Interactive user mode
        Scanner scanner = new Scanner(System.in);
        boolean exit = false;
        
        while (!exit) {
            System.out.println("\nМеню управління замовленнями:");
            System.out.println("1. Створити нове замовлення");
            System.out.println("2. Додати продукт до замовлення");
            System.out.println("3. Показати всі замовлення");
            System.out.println("4. Показати деталі замовлення");
            System.out.println("5. Видалити замовлення");
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
                    createOrderInteractive(scanner, manager);
                    break;
                case 2:
                    addProductToOrderInteractive(scanner, manager);
                    break;
                case 3:
                    String orders = manager.displayAllOrders();
                    System.out.println("=== Список замовлень ===");
                    System.out.println(orders);
                    
                    // Save to file
                    try {
                        appendToFile("orders_output.txt", "\n\n=== Оновлений список замовлень ===\n" + orders);
                    } catch (IOException e) {
                        System.out.println("Помилка при збереженні у файл: " + e.getMessage());
                    }
                    break;
                case 4:
                    showOrderDetailsInteractive(scanner, manager);
                    break;
                case 5:
                    removeOrderInteractive(scanner, manager);
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
    
    private static void createOrderInteractive(Scanner scanner, OrderManager manager) {
        System.out.println("=== Створення нового замовлення ===");
        
        int orderNumber = 0;
        boolean validOrderNumber = false;
        
        while (!validOrderNumber) {
            System.out.print("Введіть номер замовлення: ");
            try {
                orderNumber = Integer.parseInt(scanner.nextLine());
                if (manager.orderExists(orderNumber)) {
                    System.out.println("Замовлення з таким номером вже існує!");
                } else {
                    validOrderNumber = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        Order order = new Order(orderNumber);
        manager.addOrder(order);
        
        System.out.println("Замовлення #" + orderNumber + " створено успішно!");
        
        // Save to file
        try {
            appendToFile("orders_output.txt", "\n\n=== Створено нове замовлення ===\nЗамовлення #" + orderNumber);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }
        
        // Ask if user wants to add products right away
        System.out.print("Бажаєте додати продукти до замовлення? (так/ні): ");
        String answer = scanner.nextLine().toLowerCase();
        
        if (answer.equals("так")) {
            addProductsToOrder(scanner, manager, orderNumber);
        }
    }
    
    private static void addProductToOrderInteractive(Scanner scanner, OrderManager manager) {
        System.out.println("=== Додавання продукту до замовлення ===");
        
        int orderNumber = 0;
        boolean validOrderNumber = false;
        
        while (!validOrderNumber) {
            System.out.print("Введіть номер замовлення: ");
            try {
                orderNumber = Integer.parseInt(scanner.nextLine());
                if (!manager.orderExists(orderNumber)) {
                    System.out.println("Замовлення з таким номером не існує!");
                } else {
                    validOrderNumber = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        addProductsToOrder(scanner, manager, orderNumber);
    }
    
    private static void addProductsToOrder(Scanner scanner, OrderManager manager, int orderNumber) {
        boolean addMoreProducts = true;
        
        while (addMoreProducts) {
            System.out.println("=== Додавання продукту до замовлення #" + orderNumber + " ===");
            
            System.out.print("Введіть назву продукту: ");
            String name = scanner.nextLine();
            
            double quantity = 0;
            while (quantity <= 0) {
                System.out.print("Введіть кількість: ");
                try {
                    quantity = Double.parseDouble(scanner.nextLine());
                    if (quantity <= 0) {
                        System.out.println("Кількість має бути більше 0.");
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Будь ласка, введіть правильне число.");
                }
            }
            
            System.out.print("Одиниця виміру (1 - штуки, 2 - кг): ");
            ProductUnit unit = ProductUnit.PIECES;
            boolean validUnit = false;
            
            while (!validUnit) {
                try {
                    int unitChoice = Integer.parseInt(scanner.nextLine());
                    if (unitChoice == 1) {
                        unit = ProductUnit.PIECES;
                        validUnit = true;
                    } else if (unitChoice == 2) {
                        unit = ProductUnit.KG;
                        validUnit = true;
                    } else {
                        System.out.print("Невірний вибір. Введіть 1 для штук або 2 для кг: ");
                    }
                } catch (NumberFormatException e) {
                    System.out.print("Введіть 1 для штук або 2 для кг: ");
                }
            }
            
            double price = 0;
            while (price <= 0) {
                System.out.print("Введіть ціну за одиницю: ");
                try {
                    price = Double.parseDouble(scanner.nextLine());
                    if (price <= 0) {
                        System.out.println("Ціна має бути більше 0.");
                    }
                } catch (NumberFormatException e) {
                    System.out.println("Будь ласка, введіть правильне число.");
                }
            }
            
            Product product = new Product(name, quantity, unit, price);
            manager.addProductToOrder(orderNumber, product);
            
            System.out.println("Продукт успішно додано до замовлення!");
            
            // Save to file
            try {
                appendToFile("orders_output.txt", "\n=== Додано продукт до замовлення #" + orderNumber + " ===\n" + product);
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
            
            System.out.print("Додати ще один продукт? (так/ні): ");
            String answer = scanner.nextLine().toLowerCase();
            addMoreProducts = answer.equals("так");
        }
    }
    
    private static void showOrderDetailsInteractive(Scanner scanner, OrderManager manager) {
        System.out.println("=== Деталі замовлення ===");
        
        int orderNumber = 0;
        boolean validOrderNumber = false;
        
        while (!validOrderNumber) {
            System.out.print("Введіть номер замовлення: ");
            try {
                orderNumber = Integer.parseInt(scanner.nextLine());
                if (!manager.orderExists(orderNumber)) {
                    System.out.println("Замовлення з таким номером не існує!");
                } else {
                    validOrderNumber = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        String orderDetails = manager.displayOrder(orderNumber);
        System.out.println(orderDetails);
        
        // Save to file
        try {
            appendToFile("orders_output.txt", "\n\n=== Деталі замовлення ===\n" + orderDetails);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }
    }
    
    private static void removeOrderInteractive(Scanner scanner, OrderManager manager) {
        System.out.println("=== Видалення замовлення ===");
        
        int orderNumber = 0;
        boolean validOrderNumber = false;
        
        while (!validOrderNumber) {
            System.out.print("Введіть номер замовлення для видалення: ");
            try {
                orderNumber = Integer.parseInt(scanner.nextLine());
                if (!manager.orderExists(orderNumber)) {
                    System.out.println("Замовлення з таким номером не існує!");
                } else {
                    validOrderNumber = true;
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }
        
        System.out.print("Ви впевнені, що хочете видалити замовлення #" + orderNumber + "? (так/ні): ");
        String answer = scanner.nextLine().toLowerCase();
        
        if (answer.equals("так")) {
            manager.removeOrder(orderNumber);
            System.out.println("Замовлення #" + orderNumber + " видалено успішно!");
            
            // Save to file
            try {
                appendToFile("orders_output.txt", "\n\n=== Видалено замовлення ===\nЗамовлення #" + orderNumber);
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
        } else {
            System.out.println("Видалення скасовано.");
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

// ProductUnit enum
enum ProductUnit {
    PIECES("шт."),
    KG("кг");
    
    private final String label;
    
    ProductUnit(String label) {
        this.label = label;
    }
    
    @Override
    public String toString() {
        return label;
    }
}

// Product class
class Product {
    private String name;
    private double quantity;
    private ProductUnit unit;
    private double price;
    
    public Product(String name, double quantity, ProductUnit unit, double price) {
        this.name = name;
        this.quantity = quantity;
        this.unit = unit;
        this.price = price;
    }
    
    public String getName() {
        return name;
    }
    
    public double getQuantity() {
        return quantity;
    }
    
    public ProductUnit getUnit() {
        return unit;
    }
    
    public double getPrice() {
        return price;
    }
    
    public double getTotalPrice() {
        return quantity * price;
    }
    
    @Override
    public String toString() {
        return String.format("%-20s | %6.2f %-3s | %8.2f грн/%-3s | %9.2f грн", 
                            name, quantity, unit, price, unit, getTotalPrice());
    }
}

// Order class
class Order {
    private int orderNumber;
    private List<Product> products;
    
    public Order(int orderNumber) {
        this.orderNumber = orderNumber;
        this.products = new ArrayList<>();
    }
    
    public int getOrderNumber() {
        return orderNumber;
    }
    
    public void addProduct(Product product) {
        products.add(product);
    }
    
    public List<Product> getProducts() {
        return products;
    }
    
    public double getTotalOrderPrice() {
        double total = 0;
        for (Product product : products) {
            total += product.getTotalPrice();
        }
        return total;
    }
    
    @Override
    public String toString() {
        return "Замовлення #" + orderNumber + " | Кількість товарів: " + products.size() + 
               " | Загальна вартість: " + String.format("%.2f грн", getTotalOrderPrice());
    }
    
    public String getDetailedInfo() {
        StringBuilder sb = new StringBuilder();
        sb.append("=== Замовлення #").append(orderNumber).append(" ===\n");
        
        if (products.isEmpty()) {
            sb.append("Замовлення порожнє.\n");
        } else {
            sb.append(String.format("%-4s | %-20s | %-10s | %-12s | %-10s\n", 
                                   "№", "Назва", "Кількість", "Ціна", "Вартість"));
            sb.append("--------------------------------------------------------------------\n");
            
            int index = 1;
            for (Product product : products) {
                sb.append(String.format("%-4d | %s\n", index++, product));
            }
            
            sb.append("--------------------------------------------------------------------\n");
            sb.append(String.format("Загальна вартість замовлення: %.2f грн\n", getTotalOrderPrice()));
        }
        
        return sb.toString();
    }
}

// OrderManager class
class OrderManager {
    private Map<Integer, Order> orders;
    
    public OrderManager() {
        this.orders = new HashMap<>();
    }
    
    public void addOrder(Order order) {
        orders.put(order.getOrderNumber(), order);
    }
    
    public boolean orderExists(int orderNumber) {
        return orders.containsKey(orderNumber);
    }
    
    public void removeOrder(int orderNumber) {
        orders.remove(orderNumber);
    }
    
    public void addProductToOrder(int orderNumber, Product product) {
        if (orders.containsKey(orderNumber)) {
            orders.get(orderNumber).addProduct(product);
        }
    }
    
    public String displayAllOrders() {
        if (orders.isEmpty()) {
            return "Немає замовлень.";
        }
        
        StringBuilder sb = new StringBuilder();
        
        List<Order> sortedOrders = new ArrayList<>(orders.values());
        sortedOrders.sort(Comparator.comparingInt(Order::getOrderNumber));
        
        for (Order order : sortedOrders) {
            sb.append(order).append("\n");
        }
        
        return sb.toString();
    }
    
    public String displayOrder(int orderNumber) {
        if (orders.containsKey(orderNumber)) {
            return orders.get(orderNumber).getDetailedInfo();
        } else {
            return "Замовлення #" + orderNumber + " не знайдено.";
        }
    }
}