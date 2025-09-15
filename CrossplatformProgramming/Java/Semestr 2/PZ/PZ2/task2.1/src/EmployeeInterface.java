import java.util.ArrayList;
import java.util.List;

// Інтерфейс Співробітник
interface Employee {
    // Методи інтерфейсу
    String getName();
    int getYearsOfExperience();
    double calculateSalary();
    int getVacationDays();
}

// Клас Керівник
class Director implements Employee {
    private String name;
    private int yearsOfExperience;
    private static final double BASE_SALARY = 50000;
    private static final double EXPERIENCE_MULTIPLIER = 2000;

    // Конструктор
    public Director(String name, int yearsOfExperience) {
        this.name = name;
        this.yearsOfExperience = yearsOfExperience;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getYearsOfExperience() {
        return yearsOfExperience;
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (yearsOfExperience * EXPERIENCE_MULTIPLIER);
    }

    @Override
    public int getVacationDays() {
        return 24 + 10; // Базова відпустка + додаткові дні для керівника
    }
}

// Клас Менеджер
class Manager implements Employee {
    private String name;
    private int yearsOfExperience;
    private List<Employee> subordinates;
    private static final double BASE_SALARY = 35000;
    private static final double EXPERIENCE_MULTIPLIER = 1500;

    // Конструктор
    public Manager(String name, int yearsOfExperience) {
        this.name = name;
        this.yearsOfExperience = yearsOfExperience;
        this.subordinates = new ArrayList<>();
    }

    // Метод для додавання підлеглого
    public void addSubordinate(Employee employee) {
        subordinates.add(employee);
    }

    // Метод для підрахунку кількості підлеглих
    public int countSubordinates() {
        return subordinates.size();
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getYearsOfExperience() {
        return yearsOfExperience;
    }

    @Override
    public double calculateSalary() {
        // Зарплата менеджера залежить від кількості підлеглих
        return BASE_SALARY + (yearsOfExperience * EXPERIENCE_MULTIPLIER) +
                (subordinates.size() * 1000);
    }

    @Override
    public int getVacationDays() {
        return 24 + 5; // Базова відпустка + додаткові дні для менеджера
    }
}

// Клас Інженер
class Engineer implements Employee {
    private String name;
    private int yearsOfExperience;
    private static final double BASE_SALARY = 30000;
    private static final double EXPERIENCE_MULTIPLIER = 1200;

    // Конструктор
    public Engineer(String name, int yearsOfExperience) {
        this.name = name;
        this.yearsOfExperience = yearsOfExperience;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getYearsOfExperience() {
        return yearsOfExperience;
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (yearsOfExperience * EXPERIENCE_MULTIPLIER);
    }

    @Override
    public int getVacationDays() {
        // Інженери отримують додаткові дні відпустки за досвід
        int additionalDays = yearsOfExperience / 3;
        return 24 + additionalDays;
    }
}

// Клас Механік
class Mechanic implements Employee {
    private String name;
    private int yearsOfExperience;
    private static final double BASE_SALARY = 25000;
    private static final double EXPERIENCE_MULTIPLIER = 1000;

    // Конструктор
    public Mechanic(String name, int yearsOfExperience) {
        this.name = name;
        this.yearsOfExperience = yearsOfExperience;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getYearsOfExperience() {
        return yearsOfExperience;
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (yearsOfExperience * EXPERIENCE_MULTIPLIER);
    }

    @Override
    public int getVacationDays() {
        return 24; // Базова відпустка
    }
}

// Клас Інженер-архітектор
class EngineerArchitect implements Employee {
    private String name;
    private int yearsOfExperience;
    private static final double BASE_SALARY = 35000;
    private static final double EXPERIENCE_MULTIPLIER = 1500;

    // Конструктор
    public EngineerArchitect(String name, int yearsOfExperience) {
        this.name = name;
        this.yearsOfExperience = yearsOfExperience;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public int getYearsOfExperience() {
        return yearsOfExperience;
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (yearsOfExperience * EXPERIENCE_MULTIPLIER) + 5000; // Додатковий бонус
    }

    @Override
    public int getVacationDays() {
        // Інженери-архітектори отримують додаткові дні відпустки за досвід
        int additionalDays = yearsOfExperience / 3;
        return 24 + additionalDays + 3; // Базова відпустка + за досвід + додаткові дні
    }
}

// Демонстраційний клас
public class EmployeeInterface {
    public static void main(String[] args) {
        // Створення співробітників
        Director director = new Director("Олександр Петрович", 15);
        Manager manager = new Manager("Ірина Сергіївна", 8);
        Engineer engineer1 = new Engineer("Максим Іванов", 5);
        Engineer engineer2 = new Engineer("Андрій Коваль", 3);
        Mechanic mechanic = new Mechanic("Володимир Сидоренко", 10);
        EngineerArchitect architect = new EngineerArchitect("Наталія Шевченко", 7);

        // Додавання підлеглих до менеджера
        manager.addSubordinate(engineer1);
        manager.addSubordinate(engineer2);
        manager.addSubordinate(mechanic);
        manager.addSubordinate(architect);

        // Виведення інформації про співробітників
        System.out.println("Інформація про співробітників:");

        System.out.println("\nКерівник:");
        displayEmployeeInfo(director);

        System.out.println("\nМенеджер:");
        displayEmployeeInfo(manager);
        System.out.println("Кількість підлеглих: " + manager.countSubordinates());

        System.out.println("\nІнженери:");
        displayEmployeeInfo(engineer1);
        displayEmployeeInfo(engineer2);

        System.out.println("\nМеханік:");
        displayEmployeeInfo(mechanic);

        System.out.println("\nІнженер-архітектор:");
        displayEmployeeInfo(architect);

        // Виведення кількості днів відпустки для інженерів
        System.out.println("\nКількість днів щорічної відпустки у інженерів:");
        System.out.println(engineer1.getName() + ": " + engineer1.getVacationDays() + " днів");
        System.out.println(engineer2.getName() + ": " + engineer2.getVacationDays() + " днів");
        System.out.println(architect.getName() + " (Інженер-архітектор): " + architect.getVacationDays() + " днів");
    }

    // Допоміжний метод для виведення інформації про співробітника
    private static void displayEmployeeInfo(Employee employee) {
        System.out.println("Ім'я: " + employee.getName());
        System.out.println("Досвід: " + employee.getYearsOfExperience() + " років");
        System.out.println("Зарплата: " + employee.calculateSalary() + " грн");
        System.out.println("Відпустка: " + employee.getVacationDays() + " днів");
    }
}