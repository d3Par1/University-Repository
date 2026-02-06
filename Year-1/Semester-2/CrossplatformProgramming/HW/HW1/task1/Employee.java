public class Employee {
    private String firstName;
    private String lastName;
    private double dailyRate;
    private int daysWorked;

    // Конструктор з усіма параметрами
    public Employee(String firstName, String lastName, double dailyRate, int daysWorked) {
        this.firstName = firstName;
        this.lastName = lastName;
        this.dailyRate = dailyRate;
        this.daysWorked = daysWorked;
    }

    // Конструктор з трьома параметрами (без кількості відпрацьованих днів)
    public Employee(String firstName, String lastName, double dailyRate) {
        this(firstName, lastName, dailyRate, 0);
    }

    // Конструктор з двома параметрами (тільки ім'я та прізвище)
    public Employee(String firstName, String lastName) {
        this(firstName, lastName, 0, 0);
    }

    // Конструктор без параметрів
    public Employee() {
        this("", "", 0, 0);
    }

    // Гетери та сетери для полів класу
    public String getFirstName() {
        return firstName;
    }

    public void setFirstName(String firstName) {
        this.firstName = firstName;
    }

    public String getLastName() {
        return lastName;
    }

    public void setLastName(String lastName) {
        this.lastName = lastName;
    }

    public double getDailyRate() {
        return dailyRate;
    }

    public void setDailyRate(double dailyRate) {
        this.dailyRate = dailyRate;
    }

    public int getDaysWorked() {
        return daysWorked;
    }

    public void setDaysWorked(int daysWorked) {
        this.daysWorked = daysWorked;
    }

    // Метод для виведення кількості відпрацьованих днів
    public void displayDaysWorked() {
        System.out.println("Кількість відпрацьованих днів: " + daysWorked);
    }

    // Метод для обчислення зарплати
    public double calculateSalary() {
        return dailyRate * daysWorked;
    }

    // Метод для виведення зарплати
    public void displaySalary() {
        System.out.println("Зарплата працівника " + firstName + " " + lastName + ": " + calculateSalary());
    }

    // Метод для порівняння зарплат двох працівників
    public static void compareSalaries(Employee emp1, Employee emp2) {
        double salary1 = emp1.calculateSalary();
        double salary2 = emp2.calculateSalary();

        System.out.println("Порівняння зарплат:");
        System.out.println(emp1.firstName + " " + emp1.lastName + ": " + salary1);
        System.out.println(emp2.firstName + " " + emp2.lastName + ": " + salary2);

        if (salary1 > salary2) {
            System.out.println(emp1.firstName + " " + emp1.lastName + " отримує більше");
        } else if (salary1 < salary2) {
            System.out.println(emp2.firstName + " " + emp2.lastName + " отримує більше");
        } else {
            System.out.println("Зарплати однакові");
        }
    }

    // Демонстрація роботи класу
    public static void main(String[] args) {
        // Створення працівників з різними конструкторами
        Employee emp1 = new Employee("Іван", "Петренко", 500, 20);
        Employee emp2 = new Employee("Олена", "Коваленко", 600, 18);
        Employee emp3 = new Employee("Петро", "Сидоренко", 450);
        Employee emp4 = new Employee("Марія", "Іванова");
        Employee emp5 = new Employee();

        // Встановлення значень для працівників, створених без повних даних
        emp3.setDaysWorked(22);
        emp4.setDailyRate(550);
        emp4.setDaysWorked(15);
        emp5.setFirstName("Андрій");
        emp5.setLastName("Мельник");
        emp5.setDailyRate(480);
        emp5.setDaysWorked(21);

        // Виведення інформації про працівників
        System.out.println("Інформація про працівників:");
        System.out.println("1. " + emp1.getFirstName() + " " + emp1.getLastName());
        emp1.displayDaysWorked();
        emp1.displaySalary();

        System.out.println("\n2. " + emp2.getFirstName() + " " + emp2.getLastName());
        emp2.displayDaysWorked();
        emp2.displaySalary();

        System.out.println("\n3. " + emp3.getFirstName() + " " + emp3.getLastName());
        emp3.displayDaysWorked();
        emp3.displaySalary();

        System.out.println("\n4. " + emp4.getFirstName() + " " + emp4.getLastName());
        emp4.displayDaysWorked();
        emp4.displaySalary();

        System.out.println("\n5. " + emp5.getFirstName() + " " + emp5.getLastName());
        emp5.displayDaysWorked();
        emp5.displaySalary();

        // Порівняння зарплат
        System.out.println("\nПорівняння зарплат:");
        Employee.compareSalaries(emp1, emp2);
        Employee.compareSalaries(emp3, emp5);
    }
}