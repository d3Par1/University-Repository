// Лабораторна робота 4 - Завдання 1
// Клас для зберігання даних про співробітників у LinkedHashSet
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.*;
import java.io.*;
import java.text.SimpleDateFormat;

// Клас співробітника
class Employee {
    private String name;
    private int age;
    private double salary;
    private String position;
    private String department;
    private Date hireDate;

    public Employee(String name, int age, double salary, String position, String department) {
        this.name = name;
        this.age = age;
        this.salary = salary;
        this.position = position;
        this.department = department;
        this.hireDate = new Date(); // поточна дата як дата найму
    }

    public Employee(String name, int age, double salary) {
        this(name, age, salary, "Не вказано", "Не вказано");
    }

    // Геттери
    public String getName() { return name; }
    public int getAge() { return age; }
    public double getSalary() { return salary; }
    public String getPosition() { return position; }
    public String getDepartment() { return department; }
    public Date getHireDate() { return hireDate; }

    // Сеттери
    public void setName(String name) { this.name = name; }
    public void setAge(int age) { this.age = age; }
    public void setSalary(double salary) { this.salary = salary; }
    public void setPosition(String position) { this.position = position; }
    public void setDepartment(String department) { this.department = department; }
    public void setHireDate(Date hireDate) { this.hireDate = hireDate; }

    // Рівність базується на імені та віку
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Employee employee = (Employee) obj;
        return age == employee.age && Objects.equals(name, employee.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name, age);
    }

    @Override
    public String toString() {
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd.MM.yyyy");
        return String.format("%-20s | %-3d років | %-8.2f грн | %-15s | %-12s | %s",
            name, age, salary, position, department, dateFormat.format(hireDate));
    }

    // Детальна інформація
    public String getDetailedInfo() {
        SimpleDateFormat dateFormat = new SimpleDateFormat("dd.MM.yyyy");
        return String.format(
            "Ім'я: %s\nВік: %d років\nЗарплата: %.2f грн\nПосада: %s\nВідділ: %s\nДата найму: %s\n",
            name, age, salary, position, department, dateFormat.format(hireDate));
    }
}

// Головний клас для управління співробітниками
public class Lr4_1 {
    private LinkedHashSet<Employee> employees;
    private Random random;

    public Lr4_1() {
        employees = new LinkedHashSet<>();
        random = new Random();
        initializeWithSampleData();
    }

    // Ініціалізація з тестовими даними
    private void initializeWithSampleData() {
        // Набір 1: IT відділ
        addEmployee(new Employee("Олександр Петренко", 28, 45000, "Java розробник", "IT"));
        addEmployee(new Employee("Марія Коваленко", 32, 52000, "Senior Developer", "IT"));
        addEmployee(new Employee("Дмитро Шевченко", 25, 38000, "Junior Developer", "IT"));

        // Набір 2: HR відділ
        addEmployee(new Employee("Анна Бондаренко", 30, 35000, "HR менеджер", "HR"));
        addEmployee(new Employee("Володимир Мельник", 45, 48000, "HR директор", "HR"));

        // Набір 3: Фінансовий відділ
        addEmployee(new Employee("Катерина Ткаченко", 27, 42000, "Бухгалтер", "Фінанси"));
        addEmployee(new Employee("Сергій Морозов", 35, 55000, "Фін. директор", "Фінанси"));

        System.out.println("✅ Початкові дані завантажено: " + employees.size() + " співробітників");
    }

    // Додавання співробітника
    public boolean addEmployee(Employee employee) {
        if (employee == null) {
            System.out.println("❌ Помилка: співробітник не може бути null");
            return false;
        }

        boolean added = employees.add(employee);
        if (added) {
            System.out.println("✅ Співробітника додано: " + employee.getName());
        } else {
            System.out.println("⚠️ Співробітник з таким ім'ям та віком вже існує: " + employee.getName());
        }
        return added;
    }

    // Видалення співробітника
    public boolean removeEmployee(Employee employee) {
        boolean removed = employees.remove(employee);
        if (removed) {
            System.out.println("✅ Співробітника видалено: " + employee.getName());
        } else {
            System.out.println("❌ Співробітника не знайдено");
        }
        return removed;
    }

