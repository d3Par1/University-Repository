import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

public class Lr2_2 {
    enum HairColor {
        BLONDE,
        BRUNETTE,
        REDHEAD
    }

    static class Person {
        private String name;
        private HairColor hairColor;

        public Person(String name, HairColor hairColor) {
            this.name = name;
            this.hairColor = hairColor;
        }

        public String getName() {
            return name;
        }

        public HairColor getHairColor() {
            return hairColor;
        }

        @Override
        public String toString() {
            return name + " (" + hairColor + ")";
        }
    }

    public static void main(String[] args) {
        List<Person> people = createPeopleList();

        Scanner scanner = new Scanner(System.in);

        System.out.println("Виберіть колір волосся для пошуку знайомих:");
        System.out.println("1 - BLONDE (блондин/ка)");
        System.out.println("2 - BRUNETTE (брюнет/ка)");
        System.out.println("3 - REDHEAD (руда/ий)");
        System.out.print("Ваш вибір (1-3): ");

        int choice = scanner.nextInt();
        HairColor selectedColor = null;

        // Конвертація вибору користувача в enum
        switch (choice) {
            case 1:
                selectedColor = HairColor.BLONDE;
                break;
            case 2:
                selectedColor = HairColor.BRUNETTE;
                break;
            case 3:
                selectedColor = HairColor.REDHEAD;
                break;
            default:
                System.out.println("Невірний вибір. Програма завершується.");
                scanner.close();
                return;
        }

        List<Person> matchingPeople = findPeopleByHairColor(people, selectedColor);

        System.out.println("\nЗнайомі з кольором волосся " + selectedColor + ":");

        if (matchingPeople.isEmpty()) {
            System.out.println("У вас немає знайомих з таким кольором волосся.");
        } else {
            for (Person person : matchingPeople) {
                System.out.println("- " + person.getName());
            }
            System.out.println("\nЗагальна кількість знайомих з кольором " +
                    selectedColor + ": " + matchingPeople.size());
        }

        scanner.close();
    }

    private static List<Person> createPeopleList() {
        List<Person> people = new ArrayList<>();

        // Додавання людей з різними кольорами волосся
        people.add(new Person("Олександр Петренко", HairColor.BLONDE));
        people.add(new Person("Марія Коваленко", HairColor.BRUNETTE));
        people.add(new Person("Іван Шевченко", HairColor.BRUNETTE));
        people.add(new Person("Анна Мельник", HairColor.BLONDE));
        people.add(new Person("Юлія Бондаренко", HairColor.REDHEAD));
        people.add(new Person("Михайло Ткаченко", HairColor.BRUNETTE));
        people.add(new Person("Наталія Шевчук", HairColor.BLONDE));
        people.add(new Person("Сергій Бойко", HairColor.REDHEAD));
        people.add(new Person("Олена Кравченко", HairColor.BRUNETTE));
        people.add(new Person("Дмитро Савченко", HairColor.BLONDE));

        return people;
    }

    private static List<Person> findPeopleByHairColor(List<Person> people, HairColor hairColor) {
        List<Person> result = new ArrayList<>();

        for (Person person : people) {
            if (person.getHairColor() == hairColor) {
                result.add(person);
            }
        }

        return result;
    }
}
