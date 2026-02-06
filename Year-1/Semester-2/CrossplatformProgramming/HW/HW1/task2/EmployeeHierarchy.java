import java.util.ArrayList;
import java.util.List;

// Абстрактний клас Співробітник
abstract class Employee {
    private final String name;
    private final int yearsOfExperience;

    // Конструктор
    public Employee(String name, int yearsOfExperience) {
        this.name = name;
        this.yearsOfExperience = yearsOfExperience;
    }

    // Гетери
    public String getName() {
        return name;
    }

    public int getYearsOfExperience() {
        return yearsOfExperience;
    }

    // Абстрактний метод для обчислення зарплати
    public abstract double calculateSalary();

    // Метод для отримання кількості днів відпустки
    public int getVacationDays() {
        // Базова кількість днів відпустки
        return 24;
    }
}

// Клас Керівник
class Director extends Employee {
    private static final double BASE_SALARY = 50000;
    private static final double EXPERIENCE_MULTIPLIER = 2000;

    public Director(String name, int yearsOfExperience) {
        super(name, yearsOfExperience);
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (getYearsOfExperience() * EXPERIENCE_MULTIPLIER);
    }

    @Override
    public int getVacationDays() {
        return super.getVacationDays() + 10; // Керівники мають додаткові дні відпустки
    }
}

// Клас Менеджер
class Manager extends Employee {
    private static final double BASE_SALARY = 35000;
    private static final double EXPERIENCE_MULTIPLIER = 1500;
    private List<Employee> subordinates;

    public Manager(String name, int yearsOfExperience) {
        super(name, yearsOfExperience);
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

    // Метод для підрахунку інженерів, механіків та архітекторів
    public int[] countSubordinatesByType() {
        int[] counts = new int[3]; // [інженери, механіки, архітектори]

        for (Employee employee : subordinates) {
            if (employee instanceof Engineer) {
                counts[0]++;
            } else if (employee instanceof Mechanic) {
                counts[1]++;
            } else if (employee instanceof EngineerArchitect) {
                counts[2]++;
            }
        }

        return counts;
    }

    @Override
    public double calculateSalary() {
        // Зарплата менеджера залежить від кількості підлеглих
        return BASE_SALARY + (getYearsOfExperience() * EXPERIENCE_MULTIPLIER) +
                (subordinates.size() * 1000);
    }

    @Override
    public int getVacationDays() {
        return super.getVacationDays() + 5; // Менеджери мають додаткові дні відпустки
    }
}

// Клас Інженер
class Engineer extends Employee {
    private static final double BASE_SALARY = 30000;
    private static final double EXPERIENCE_MULTIPLIER = 1200;

    public Engineer(String name, int yearsOfExperience) {
        super(name, yearsOfExperience);
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (getYearsOfExperience() * EXPERIENCE_MULTIPLIER);
    }

    @Override
    public int getVacationDays() {
        // Інженери отримують додаткові дні відпустки за досвід
        int additionalDays = getYearsOfExperience() / 3;
        return super.getVacationDays() + additionalDays;
    }
}

// Клас Механік
class Mechanic extends Employee {
    private static final double BASE_SALARY = 25000;
    private static final double EXPERIENCE_MULTIPLIER = 1000;

    public Mechanic(String name, int yearsOfExperience) {
        super(name, yearsOfExperience);
    }

    @Override
    public double calculateSalary() {
        return BASE_SALARY + (getYearsOfExperience() * EXPERIENCE_MULTIPLIER);
    }
}

// Клас Інженер-архітектор
class EngineerArchitect extends Engineer {
    private static final double ARCHITECT_BONUS = 5000;

    public EngineerArchitect(String name, int yearsOfExperience) {
        super(name, yearsOfExperience);
    }

    @Override
    public double calculateSalary() {
        // Інженер-архітектор отримує додатковий бонус
        return super.calculateSalary() + ARCHITECT_BONUS;
    }

    @Override
    public int getVacationDays() {
        // Інженери-архітектори отримують більше відпустки
        return super.getVacationDays() + 3;
    }
}

// Демонстраційний клас
public class EmployeeHierarchy {
    public static void main(String[] args) {
        // Створення співробітників
        Director director = new Director("Олександр Петрович", 15);
        Manager manager = new Manager("Ірина Сергіївна", 8);
        Engineer engineer1 = new Engineer("Максим Іванов", 5);
        Engineer engineer2 = new Engineer("Андрій Коваль", 3);
        Mechanic mechanic1 = new Mechanic("Володимир Сидоренко", 10);
        Mechanic mechanic2 = new Mechanic("Олег Мельник", 6);
        EngineerArchitect architect = new EngineerArchitect("Наталія Шевченко", 7);

        // Додавання підлеглих до менеджера
        manager.addSubordinate(engineer1);
        manager.addSubordinate(engineer2);
        manager.addSubordinate(mechanic1);
        manager.addSubordinate(mechanic2);
        manager.addSubordinate(architect);

        // Виведення інформації про співробітників
        System.out.println("Інформація про співробітників:");
        System.out.println("\nКерівник:");
        displayEmployeeInfo(director);

        System.out.println("\nМенеджер:");
        displayEmployeeInfo(manager);
        int[] subordinateCounts = manager.countSubordinatesByType();
        System.out.println("Кількість підлеглих: " + manager.countSubordinates());
        System.out.println("- Інженерів: " + subordinateCounts[0]);
        System.out.println("- Механіків: " + subordinateCounts[1]);
        System.out.println("- Інженерів-архітекторів: " + subordinateCounts[2]);

        System.out.println("\nІнженери:");
        displayEmployeeInfo(engineer1);
        displayEmployeeInfo(engineer2);

        System.out.println("\nМеханіки:");
        displayEmployeeInfo(mechanic1);
        displayEmployeeInfo(mechanic2);

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