    // Пошук співробітника за іменем
    public Employee findEmployeeByName(String name) {
        for (Employee emp : employees) {
            if (emp.getName().equalsIgnoreCase(name)) {
                return emp;
            }
        }
        return null;
    }

    // Пошук співробітників за віком
    public List<Employee> findEmployeesByAge(int age) {
        List<Employee> result = new ArrayList<>();
        for (Employee emp : employees) {
            if (emp.getAge() == age) {
                result.add(emp);
            }
        }
        return result;
    }

    // Пошук співробітників за відділом
    public List<Employee> findEmployeesByDepartment(String department) {
        List<Employee> result = new ArrayList<>();
        for (Employee emp : employees) {
            if (emp.getDepartment().equalsIgnoreCase(department)) {
                result.add(emp);
            }
        }
        return result;
    }

    // Пошук співробітників за діапазоном зарплати
    public List<Employee> findEmployeesBySalaryRange(double minSalary, double maxSalary) {
        List<Employee> result = new ArrayList<>();
        for (Employee emp : employees) {
            if (emp.getSalary() >= minSalary && emp.getSalary() <= maxSalary) {
                result.add(emp);
            }
        }
        return result;
    }

    // Виведення всіх співробітників
    public void displayAllEmployees() {
        System.out.println("\n👥 СПИСОК СПІВРОБІТНИКІВ (" + employees.size() + " осіб)");
        System.out.println("=" .repeat(85));

        if (employees.isEmpty()) {
            System.out.println("Список співробітників порожній");
            return;
        }

        System.out.println("Ім'я                 | Вік     | Зарплата  | Посада          | Відділ       | Дата найму");
        System.out.println("-" .repeat(85));

        for (Employee emp : employees) {
            System.out.println(emp);
        }

        System.out.println("-" .repeat(85));
        System.out.printf("Загальна кількість: %d співробітників\n", employees.size());
    }

    // Статистика
    public void displayStatistics() {
        System.out.println("\n📊 СТАТИСТИКА СПІВРОБІТНИКІВ");
        System.out.println("=" .repeat(40));

        if (employees.isEmpty()) {
            System.out.println("Немає даних для статистики");
            return;
        }

        // Загальна статистика
        double totalSalary = employees.stream().mapToDouble(Employee::getSalary).sum();
        double avgSalary = employees.stream().mapToDouble(Employee::getSalary).average().orElse(0);
        double maxSalary = employees.stream().mapToDouble(Employee::getSalary).max().orElse(0);
        double minSalary = employees.stream().mapToDouble(Employee::getSalary).min().orElse(0);

        double avgAge = employees.stream().mapToInt(Employee::getAge).average().orElse(0);
        int maxAge = employees.stream().mapToInt(Employee::getAge).max().orElse(0);
        int minAge = employees.stream().mapToInt(Employee::getAge).min().orElse(0);

        System.out.printf("Загальна кількість: %d\n", employees.size());
        System.out.printf("Загальний фонд зарплати: %.2f грн\n", totalSalary);
        System.out.printf("Середня зарплата: %.2f грн\n", avgSalary);
        System.out.printf("Мінімальна зарплата: %.2f грн\n", minSalary);
        System.out.printf("Максимальна зарплата: %.2f грн\n", maxSalary);
        System.out.printf("Середній вік: %.1f років\n", avgAge);
        System.out.printf("Мінімальний вік: %d років\n", minAge);
        System.out.printf("Максимальний вік: %d років\n", maxAge);

        // Статистика по відділах
        System.out.println("\nСтатистика по відділах:");
        Map<String, List<Employee>> byDepartment = new HashMap<>();
        for (Employee emp : employees) {
            byDepartment.computeIfAbsent(emp.getDepartment(), k -> new ArrayList<>()).add(emp);
        }

        for (Map.Entry<String, List<Employee>> entry : byDepartment.entrySet()) {
            String dept = entry.getKey();
            List<Employee> deptEmployees = entry.getValue();
            double deptAvgSalary = deptEmployees.stream().mapToDouble(Employee::getSalary).average().orElse(0);

            System.out.printf("  %s: %d осіб, середня зарплата: %.2f грн\n",
                dept, deptEmployees.size(), deptAvgSalary);
        }

        // Вікові групи
        System.out.println("\nВікові групи:");
        long young = employees.stream().filter(e -> e.getAge() < 30).count();
        long middle = employees.stream().filter(e -> e.getAge() >= 30 && e.getAge() < 45).count();
        long senior = employees.stream().filter(e -> e.getAge() >= 45).count();

        System.out.printf("  Молоді (< 30 років): %d\n", young);
        System.out.printf("  Середні (30-44 роки): %d\n", middle);
        System.out.printf("  Старші (45+ років): %d\n", senior);
    }

