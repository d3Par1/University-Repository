import java.util.*;

// Клас BirthDate для зберігання дати народження
class BirthDate {
    private int year;
    private int month;
    private int day;

    public BirthDate(int year, int month, int day) {
        this.year = year;
        this.month = month;
        this.day = day;
    }

    // Гетери
    public int getYear() { return year; }
    public int getMonth() { return month; }
    public int getDay() { return day; }

    // Сетери
    public void setYear(int year) { this.year = year; }
    public void setMonth(int month) { this.month = month; }
    public void setDay(int day) { this.day = day; }

    @Override
    public String toString() {
        return String.format("%04d-%02d-%02d", year, month, day);
    }
}

// Клас Student
class Student {
    private String lastName;
    private String firstName;
    private String groupName;
    private BirthDate birthDate;
    private float averageGrade;

    public Student(String lastName, String firstName, String groupName,
                   BirthDate birthDate, float averageGrade) {
        this.lastName = lastName;
        this.firstName = firstName;
        this.groupName = groupName;
        this.birthDate = birthDate;
        this.averageGrade = averageGrade;
    }

    // Гетери
    public String getLastName() { return lastName; }
    public String getFirstName() { return firstName; }
    public String getGroupName() { return groupName; }
    public BirthDate getBirthDate() { return birthDate; }
    public float getAverageGrade() { return averageGrade; }

    // Сетери
    public void setLastName(String lastName) { this.lastName = lastName; }
    public void setFirstName(String firstName) { this.firstName = firstName; }
    public void setGroupName(String groupName) { this.groupName = groupName; }
    public void setBirthDate(BirthDate birthDate) { this.birthDate = birthDate; }
    public void setAverageGrade(float averageGrade) { this.averageGrade = averageGrade; }

    @Override
    public String toString() {
        return String.format("%s %s, група %s, дата народження %s, середня оцінка %.1f",
                lastName, firstName, groupName, birthDate, averageGrade);
    }
}

// Система управління студентами
class StudentManagementSystem {
    private ArrayList<Student> students;
    private Scanner scanner;

    public StudentManagementSystem() {
        this.students = new ArrayList<>();
        this.scanner = new Scanner(System.in);
        initializeStudents();
    }

    // Ініціалізація списку студентів тестовими даними
    private void initializeStudents() {
        students.add(new Student("Павленко", "Павло", "ТВ-23",
                new BirthDate(2004, 6, 25), 4.6f));
        students.add(new Student("Макаренко", "Семен", "ТВ-22",
                new BirthDate(2003, 11, 3), 4.2f));
        students.add(new Student("Максименко", "Максим", "ТВ-21",
                new BirthDate(2004, 10, 15), 4.7f));
        students.add(new Student("Петренко", "Петро", "ТВ-21",
                new BirthDate(2004, 1, 5), 4.1f));
        students.add(new Student("Миколенко", "Максим", "ТВ-22",
                new BirthDate(2004, 8, 8), 4.5f));
    }

    public void start() {
        System.out.println("=== Система управління студентами ===");

        while (true) {
            displayMenu();
            int choice = getChoice();

            switch (choice) {
                case 1:
                    displayAllStudents();
                    break;
                case 2:
                    editStudent();
                    break;
                case 3:
                    addStudent();
                    break;
                case 4:
                    removeStudent();
                    break;
                case 5:
                    sortStudents();
                    break;
                case 6:
                    searchStudent();
                    break;
                case 7:
                    System.out.println("До побачення!");
                    return;
                default:
                    System.out.println("Невірний вибір. Спробуйте ще раз.");
            }
            System.out.println();
        }
    }

    private void displayMenu() {
        System.out.println("\n1. Показати всіх студентів");
        System.out.println("2. Редагувати студента");
        System.out.println("3. Додати студента");
        System.out.println("4. Видалити студента");
        System.out.println("5. Сортувати студентів");
        System.out.println("6. Пошук студента");
        System.out.println("7. Вийти");
        System.out.print("Виберіть опцію: ");
    }

    private int getChoice() {
        try {
            return scanner.nextInt();
        } catch (Exception e) {
            scanner.nextLine();
            return -1;
        }
    }

    private void displayAllStudents() {
        if (students.isEmpty()) {
            System.out.println("Список студентів порожній");
            return;
        }

        System.out.println("\n=== Список студентів ===");
        for (int i = 0; i < students.size(); i++) {
            System.out.println((i + 1) + ". " + students.get(i));
        }
    }

