import java.util.*;

// Клас Animal (Тварина)
class Animal {
    private String name;
    private String species;
    private int age;

    public Animal(String name, String species, int age) {
        this.name = name;
        this.species = species;
        this.age = age;
    }

    // Гетери
    public String getName() { return name; }
    public String getSpecies() { return species; }
    public int getAge() { return age; }

    // Сетери
    public void setName(String name) { this.name = name; }
    public void setSpecies(String species) { this.species = species; }
    public void setAge(int age) { this.age = age; }

    @Override
    public String toString() {
        return String.format("Тварина{ім'я='%s', вид='%s', вік=%d}", name, species, age);
    }
}

// Клас Zoo (Зоопарк)
class Zoo {
    private Deque<Animal> animalDeque;

    public Zoo() {
        this.animalDeque = new ArrayDeque<>();
    }

    // Додавання тварини вперед
    public void addAnimalToFront(Animal animal) {
        animalDeque.addFirst(animal);
        System.out.println("Тварину " + animal.getName() + " додано на початок черги");
    }

    // Додавання тварини назад
    public void addAnimalToBack(Animal animal) {
        animalDeque.addLast(animal);
        System.out.println("Тварину " + animal.getName() + " додано в кінець черги");
    }

    // Видалення тварини спереду
    public Animal removeAnimalFromFront() {
        if (animalDeque.isEmpty()) {
            System.out.println("Черга тварин порожня!");
            return null;
        }
        Animal removed = animalDeque.removeFirst();
        System.out.println("Тварину " + removed.getName() + " видалено з початку черги");
        return removed;
    }

    // Видалення тварини ззаду
    public Animal removeAnimalFromBack() {
        if (animalDeque.isEmpty()) {
            System.out.println("Черга тварин порожня!");
            return null;
        }
        Animal removed = animalDeque.removeLast();
        System.out.println("Тварину " + removed.getName() + " видалено з кінця черги");
        return removed;
    }

    public Deque<Animal> getAnimalDeque() {
        return animalDeque;
    }

    public void displayAnimals() {
        if (animalDeque.isEmpty()) {
            System.out.println("В зоопарку немає тварин");
            return;
        }
        System.out.println("Тварини в зоопарку:");
        int i = 1;
        for (Animal animal : animalDeque) {
            System.out.println(i++ + ". " + animal);
        }
    }
}

// Клас AnimalCare (Догляд за тваринами)
class AnimalCare {

    // Годування всіх тварин
    public void feedAllAnimals(Zoo zoo) {
        if (zoo.getAnimalDeque().isEmpty()) {
            System.out.println("Немає тварин для годування");
            return;
        }

        System.out.println("Годування всіх тварин:");
        for (Animal animal : zoo.getAnimalDeque()) {
            System.out.println("Годування " + animal.getName() + " (" + animal.getSpecies() + ")");
        }
        System.out.println("Всі тварини нагодовані!");
    }

    // Переселення тварини на вказану кількість позицій
    public void moveAnimal(Zoo zoo, int positions) {
        Deque<Animal> deque = zoo.getAnimalDeque();
        if (deque.isEmpty()) {
            System.out.println("Немає тварин для переселення");
            return;
        }

        if (Math.abs(positions) >= deque.size()) {
            System.out.println("Кількість позицій перевищує розмір черги!");
            return;
        }

        if (positions > 0) {
            // Переміщення вперед
            for (int i = 0; i < positions; i++) {
                Animal animal = deque.removeFirst();
                deque.addLast(animal);
            }
            System.out.println("Тварини переміщені на " + positions + " позицій вперед");
        } else if (positions < 0) {
            // Переміщення назад
            for (int i = 0; i < Math.abs(positions); i++) {
                Animal animal = deque.removeLast();
                deque.addFirst(animal);
            }
            System.out.println("Тварини переміщені на " + Math.abs(positions) + " позицій назад");
        }
    }
}

// Клас ZooKeeper (Працівник зоопарку)
class ZooKeeper {
    private Zoo zoo;
    private AnimalCare animalCare;
    private Scanner scanner;