    // Сортування співробітників
    public void sortEmployees(String criterion) {
        List<Employee> sorted = new ArrayList<>(employees);

        switch (criterion.toLowerCase()) {
            case "name":
            case "ім'я":
                sorted.sort(Comparator.comparing(Employee::getName));
                break;
            case "age":
            case "вік":
                sorted.sort(Comparator.comparing(Employee::getAge));
                break;
            case "salary":
            case "зарплата":
                sorted.sort(Comparator.comparing(Employee::getSalary).reversed());
                break;
            case "department":
            case "відділ":
                sorted.sort(Comparator.comparing(Employee::getDepartment)
                    .thenComparing(Employee::getName));
                break;
            default:
                System.out.println("❌ Невірний критерій сортування");
                return;
        }

        System.out.println("\n📋 Співробітники відсортовані за: " + criterion);
        System.out.println("=" .repeat(85));
        System.out.println("Ім'я                 | Вік     | Зарплата  | Посада          | Відділ       | Дата найму");
        System.out.println("-" .repeat(85));

        for (Employee emp : sorted) {
            System.out.println(emp);
        }
    }

    // Збереження у файл
    public void saveToFile(String filename) {
        try (PrintWriter writer = new PrintWriter(new FileWriter(filename))) {
            writer.println("=== ЗВІТ ПО СПІВРОБІТНИКАХ ===");
            writer.println("Дата створення: " + new SimpleDateFormat("dd.MM.yyyy HH:mm:ss").format(new Date()));
            writer.println("Студент: групи ТВ-43, прізвище Step, варіант 16");
            writer.println();

            writer.println("ПОЧАТКОВІ ДАНІ:");
            for (Employee emp : employees) {
                writer.println(emp.getDetailedInfo());
                writer.println("-" .repeat(40));
            }

            writer.println("СТАТИСТИКА:");
            if (!employees.isEmpty()) {
                double avgSalary = employees.stream().mapToDouble(Employee::getSalary).average().orElse(0);
                double avgAge = employees.stream().mapToInt(Employee::getAge).average().orElse(0);

                writer.printf("Загальна кількість: %d співробітників\n", employees.size());
                writer.printf("Середня зарплата: %.2f грн\n", avgSalary);
                writer.printf("Середній вік: %.1f років\n", avgAge);
            }

            System.out.println("✅ Дані збережено у файл: " + filename);
        } catch (IOException e) {
            System.out.println("❌ Помилка збереження файлу: " + e.getMessage());
        }
    }

    // Генерація випадкових співробітників
    public void generateRandomEmployees(int count) {
        String[] names = {
            "Олексій Іваненко", "Тетяна Петрова", "Микола Сидоренко", "Ольга Коваль",
            "Андрій Лисенко", "Юлія Федорова", "Віктор Мороз", "Наталія Козлова",
            "Павло Крамар", "Світлана Гончар", "Роман Клименко", "Ірина Савченко"
        };

        String[] positions = {
            "Менеджер", "Аналітик", "Спеціаліст", "Консультант", "Координатор",
            "Розробник", "Дизайнер", "Тестувальник", "Адміністратор"
        };

        String[] departments = {"IT", "HR", "Фінанси", "Маркетинг", "Продажі", "Логістика"};

        System.out.printf("Генерація %d випадкових співробітників...\n", count);

        for (int i = 0; i < count; i++) {
            String name = names[random.nextInt(names.length)];
            int age = 22 + random.nextInt(43); // 22-65 років
            double salary = 25000 + random.nextInt(75000); // 25k-100k
            String position = positions[random.nextInt(positions.length)];
            String department = departments[random.nextInt(departments.length)];

            Employee emp = new Employee(name, age, salary, position, department);
            addEmployee(emp);
        }
    }

