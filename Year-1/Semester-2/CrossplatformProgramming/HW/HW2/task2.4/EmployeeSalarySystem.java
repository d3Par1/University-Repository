import java.io.*;
import java.util.*;

// Базовий абстрактний клас Співробітник
abstract class Employee implements Comparable<Employee> {
    private int id;
    private String firstName;
    private String lastName;
    private String middleName;

    // Конструктор
    public Employee(int id, String lastName, String firstName, String middleName) {
        this.id = id;
        this.lastName = lastName;
        this.firstName = firstName;
        this.middleName = middleName;
    }

    // Гетери
    public int getId() {
        return id;
    }

    public String getFirstName() {
        return firstName;
    }

    public String getLastName() {
        return lastName;
    }

    public String getMiddleName() {
        return middleName;
    }

    // Метод для отримання повного імені
    public String getFullName() {
        return lastName + " " + firstName + " " + middleName;
    }

    // Абстрактний метод для розрахунку середньомісячної зарплати
    public abstract double calculateMonthlySalary();

    // Метод для порівняння працівників за зарплатою та прізвищем
    @Override
    public int compareTo(Employee other) {
        // Спочатку порівнюємо за зарплатою (в спадаючому порядку)
        int salaryComparison = Double.compare(other.calculateMonthlySalary(), this.calculateMonthlySalary());

        // Якщо зарплати однакові, порівнюємо за прізвищем
        if (salaryComparison == 0) {
            return this.lastName.compareTo(other.lastName);
        }

        return salaryComparison;
    }

    // Абстрактний метод для серіалізації працівника у рядок для запису у файл
    public abstract String toFileString();

    // Метод для отримання типу працівника
    public abstract String getEmployeeType();
}

// Клас для співробітника з погодинною оплатою
class HourlyEmployee extends Employee {
    private double hourlyRate;

    // Конструктор
    public HourlyEmployee(int id, String lastName, String firstName, String middleName, double hourlyRate) {
        super(id, lastName, firstName, middleName);
        this.hourlyRate = hourlyRate;
    }

    // Гетери і сетери
    public double getHourlyRate() {
        return hourlyRate;
    }

    public void setHourlyRate(double hourlyRate) {
        this.hourlyRate = hourlyRate;
    }

    // Розрахунок середньомісячної зарплати
    @Override
    public double calculateMonthlySalary() {
        // Формула: 20.8 * 8 * ставка за годину
        return 20.8 * 8 * hourlyRate;
    }

    // Серіалізація у рядок для запису у файл
    @Override
    public String toFileString() {
        return "HOURLY," + getId() + "," + getLastName() + "," + getFirstName() + "," +
                getMiddleName() + "," + hourlyRate;
    }

    // Отримання типу працівника
    @Override
    public String getEmployeeType() {
        return "Погодинник";
    }

    // Метод для створення працівника з рядка з файлу
    public static HourlyEmployee fromFileString(String line) {
        String[] parts = line.split(",");
        if (parts.length != 6 || !parts[0].equals("HOURLY")) {
            throw new IllegalArgumentException("Неправильний формат рядка для працівника з погодинною оплатою");
        }

        int id = Integer.parseInt(parts[1]);
        String lastName = parts[2];
        String firstName = parts[3];
        String middleName = parts[4];
        double hourlyRate = Double.parseDouble(parts[5]);

        return new HourlyEmployee(id, lastName, firstName, middleName, hourlyRate);
    }
}

// Клас для співробітника з фіксованою оплатою
class SalariedEmployee extends Employee {
    private double monthlySalary;

    // Конструктор
    public SalariedEmployee(int id, String lastName, String firstName, String middleName, double monthlySalary) {
        super(id, lastName, firstName, middleName);
        this.monthlySalary = monthlySalary;
    }

    // Гетери і сетери
    public double getMonthlySalary() {
        return monthlySalary;
    }

    public void setMonthlySalary(double monthlySalary) {
        this.monthlySalary = monthlySalary;
    }

    // Розрахунок середньомісячної зарплати
    @Override
    public double calculateMonthlySalary() {
        // Формула: фіксована місячна оплата
        return monthlySalary;
    }

    // Серіалізація у рядок для запису у файл
    @Override
    public String toFileString() {
        return "SALARIED," + getId() + "," + getLastName() + "," + getFirstName() + "," +
                getMiddleName() + "," + monthlySalary;
    }

    // Отримання типу працівника
    @Override
    public String getEmployeeType() {
        return "Фіксована оплата";
    }

