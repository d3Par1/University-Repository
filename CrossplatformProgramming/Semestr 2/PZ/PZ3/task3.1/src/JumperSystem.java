import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Random;

// Клас для представлення студента-спортсмена зі стрибків у висоту
class Jumper {
    private String lastName;
    private String firstName;
    private double personalBest; // Найкращий результат у метрах
    private double competitionResult; // Результат на змаганні

    // Конструктор з усіма параметрами
    public Jumper(String lastName, String firstName, double personalBest) {
        this.lastName = lastName;
        this.firstName = firstName;
        this.personalBest = personalBest;
        this.competitionResult = 0.0;
    }

    // Конструктор з прізвищем та ім'ям
    public Jumper(String lastName, String firstName) {
        this(lastName, firstName, 0.0);
    }

    // Гетери та сетери
    public String getLastName() {
        return lastName;
    }

    public void setLastName(String lastName) {
        this.lastName = lastName;
    }

    public String getFirstName() {
        return firstName;
    }

    public void setFirstName(String firstName) {
        this.firstName = firstName;
    }

    public double getPersonalBest() {
        return personalBest;
    }

    public void setPersonalBest(double personalBest) {
        this.personalBest = personalBest;
    }

    public double getCompetitionResult() {
        return competitionResult;
    }

    public void setCompetitionResult(double competitionResult) {
        this.competitionResult = competitionResult;
    }

    // Метод для участі у змаганні
    public void compete() {
        Random random = new Random();
        double randomFactor = random.nextDouble(); // Випадкове число від 0 до 1

        // Розрахунок результату змагання за формулою: d = 0.8 * dmax + 0.3 * dmax * rnd
        competitionResult = 0.8 * personalBest + 0.3 * personalBest * randomFactor;

        // Округлення до 2 знаків після коми для читабельності
        competitionResult = Math.round(competitionResult * 100.0) / 100.0;

        // Якщо результат кращий за персональний рекорд, оновлюємо рекорд
        if (competitionResult > personalBest) {
            personalBest = competitionResult;
        }
    }

    // Перевизначення toString() для зручного виведення інформації про стрибуна
    @Override
    public String toString() {
        return lastName + " " + firstName + ", особистий рекорд: " + personalBest + " м";
    }
}

// Клас для виведення списків стрибунів
class JumperListPrinter {
    // Метод для виведення списку
    public static void printList(List<Jumper> jumpers, String title) {
        System.out.println("\n" + title);
        System.out.println("------------------------------------------------");
        for (int i = 0; i < jumpers.size(); i++) {
            Jumper jumper = jumpers.get(i);
            System.out.println((i + 1) + ". " + jumper.getLastName() + " " + jumper.getFirstName() +
                    ", особистий рекорд: " + jumper.getPersonalBest() + " м");
        }
        System.out.println("------------------------------------------------");
    }

    // Метод для виведення результатів змагань
    public static void printCompetitionResults(List<Jumper> jumpers) {
        System.out.println("\nРезультати змагань зі стрибків у висоту:");
        System.out.println("------------------------------------------------");

        // Створюємо копію списку для сортування за результатом змагань
        List<Jumper> sortedByResult = new ArrayList<>(jumpers);
        Collections.sort(sortedByResult, new Comparator<Jumper>() {
            @Override
            public int compare(Jumper j1, Jumper j2) {
                return Double.compare(j2.getCompetitionResult(), j1.getCompetitionResult()); // Сортування за спаданням
            }
        });

        for (int i = 0; i < sortedByResult.size(); i++) {
            Jumper jumper = sortedByResult.get(i);
            System.out.println((i + 1) + ". " + jumper.getLastName() + " " + jumper.getFirstName() +
                    ", результат: " + jumper.getCompetitionResult() + " м");
        }
        System.out.println("------------------------------------------------");
    }
}

public class JumperSystem {
    public static void main(String[] args) {
        // Створення списку найкращих стрибунів
        List<Jumper> topJumpers = new ArrayList<>();

        // Додавання 5 стрибунів
        topJumpers.add(new Jumper("Бондаренко", "Богдан", 2.25));
        topJumpers.add(new Jumper("Ковальчук", "Олексій", 2.35));
        topJumpers.add(new Jumper("Шевченко", "Іван", 2.20));
        topJumpers.add(new Jumper("Мельник", "Андрій", 2.30));
        topJumpers.add(new Jumper("Петренко", "Василь", 2.15));

        // Виведення початкового списку
        JumperListPrinter.printList(topJumpers, "Початковий список стрибунів:");

        // Сортування за алфавітом (за прізвищем, потім за ім'ям)
        Collections.sort(topJumpers, new Comparator<Jumper>() {
            @Override
            public int compare(Jumper j1, Jumper j2) {
                int lastNameComparison = j1.getLastName().compareTo(j2.getLastName());
                if (lastNameComparison != 0) {
                    return lastNameComparison;
                }
                return j1.getFirstName().compareTo(j2.getFirstName());
            }
        });

        // Виведення списку, відсортованого за алфавітом
        JumperListPrinter.printList(topJumpers, "Список стрибунів, відсортований за алфавітом:");

        // Сортування за персональним рекордом (у порядку спадання)
        Collections.sort(topJumpers, new Comparator<Jumper>() {
            @Override
            public int compare(Jumper j1, Jumper j2) {
                return Double.compare(j2.getPersonalBest(), j1.getPersonalBest()); // Сортування за спаданням
            }
        });

        // Виведення списку, відсортованого за персональним рекордом
        JumperListPrinter.printList(topJumpers, "Список стрибунів, відсортований за персональним рекордом:");

        // Проведення змагань
        System.out.println("\nПроведення змагань зі стрибків у висоту...");
        for (Jumper jumper : topJumpers) {
            jumper.compete();
        }

        // Виведення результатів змагань
        JumperListPrinter.printCompetitionResults(topJumpers);

        // Сортування за оновленим персональним рекордом
        Collections.sort(topJumpers, new Comparator<Jumper>() {
            @Override
            public int compare(Jumper j1, Jumper j2) {
                return Double.compare(j2.getPersonalBest(), j1.getPersonalBest()); // Сортування за спаданням
            }
        });

        // Виведення оновленого списку з персональними рекордами
        JumperListPrinter.printList(topJumpers, "Оновлений список стрибунів з персональними рекордами:");
    }
}