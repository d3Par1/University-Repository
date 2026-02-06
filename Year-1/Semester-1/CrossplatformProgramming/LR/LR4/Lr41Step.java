
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.*;

public class Lr41Step {
    public static void main(String[] args) {
        EmployeeManager manager = new EmployeeManager();

        // Method 1: Adding employees programmatically (3 data sets)
        // Data set 1
        manager.addEmployee(new Employee("Іванов Іван", 25, 28000.0));
        manager.addEmployee(new Employee("Петров Петро", 30, 32000.0));
        manager.addEmployee(new Employee("Сидоренко Сидір", 35, 35000.0));
        manager.addEmployee(new Employee("Ковальчук Марія", 28, 30000.0));
        manager.addEmployee(new Employee("Шевченко Тарас", 40, 45000.0));

        // Data set 2
        manager.addEmployee(new Employee("Мельник Олена", 27, 29000.0));
        manager.addEmployee(new Employee("Бондаренко Богдан", 32, 33000.0));
        manager.addEmployee(new Employee("Кравченко Катерина", 29, 31000.0));
        manager.addEmployee(new Employee("Коваленко Костянтин", 34, 36000.0));
        manager.addEmployee(new Employee("Ткаченко Тимофій", 45, 48000.0));

        // Data set 3
        manager.addEmployee(new Employee("Савченко Софія", 26, 27500.0));
        manager.addEmployee(new Employee("Романенко Роман", 31, 33500.0));
        manager.addEmployee(new Employee("Данилюк Дарія", 30, 32500.0));
        manager.addEmployee(new Employee("Лисенко Леонід", 38, 40000.0));
        manager.addEmployee(new Employee("Павленко Павло", 42, 46000.0));

        // Output to console and file
        System.out.println("=== Співробітники, додані програмно ===");
        String initialData = manager.displayEmployees();
        System.out.println(initialData);

        // Save to file
        try {
            saveToFile("employees_output.txt", "=== Початковий список співробітників ===\n" + initialData);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні у файл: " + e.getMessage());
        }

        // Method 2: Interactive user mode
        Scanner scanner = new Scanner(System.in);
        boolean exit = false;

        while (!exit) {
            System.out.println("\nМеню управління співробітниками:");
            System.out.println("1. Додати співробітника");
            System.out.println("2. Видалити співробітника");
            System.out.println("3. Показати всіх співробітників");
            System.out.println("4. Вихід");
            System.out.print("Оберіть опцію (1-4): ");

            int choice = 0;
            try {
                choice = Integer.parseInt(scanner.nextLine());
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть число від 1 до 4.");
                continue;
            }

            switch (choice) {
                case 1:
                    addEmployeeInteractive(scanner, manager);
                    break;
                case 2:
                    removeEmployeeInteractive(scanner, manager);
                    break;
                case 3:
                    String employees = manager.displayEmployees();
                    System.out.println("=== Список співробітників ===");
                    System.out.println(employees);

                    // Save to file
                    try {
                        appendToFile("employees_output.txt", "\n\n=== Оновлений список співробітників ===\n" + employees);
                    } catch (IOException e) {
                        System.out.println("Помилка при збереженні у файл: " + e.getMessage());
                    }
                    break;
                case 4:
                    exit = true;
                    System.out.println("Дякуємо за використання програми!");
                    break;
                default:
                    System.out.println("Невірний вибір. Будь ласка, введіть число від 1 до 4.");
            }
        }

        scanner.close();
    }

    private static void addEmployeeInteractive(Scanner scanner, EmployeeManager manager) {
        System.out.println("=== Додавання нового співробітника ===");

        System.out.print("Введіть ПІБ: ");
        String name = scanner.nextLine();

        int age = 0;
        while (age < 18 || age > 65) {
            System.out.print("Введіть вік (18-65): ");
            try {
                age = Integer.parseInt(scanner.nextLine());
                if (age < 18 || age > 65) {
                    System.out.println("Вік має бути між 18 та 65 роками.");
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }

        double salary = 0;
        while (salary <= 0) {
            System.out.print("Введіть зарплату: ");
            try {
                salary = Double.parseDouble(scanner.nextLine());
                if (salary <= 0) {
                    System.out.println("Зарплата має бути більше 0.");
                }
            } catch (NumberFormatException e) {
                System.out.println("Будь ласка, введіть правильне число.");
            }
        }

        Employee employee = new Employee(name, age, salary);
        boolean added = manager.addEmployee(employee);

        if (added) {
            System.out.println("Співробітник успішно доданий!");

            // Save to file
            try {
                appendToFile("employees_output.txt", "\n\n=== Додано нового співробітника ===\n" + employee);
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
        } else {
            System.out.println("Співробітник з таким ім'ям вже існує!");
        }
    }

    private static void removeEmployeeInteractive(Scanner scanner, EmployeeManager manager) {
        System.out.println("=== Видалення співробітника ===");
        System.out.print("Введіть ПІБ співробітника для видалення: ");
        String name = scanner.nextLine();

        boolean removed = manager.removeEmployee(name);

        if (removed) {
            System.out.println("Співробітник успішно видалений!");

            // Save to file
            try {
                appendToFile("employees_output.txt", "\n\n=== Видалено співробітника ===\n" + name);
            } catch (IOException e) {
                System.out.println("Помилка при збереженні у файл: " + e.getMessage());
            }
        } else {
            System.out.println("Співробітник з таким ім'ям не знайдений!");
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

// Employee class
class Employee {
    private String name;
    private int age;
    private double salary;

    public Employee(String name, int age, double salary) {
        this.name = name;
        this.age = age;
        this.salary = salary;
    }

    public String getName() {
        return name;
    }

    public int getAge() {
        return age;
    }

    public double getSalary() {
        return salary;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Employee employee = (Employee) o;
        return Objects.equals(name, employee.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name);
    }

    @Override
    public String toString() {
        return String.format("ПІБ: %-25s | Вік: %-3d | Зарплата: %.2f грн", name, age, salary);
    }
}

// EmployeeManager class
class EmployeeManager {
    private Set<Employee> employees;

    public EmployeeManager() {
        this.employees = new LinkedHashSet<>();
    }

    public boolean addEmployee(Employee employee) {
        return employees.add(employee);
    }

    public boolean removeEmployee(String name) {
        return employees.removeIf(e -> e.getName().equalsIgnoreCase(name));
    }

    public String displayEmployees() {
        if (employees.isEmpty()) {
            return "Немає співробітників.";
        }

        StringBuilder sb = new StringBuilder();
        int index = 1;

        for (Employee employee : employees) {
            sb.append(String.format("%d. %s\n", index++, employee));
        }

        return sb.toString();
    }
}