    // Метод для створення працівника з рядка з файлу
    public static SalariedEmployee fromFileString(String line) {
        String[] parts = line.split(",");
        if (parts.length != 6 || !parts[0].equals("SALARIED")) {
            throw new IllegalArgumentException("Неправильний формат рядка для працівника з фіксованою оплатою");
        }

        int id = Integer.parseInt(parts[1]);
        String lastName = parts[2];
        String firstName = parts[3];
        String middleName = parts[4];
        double monthlySalary = Double.parseDouble(parts[5]);

        return new SalariedEmployee(id, lastName, firstName, middleName, monthlySalary);
    }
}

// Головний клас програми
public class EmployeeSalarySystem {
    private static final String FILENAME = "employees.txt";
    private static List<Employee> employees = new ArrayList<>();
    private static Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        boolean running = true;

        while (running) {
            System.out.println("\n===== Система обліку співробітників =====");
            System.out.println("1. Додати нового співробітника");
            System.out.println("2. Показати всіх співробітників");
            System.out.println("3. Упорядкувати за спаданням зарплати");
            System.out.println("4. Показати перших 5 співробітників");
            System.out.println("5. Показати останні 3 ідентифікатори");
            System.out.println("6. Зберегти дані у файл");
            System.out.println("7. Завантажити дані з файлу");
            System.out.println("0. Вихід");

            System.out.print("Ваш вибір: ");
            int choice = getIntInput();

            switch (choice) {
                case 1:
                    addEmployee();
                    break;
                case 2:
                    displayAllEmployees();
                    break;
                case 3:
                    sortEmployees();
                    break;
                case 4:
                    displayTop5Employees();
                    break;
                case 5:
                    displayLast3EmployeeIds();
                    break;
                case 6:
                    saveEmployeesToFile();
                    break;
                case 7:
                    loadEmployeesFromFile();
                    break;
                case 0:
                    running = false;
                    break;
                default:
                    System.out.println("Невірний вибір. Спробуйте ще раз.");
            }
        }