    // Інтерактивне меню
    public void showInteractiveMenu() {
        Scanner scanner = new Scanner(System.in);

        while (true) {
            System.out.println("\n🎮 МЕНЮ УПРАВЛІННЯ СПІВРОБІТНИКАМИ");
            System.out.println("=" .repeat(40));
            System.out.println("1. Показати всіх співробітників");
            System.out.println("2. Додати співробітника");
            System.out.println("3. Видалити співробітника");
            System.out.println("4. Знайти співробітника");
            System.out.println("5. Показати статистику");
            System.out.println("6. Сортувати співробітників");
            System.out.println("7. Генерувати випадкових співробітників");
            System.out.println("8. Зберегти у файл");
            System.out.println("0. Вийти");
            System.out.print("Ваш вибір: ");

            try {
                int choice = scanner.nextInt();
                scanner.nextLine(); // споживаємо новий рядок

                switch (choice) {
                    case 1:
                        displayAllEmployees();
                        break;

                    case 2:
                        System.out.print("Введіть ім'я: ");
                        String name = scanner.nextLine();
                        System.out.print("Введіть вік: ");
                        int age = scanner.nextInt();
                        System.out.print("Введіть зарплату: ");
                        double salary = scanner.nextDouble();
                        scanner.nextLine();
                        System.out.print("Введіть посаду: ");
                        String position = scanner.nextLine();
                        System.out.print("Введіть відділ: ");
                        String department = scanner.nextLine();

                        Employee newEmp = new Employee(name, age, salary, position, department);
                        addEmployee(newEmp);
                        break;

                    case 3:
                        System.out.print("Введіть ім'я співробітника для видалення: ");
                        String removeName = scanner.nextLine();
                        Employee empToRemove = findEmployeeByName(removeName);
                        if (empToRemove != null) {
                            removeEmployee(empToRemove);
                        } else {
                            System.out.println("❌ Співробітника не знайдено");
                        }
                        break;

                    case 4:
                        System.out.println("Пошук за:");
                        System.out.println("1 - Ім'ям, 2 - Віком, 3 - Відділом, 4 - Зарплатою");
                        int searchType = scanner.nextInt();
                        scanner.nextLine();

                        if (searchType == 1) {
                            System.out.print("Введіть ім'я: ");
                            String searchName = scanner.nextLine();
                            Employee found = findEmployeeByName(searchName);
                            if (found != null) {
                                System.out.println("✅ Знайдено: " + found);
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        } else if (searchType == 2) {
                            System.out.print("Введіть вік: ");
                            int searchAge = scanner.nextInt();
                            List<Employee> foundByAge = findEmployeesByAge(searchAge);
                            if (!foundByAge.isEmpty()) {
                                System.out.println("✅ Знайдено " + foundByAge.size() + " співробітників:");
                                foundByAge.forEach(System.out::println);
                            } else {
                                System.out.println("❌ Не знайдено");
                            }
                        }
                        break;

                    case 5:
                        displayStatistics();
                        break;

                    case 6:
                        System.out.println("Сортувати за: name/age/salary/department");
                        String sortBy = scanner.nextLine();
                        sortEmployees(sortBy);
                        break;

                    case 7:
                        System.out.print("Кількість співробітників для генерації: ");
                        int count = scanner.nextInt();
                        generateRandomEmployees(count);
                        break;

                    case 8:
                        System.out.print("Введіть ім'я файлу (наприклад, employees.txt): ");
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
        System.out.println("=== Лабораторна робота 4 - Завдання 1 ===");
        System.out.println("LinkedHashSet: управління співробітниками");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        Lr4_1 program = new Lr4_1();

        // Демонстрація основного функціоналу
        program.displayAllEmployees();
        program.displayStatistics();

        // Демонстрація пошуку
        System.out.println("\n🔍 ДЕМОНСТРАЦІЯ ПОШУКУ:");
        Employee found = program.findEmployeeByName("Марія Коваленко");
        if (found != null) {
            System.out.println("Знайдено співробітника:");
            System.out.println(found.getDetailedInfo());
        }

        // Демонстрація сортування
        program.sortEmployees("salary");

        // Збереження у файл
        program.saveToFile("employees_output.txt");

        // Інтерактивне меню
        program.showInteractiveMenu();

        System.out.println("\n✅ Програма завершена успішно!");
    }
}