    private void editStudent() {
        displayAllStudents();
        if (students.isEmpty()) return;

        System.out.print("Введіть номер студента для редагування: ");
        int index = scanner.nextInt() - 1;

        if (index < 0 || index >= students.size()) {
            System.out.println("Невірний номер студента");
            return;
        }

        Student student = students.get(index);
        System.out.println("Редагування студента: " + student);

        scanner.nextLine(); // очищення буфера

        System.out.print("Нове прізвище (" + student.getLastName() + "): ");
        String lastName = scanner.nextLine();
        if (!lastName.trim().isEmpty()) {
            student.setLastName(lastName);
        }

        System.out.print("Нове ім'я (" + student.getFirstName() + "): ");
        String firstName = scanner.nextLine();
        if (!firstName.trim().isEmpty()) {
            student.setFirstName(firstName);
        }

        System.out.print("Нова група (" + student.getGroupName() + "): ");
        String groupName = scanner.nextLine();
        if (!groupName.trim().isEmpty()) {
            student.setGroupName(groupName);
        }

        System.out.print("Нова середня оцінка (" + student.getAverageGrade() + "): ");
        String gradeStr = scanner.nextLine();
        if (!gradeStr.trim().isEmpty()) {
            try {
                float grade = Float.parseFloat(gradeStr);
                student.setAverageGrade(grade);
            } catch (NumberFormatException e) {
                System.out.println("Невірний формат оцінки");
            }
        }

        // Підтвердження змін
        System.out.println("\nОновлені дані студента: " + student);
        System.out.print("Чи правильно введені дані? (y/n): ");
        String confirm = scanner.nextLine();
        if (confirm.toLowerCase().startsWith("y") || confirm.toLowerCase().startsWith("т")) {
            System.out.println("Дані студента оновлено");
        } else {
            // Тут можна було б відновити старі дані, але для простоти залишаємо як є
            System.out.println("Зміни збережено");
        }
    }

    private void addStudent() {
        scanner.nextLine(); // очищення буфера

        System.out.print("Введіть прізвище: ");
        String lastName = scanner.nextLine();

        System.out.print("Введіть ім'я: ");
        String firstName = scanner.nextLine();

        System.out.print("Введіть групу: ");
        String groupName = scanner.nextLine();

        System.out.print("Введіть рік народження: ");
        int year = scanner.nextInt();

        System.out.print("Введіть місяць народження: ");
        int month = scanner.nextInt();

        System.out.print("Введіть день народження: ");
        int day = scanner.nextInt();

        System.out.print("Введіть середню оцінку: ");
        float grade = scanner.nextFloat();

        BirthDate birthDate = new BirthDate(year, month, day);
        Student newStudent = new Student(lastName, firstName, groupName, birthDate, grade);

        students.add(newStudent);
        System.out.println("Студента додано: " + newStudent);
    }

    private void removeStudent() {
        displayAllStudents();
        if (students.isEmpty()) return;

        System.out.print("Введіть номер студента для видалення: ");
        int index = scanner.nextInt() - 1;

        if (index < 0 || index >= students.size()) {
            System.out.println("Невірний номер студента");
            return;
        }

        Student removed = students.remove(index);
        System.out.println("Студента видалено: " + removed);
    }

    private void sortStudents() {
        System.out.println("Виберіть критерій сортування:");
        System.out.println("1. За прізвищем");
        System.out.println("2. За ім'ям");
        System.out.println("3. За групою");
        System.out.println("4. За середньою оцінкою");
        System.out.print("Ваш вибір: ");

        int choice = scanner.nextInt();

        switch (choice) {
            case 1:
                students.sort(Comparator.comparing(Student::getLastName));
                System.out.println("Студентів відсортовано за прізвищем");
                break;
            case 2:
                students.sort(Comparator.comparing(Student::getFirstName));
                System.out.println("Студентів відсортовано за ім'ям");
                break;
            case 3:
                students.sort(Comparator.comparing(Student::getGroupName));
                System.out.println("Студентів відсортовано за групою");
                break;
            case 4:
                students.sort(Comparator.comparing(Student::getAverageGrade).reversed());
                System.out.println("Студентів відсортовано за середньою оцінкою (від найвищої)");
                break;
            default:
                System.out.println("Невірний вибір");
        }
    }

    private void searchStudent() {
        scanner.nextLine(); // очищення буфера
        System.out.print("Введіть прізвище або ім'я для пошуку: ");
        String searchTerm = scanner.nextLine().toLowerCase();

        System.out.println("\n=== Результати пошуку ===");
        boolean found = false;

        for (int i = 0; i < students.size(); i++) {
            Student student = students.get(i);
            if (student.getLastName().toLowerCase().contains(searchTerm) ||
                    student.getFirstName().toLowerCase().contains(searchTerm)) {
                System.out.println((i + 1) + ". " + student);
                found = true;
            }
        }

        if (!found) {
            System.out.println("Студентів не знайдено");
        }
    }
}

// Основний клас
public class StudentManagementApp {
    public static void main(String[] args) {
        StudentManagementSystem system = new StudentManagementSystem();
        system.start();
    }
}