    public ZooKeeper() {
        this.zoo = new Zoo();
        this.animalCare = new AnimalCare();
        this.scanner = new Scanner(System.in);
    }

    public void start() {
        System.out.println("=== Система управління зоопарком ===");

        while (true) {
            displayMenu();
            int choice = getChoice();

            switch (choice) {
                case 1:
                    addAnimal();
                    break;
                case 2:
                    removeAnimal();
                    break;
                case 3:
                    animalCare.feedAllAnimals(zoo);
                    break;
                case 4:
                    moveAnimals();
                    break;
                case 5:
                    zoo.displayAnimals();
                    break;
                case 6:
                    System.out.println("До побачення!");
                    return;
                default:
                    System.out.println("Невірний вибір. Спробуйте ще раз.");
            }
            System.out.println();
        }
    }

    private void displayMenu() {
        System.out.println("1. Додати тварину");
        System.out.println("2. Видалити тварину");
        System.out.println("3. Нагодувати всіх тварин");
        System.out.println("4. Переселити тварин");
        System.out.println("5. Показати всіх тварин");
        System.out.println("6. Вийти");
        System.out.print("Виберіть опцію: ");
    }

    private int getChoice() {
        try {
            return scanner.nextInt();
        } catch (Exception e) {
            scanner.nextLine(); // очищення буфера
            return -1;
        }
    }

    private void addAnimal() {
        scanner.nextLine(); // очищення буфера
        System.out.print("Введіть ім'я тварини: ");
        String name = scanner.nextLine();
        System.out.print("Введіть вид тварини: ");
        String species = scanner.nextLine();
        System.out.print("Введіть вік тварини: ");
        int age = scanner.nextInt();

        Animal animal = new Animal(name, species, age);

        System.out.println("Куди додати тварину?");
        System.out.println("1. На початок черги");
        System.out.println("2. В кінець черги");
        System.out.print("Виберіть: ");
        int position = scanner.nextInt();

        if (position == 1) {
            zoo.addAnimalToFront(animal);
        } else if (position == 2) {
            zoo.addAnimalToBack(animal);
        } else {
            System.out.println("Невірний вибір. Тварину додано в кінець черги.");
            zoo.addAnimalToBack(animal);
        }
    }

    private void removeAnimal() {
        if (zoo.getAnimalDeque().isEmpty()) {
            System.out.println("Немає тварин для видалення");
            return;
        }

        System.out.println("Звідки видалити тварину?");
        System.out.println("1. З початку черги");
        System.out.println("2. З кінця черги");
        System.out.print("Виберіть: ");
        int position = scanner.nextInt();

        if (position == 1) {
            zoo.removeAnimalFromFront();
        } else if (position == 2) {
            zoo.removeAnimalFromBack();
        } else {
            System.out.println("Невірний вибір.");
        }
    }

    private void moveAnimals() {
        if (zoo.getAnimalDeque().isEmpty()) {
            System.out.println("Немає тварин для переселення");
            return;
        }

        System.out.print("Введіть кількість позицій для переселення (+ вперед, - назад): ");
        int positions = scanner.nextInt();
        animalCare.moveAnimal(zoo, positions);
    }
}

// Основний клас
public class ZooManagementSystem {
    public static void main(String[] args) {
        // Створення об'єкта ZooKeeper і тестування системи
        ZooKeeper zooKeeper = new ZooKeeper();

        // Додавання тестових тварин
        Zoo testZoo = new Zoo();
        testZoo.addAnimalToBack(new Animal("Лев", "Panthera leo", 5));
        testZoo.addAnimalToBack(new Animal("Слон", "Elephas maximus", 12));
        testZoo.addAnimalToFront(new Animal("Тигр", "Panthera tigris", 7));

        System.out.println("=== Тестування системи ===");
        testZoo.displayAnimals();

        AnimalCare care = new AnimalCare();
        care.feedAllAnimals(testZoo);
        care.moveAnimal(testZoo, 1);
        testZoo.displayAnimals();

        System.out.println("\n=== Інтерактивний режим ===");
        zooKeeper.start();
    }
}