        scanner.close();
        System.out.println("Дякуємо за використання системи!");
    }

    // Метод для отримання числового введення
    private static int getIntInput() {
        try {
            return Integer.parseInt(scanner.nextLine());
        } catch (NumberFormatException e) {
            return -1; // Повертаємо недійсне значення для обробки в коді виклику
        }
    }

    // Метод для отримання введення з плаваючою комою
    private static double getDoubleInput() {
        try {
            return Double.parseDouble(scanner.nextLine());
        } catch (NumberFormatException e) {
            return -1.0; // Повертаємо недійсне значення для обробки в коді виклику
        }
    }

    // Метод для додавання нового співробітника
    private static void addEmployee() {
        System.out.println("\n--- Додавання нового співробітника ---");

        System.out.print("Введіть ідентифікатор: ");
        int id = getIntInput();
        if (id < 0) {
            System.out.println("Помилка: Невірний ідентифікатор.");
            return;
        }

        // Перевірка на унікальність ідентифікатора
        for (Employee emp : employees) {
            if (emp.getId() == id) {
                System.out.println("Помилка: Співробітник з таким ідентифікатором вже існує.");
                return;
            }
        }

        System.out.print("Введіть прізвище: ");
        String lastName = scanner.nextLine();

        System.out.print("Введіть ім'я: ");
        String firstName = scanner.nextLine();

        System.out.print("Введіть по батькові: ");
        String middleName = scanner.nextLine();

        System.out.println("Виберіть тип оплати:");
        System.out.println("1. Погодинна оплата");
        System.out.println("2. Фіксована місячна оплата");
        System.out.print("Ваш вибір: ");
        int paymentType = getIntInput();

        switch (paymentType) {
            case 1:
                System.out.print("Введіть ставку за годину: ");
                double hourlyRate = getDoubleInput();
                if (hourlyRate < 0) {
                    System.out.println("Помилка: Невірна ставка за годину.");
                    return;
                }
                employees.add(new HourlyEmployee(id, lastName, firstName, middleName, hourlyRate));
                System.out.println("Співробітник з погодинною оплатою успішно доданий.");
                break;

            case 2:
                System.out.print("Введіть фіксовану місячну зарплату: ");
                double monthlySalary = getDoubleInput();
                if (monthlySalary < 0) {
                    System.out.println("Помилка: Невірна місячна зарплата.");
                    return;
                }
                employees.add(new SalariedEmployee(id, lastName, firstName, middleName, monthlySalary));
                System.out.println("Співробітник з фіксованою оплатою успішно доданий.");
                break;

            default:
                System.out.println("Помилка: Невірний тип оплати.");
        }
    }

    // Метод для відображення всіх співробітників
    private static void displayAllEmployees() {
        if (employees.isEmpty()) {
            System.out.println("Список співробітників порожній.");
            return;
        }

        System.out.println("\n--- Список всіх співробітників ---");
        System.out.println("ID  | ПІБ                             | Тип        | Зарплата (грн)");
        System.out.println("----|----------------------------------|------------|---------------");

        for (Employee emp : employees) {
            System.out.printf("%-4d| %-34s| %-11s| %.2f%n",
                    emp.getId(), emp.getFullName(), emp.getEmployeeType(), emp.calculateMonthlySalary());
        }
    }

    // Метод для сортування співробітників
    private static void sortEmployees() {
        if (employees.isEmpty()) {
            System.out.println("Список співробітників порожній.");
            return;
        }

        // Сортування за зарплатою (спадання) та прізвищем (при рівності зарплат)
        Collections.sort(employees);

        System.out.println("\nСпівробітники відсортовані за спаданням зарплати та за алфавітом за прізвищем.");
        displayAllEmployees();
    }

    // Метод для відображення перших 5 співробітників
    private static void displayTop5Employees() {
        if (employees.isEmpty()) {
            System.out.println("Список співробітників порожній.");
            return;
        }

        // Сортування, якщо ще не відсортовано
        Collections.sort(employees);

        System.out.println("\n--- Перші 5 співробітників за зарплатою ---");

        int count = Math.min(5, employees.size());
        for (int i = 0; i < count; i++) {
            Employee emp = employees.get(i);
            System.out.printf("%d. %s (%.2f грн)%n",
                    i + 1, emp.getFullName(), emp.calculateMonthlySalary());
        }
    }

    // Метод для відображення останніх 3 ідентифікаторів
    private static void displayLast3EmployeeIds() {
        if (employees.isEmpty()) {
            System.out.println("Список співробітників порожній.");
            return;
        }

        // Сортування, якщо ще не відсортовано
        Collections.sort(employees);

        System.out.println("\n--- Останні 3 ідентифікатори співробітників ---");

        int startIndex = Math.max(0, employees.size() - 3);
        for (int i = startIndex; i < employees.size(); i++) {
            Employee emp = employees.get(i);
            System.out.printf("ID: %d (%s)%n", emp.getId(), emp.getFullName());
        }
    }

    // Метод для збереження співробітників у файл
    private static void saveEmployeesToFile() {
        if (employees.isEmpty()) {
            System.out.println("Список співробітників порожній. Нічого зберігати.");
            return;
        }

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(FILENAME))) {
            for (Employee emp : employees) {
                writer.write(emp.toFileString());
                writer.newLine();
            }
            System.out.println("Дані успішно збережено у файл: " + FILENAME);
        } catch (IOException e) {
            System.out.println("Помилка при збереженні даних: " + e.getMessage());
        }
    }

    // Метод для завантаження співробітників з файлу
    private static void loadEmployeesFromFile() {
        File file = new File(FILENAME);
        if (!file.exists() || file.length() == 0) {
            System.out.println("Файл не існує або порожній: " + FILENAME);
            return;
        }

        try (BufferedReader reader = new BufferedReader(new FileReader(FILENAME))) {
            employees.clear(); // Очищаємо поточний список
            String line;
            int lineNumber = 0;

            while ((line = reader.readLine()) != null) {
                lineNumber++;
                try {
                    String[] parts = line.split(",");
                    if (parts.length < 1) {
                        System.out.println("Попередження: Рядок " + lineNumber + " має неправильний формат.");
                        continue;
                    }

                    switch (parts[0]) {
                        case "HOURLY":
                            employees.add(HourlyEmployee.fromFileString(line));
                            break;
                        case "SALARIED":
                            employees.add(SalariedEmployee.fromFileString(line));
                            break;
                        default:
                            System.out.println("Попередження: Рядок " + lineNumber +
                                    " містить невідомий тип співробітника: " + parts[0]);
                    }
                } catch (Exception e) {
                    System.out.println("Помилка при обробці рядка " + lineNumber + ": " + e.getMessage());
                }
            }

            System.out.println("Дані успішно завантажено з файлу: " + FILENAME);
            System.out.println("Завантажено " + employees.size() + " співробітників.");
        } catch (IOException e) {
            System.out.println("Помилка при читанні файлу: " + e.getMessage());
        }
    }
}