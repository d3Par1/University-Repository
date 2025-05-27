import java.time.DayOfWeek;
import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.Scanner;

public class DayOfWeekChecker {

    // Створення enum для днів тижня
    enum Day {
        MONDAY(false),
        TUESDAY(false),
        WEDNESDAY(false),
        THURSDAY(false),
        FRIDAY(false),
        SATURDAY(true),
        SUNDAY(true);

        private final boolean isWeekend;

        // Конструктор для enum
        Day(boolean isWeekend) {
            this.isWeekend = isWeekend;
        }

        // Метод для перевірки, чи є день вихідним
        public boolean isWeekend() {
            return isWeekend;
        }

        // Метод для отримання дня тижня з Java DayOfWeek
        public static Day fromDayOfWeek(DayOfWeek dayOfWeek) {
            return Day.values()[dayOfWeek.getValue() - 1];
        }

        // Метод для отримання українського представлення дня тижня
        public String getUkrainianName() {
            switch (this) {
                case MONDAY: return "Понеділок";
                case TUESDAY: return "Вівторок";
                case WEDNESDAY: return "Середа";
                case THURSDAY: return "Четвер";
                case FRIDAY: return "П'ятниця";
                case SATURDAY: return "Субота";
                case SUNDAY: return "Неділя";
                default: return "Невідомий день";
            }
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("Перевірка дня тижня");
        System.out.println("1. Використати поточну дату");
        System.out.println("2. Ввести свою дату");
        System.out.print("Виберіть опцію (1 або 2): ");

        int option = scanner.nextInt();
        scanner.nextLine(); // Очистка буфера після nextInt()

        LocalDate date;

        if (option == 1) {
            // Отримання поточної дати
            date = LocalDate.now();
            System.out.println("Використовується поточна дата: " +
                    date.format(DateTimeFormatter.ofPattern("dd.MM.yyyy")));
        } else if (option == 2) {
            // Введення дати користувачем
            System.out.print("Введіть дату у форматі дд.мм.рррр: ");
            String dateStr = scanner.nextLine();

            // Парсинг введеної дати
            try {
                DateTimeFormatter formatter = DateTimeFormatter.ofPattern("dd.MM.yyyy");
                date = LocalDate.parse(dateStr, formatter);
            } catch (Exception e) {
                System.out.println("Помилка форматування дати. Використовується поточна дата.");
                date = LocalDate.now();
            }
        } else {
            System.out.println("Невірна опція. Використовується поточна дата.");
            date = LocalDate.now();
        }

        // Отримання дня тижня
        DayOfWeek dayOfWeek = date.getDayOfWeek();
        Day day = Day.fromDayOfWeek(dayOfWeek);

        // Виведення результатів
        System.out.println("\nРезультати для дати " + date.format(DateTimeFormatter.ofPattern("dd.MM.yyyy")) + ":");
        System.out.println("День тижня: " + day.getUkrainianName());
        System.out.println("Цей день " + (day.isWeekend() ? "вихідний" : "будній"));

        // Демонстрація всіх днів тижня
        System.out.println("\nВсі дні тижня та їх статус:");
        for (Day d : Day.values()) {
            System.out.println(d.getUkrainianName() + " (" + d.name() + ") - " +
                    (d.isWeekend() ? "вихідний" : "будній"));
        }

        scanner.close();
    }
}