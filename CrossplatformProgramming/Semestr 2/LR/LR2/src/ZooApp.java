import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

class Animal {
    private String name;
    private String species;
    private String habitat;

    public Animal(String name, String species, String habitat) {
        this.name = name;
        this.species = species;
        this.habitat = habitat;
    }

    public String getName() {
        return name;
    }

    public String getSpecies() {
        return species;
    }

    public String getHabitat() {
        return habitat;
    }

    @Override
    public String toString() {
        return "Тварина: " + name + ", Вид: " + species + ", Місце існування: " + habitat;
    }
}

class Zoo {
    private List<Animal> animals;

    public Zoo() {
        animals = new ArrayList<>();
    }

    public void addAnimal(Animal animal) {
        animals.add(animal);
        System.out.println("Тварину успішно додано до зоопарку.");
    }

    public void removeAnimal(String name) {
        boolean removed = false;

        for (int i = 0; i < animals.size(); i++) {
            if (animals.get(i).getName().equalsIgnoreCase(name)) {
                animals.remove(i);
                removed = true;
                System.out.println("Тварину \"" + name + "\" успішно видалено.");
                break;
            }
        }

        if (!removed) {
            System.out.println("Тварину \"" + name + "\" не знайдено в зоопарку.");
        }
    }

    public List<Animal> findAnimalsByName(String name) {
        List<Animal> found = new ArrayList<>();

        for (Animal animal : animals) {
            if (animal.getName().toLowerCase().contains(name.toLowerCase())) {
                found.add(animal);
            }
        }

        return found;
    }

    public List<Animal> findAnimalsBySpecies(String species) {
        List<Animal> found = new ArrayList<>();

        for (Animal animal : animals) {
            if (animal.getSpecies().toLowerCase().contains(species.toLowerCase())) {
                found.add(animal);
            }
        }

        return found;
    }

    public List<Animal> findAnimalsByHabitat(String habitat) {
        List<Animal> found = new ArrayList<>();

        for (Animal animal : animals) {
            if (animal.getHabitat().toLowerCase().contains(habitat.toLowerCase())) {
                found.add(animal);
            }
        }

        return found;
    }

    public void displayAllAnimals() {
        if (animals.isEmpty()) {
            System.out.println("У зоопарку немає тварин.");
        } else {
            System.out.println("Список усіх тварин у зоопарку:");
            for (int i = 0; i < animals.size(); i++) {
                System.out.println((i + 1) + ". " + animals.get(i));
            }
        }
    }
}

public class ZooApp {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Zoo zoo = new Zoo();

        zoo.addAnimal(new Animal("Лев Сімба", "Лев", "Савана"));
        zoo.addAnimal(new Animal("Тигр Шерхан", "Тигр", "Джунглі"));
        zoo.addAnimal(new Animal("Слон Дамбо", "Слон", "Савана"));
        zoo.addAnimal(new Animal("Жираф Мелман", "Жираф", "Савана"));
        zoo.addAnimal(new Animal("Пінгвін Шкіпер", "Пінгвін", "Антарктида"));

        int choice;
        do {
            System.out.println("\n==== Меню зоопарку ====");
            System.out.println("1. Додати нову тварину");
            System.out.println("2. Видалити тварину");
            System.out.println("3. Знайти тварин за назвою");
            System.out.println("4. Знайти тварин за видом");
            System.out.println("5. Знайти тварин за місцем існування");
            System.out.println("6. Показати всіх тварин");
            System.out.println("0. Вихід");
            System.out.print("Виберіть опцію: ");

            choice = scanner.nextInt();
            scanner.nextLine();

            switch (choice) {
                case 1:
                    System.out.print("Введіть назву тварини: ");
                    String name = scanner.nextLine();
                    System.out.print("Введіть вид тварини: ");
                    String species = scanner.nextLine();
                    System.out.print("Введіть місце існування: ");
                    String habitat = scanner.nextLine();

                    zoo.addAnimal(new Animal(name, species, habitat));
                    break;

                case 2:
                    System.out.print("Введіть назву тварини для видалення: ");
                    String nameToRemove = scanner.nextLine();
                    zoo.removeAnimal(nameToRemove);
                    break;

                case 3:
                    System.out.print("Введіть назву тварини для пошуку: ");
                    String searchName = scanner.nextLine();
                    List<Animal> foundByName = zoo.findAnimalsByName(searchName);

                    if (foundByName.isEmpty()) {
                        System.out.println("Тварин з такою назвою не знайдено.");
                    } else {
                        System.out.println("Знайдені тварини:");
                        for (Animal animal : foundByName) {
                            System.out.println(animal);
                        }
                    }
                    break;

                case 4:
                    System.out.print("Введіть вид тварини для пошуку: ");
                    String searchSpecies = scanner.nextLine();
                    List<Animal> foundBySpecies = zoo.findAnimalsBySpecies(searchSpecies);

                    if (foundBySpecies.isEmpty()) {
                        System.out.println("Тварин цього виду не знайдено.");
                    } else {
                        System.out.println("Знайдені тварини:");
                        for (Animal animal : foundBySpecies) {
                            System.out.println(animal);
                        }
                    }
                    break;

                case 5:
                    System.out.print("Введіть місце існування для пошуку: ");
                    String searchHabitat = scanner.nextLine();
                    List<Animal> foundByHabitat = zoo.findAnimalsByHabitat(searchHabitat);

                    if (foundByHabitat.isEmpty()) {
                        System.out.println("Тварин з таким місцем існування не знайдено.");
                    } else {
                        System.out.println("Знайдені тварини:");
                        for (Animal animal : foundByHabitat) {
                            System.out.println(animal);
                        }
                    }
                    break;

                case 6:
                    zoo.displayAllAnimals();
                    break;

                case 0:
                    System.out.println("Дякуємо за використання програми зоопарку!");
                    break;

                default:
                    System.out.println("Невірний вибір. Спробуйте ще раз.");
            }

        } while (choice != 0);

        scanner.close();
    }
}