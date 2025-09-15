// Лабораторна робота 1 - Завдання 2
// Система класів з наслідуванням: Птах -> Курка, Зозуля; Квочка
// Автор: Студент групи ТВ-43, прізвище Step
// Варіант 16

import java.util.Scanner;
import java.util.ArrayList;
import java.util.List;

// Базовий абстрактний клас Птах
abstract class Bird {
    protected String name;
    protected String color;
    protected double weight;
    protected boolean canFly;

    public Bird(String name, String color, double weight, boolean canFly) {
        this.name = name;
        this.color = color;
        this.weight = weight;
        this.canFly = canFly;
    }

    // Геттери та сеттери
    public String getName() { return name; }
    public String getColor() { return color; }
    public double getWeight() { return weight; }
    public boolean getCanFly() { return canFly; }

    public void setName(String name) { this.name = name; }
    public void setColor(String color) { this.color = color; }
    public void setWeight(double weight) { this.weight = weight; }
    public void setCanFly(boolean canFly) { this.canFly = canFly; }

    // Абстрактні методи
    public abstract void fly();
    public abstract void sing();

    // Спільні методи
    public void eat() {
        System.out.println(name + " їсть корм");
    }

    public void sleep() {
        System.out.println(name + " спить");
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Bird bird = (Bird) obj;
        return name.equals(bird.name) && color.equals(bird.color) &&
               Math.abs(weight - bird.weight) < 0.01 && canFly == bird.canFly;
    }

    @Override
    public int hashCode() {
        return name.hashCode() + color.hashCode() + Double.hashCode(weight) + Boolean.hashCode(canFly);
    }

    @Override
    public String toString() {
        return String.format("%s (колір: %s, вага: %.1f кг, літає: %s)",
            name, color, weight, canFly ? "так" : "ні");
    }
}

// Клас Курка
class Chicken extends Bird {
    private int eggsLaid;
    private boolean isBrooding;

    public Chicken(String name, String color, double weight) {
        super(name, color, weight, false);
        this.eggsLaid = 0;
        this.isBrooding = false;
    }

    public int getEggsLaid() { return eggsLaid; }
    public boolean getIsBrooding() { return isBrooding; }
    public void setIsBrooding(boolean isBrooding) { this.isBrooding = isBrooding; }

    @Override
    public void fly() {
        System.out.println(name + " не може літати, але може підлітати на короткі відстані");
    }

    @Override
    public void sing() {
        if (isBrooding) {
            System.out.println(name + " тихо кудкудакає, висиджуючи яйця");
        } else {
            System.out.println(name + " голосно кудкудакає: Кудах-кудах!");
        }
    }

    public void layEgg() {
        if (!isBrooding) {
            eggsLaid++;
            System.out.println(name + " знесла яйце! Всього яєць: " + eggsLaid);
        } else {
            System.out.println(name + " зараз висиджує яйця і не може нести нові");
        }
    }

    public void startBrooding() {
        if (eggsLaid > 0) {
            isBrooding = true;
            System.out.println(name + " почала висиджувати " + eggsLaid + " яйце(ць)");
        } else {
            System.out.println(name + " не має яєць для висиджування");
        }
    }

    public List<String> hatchChicks() {
        List<String> chicks = new ArrayList<>();
        if (isBrooding && eggsLaid > 0) {
            for (int i = 1; i <= eggsLaid; i++) {
                chicks.add("Пташеня " + i);
            }
            System.out.println(name + " вивела " + chicks.size() + " пташенят!");
            eggsLaid = 0;
            isBrooding = false;
        } else {
            System.out.println(name + " не висиджує яйця");
        }
        return chicks;
    }

    @Override
    public String toString() {
        return super.toString() + String.format(" [яєць знесено: %d, висиджує: %s]",
            eggsLaid, isBrooding ? "так" : "ні");
    }
}

// Клас Зозуля
class Cuckoo extends Bird {
    private boolean isMigratory;
    private String currentLocation;

    public Cuckoo(String name, String color, double weight, boolean isMigratory) {
        super(name, color, weight, true);
        this.isMigratory = isMigratory;
        this.currentLocation = "рідний ліс";
    }

    public boolean getIsMigratory() { return isMigratory; }
    public String getCurrentLocation() { return currentLocation; }
    public void setCurrentLocation(String location) { this.currentLocation = location; }

    @Override
    public void fly() {
        if (isMigratory) {
            System.out.println(name + " літає на великі відстані під час міграції");
        } else {
            System.out.println(name + " літає в межах свого лісу");
        }
    }

    @Override
    public void sing() {
        System.out.println(name + " співає: Ку-ку! Ку-ку!");
    }

    public void migrate(String destination) {
        if (isMigratory) {
            currentLocation = destination;
            System.out.println(name + " мігрувала до: " + destination);
        } else {
            System.out.println(name + " не є мігруючою птахою");
        }
    }

    public void layEggInNest(String hostBirdName) {
        System.out.println(name + " підклала яйце в гніздо птаха " + hostBirdName);
    }

    @Override
    public String toString() {
        return super.toString() + String.format(" [мігруюча: %s, знаходиться: %s]",
            isMigratory ? "так" : "ні", currentLocation);
    }
}

// Клас Квочка (використовує композицію)
class BroodHen {
    private Chicken chicken;
    private List<String> chicks;
    private boolean isProtecting;

    public BroodHen(Chicken chicken) {
        this.chicken = chicken;
        this.chicks = new ArrayList<>();
        this.isProtecting = false;
    }

    public Chicken getChicken() { return chicken; }
    public List<String> getChicks() { return chicks; }
    public boolean getIsProtecting() { return isProtecting; }

    public void fly() {
        System.out.println("Квочка " + chicken.getName() + ":");
        chicken.fly();
    }

    public void sing() {
        System.out.println("Квочка " + chicken.getName() + ":");
        if (isProtecting && !chicks.isEmpty()) {
            System.out.println("Квочка тривожно кудкудакає, захищаючи пташенят");
        } else {
            chicken.sing();
        }
    }

    public void layEgg() {
        System.out.println("Квочка " + chicken.getName() + ":");
        chicken.layEgg();
    }

    public void broodEggs() {
        System.out.println("Квочка " + chicken.getName() + ":");
        chicken.startBrooding();
        List<String> newChicks = chicken.hatchChicks();
        chicks.addAll(newChicks);
        if (!newChicks.isEmpty()) {
            isProtecting = true;
            System.out.println("Квочка тепер захищає своїх пташенят");
        }
    }

    public void protectChicks() {
        if (!chicks.isEmpty()) {
            isProtecting = true;
            System.out.println("Квочка " + chicken.getName() + " захищає " +
                chicks.size() + " пташенят від небезпеки");
        } else {
            System.out.println("У квочки " + chicken.getName() + " немає пташенят для захисту");
        }
    }

    public void feedChicks() {
        if (!chicks.isEmpty()) {
            System.out.println("Квочка " + chicken.getName() + " годує " +
                chicks.size() + " пташенят");
        } else {
            System.out.println("У квочки " + chicken.getName() + " немає пташенят для годування");
        }
    }

    public void teachChicks() {
        if (!chicks.isEmpty()) {
            System.out.println("Квочка " + chicken.getName() + " навчає пташенят:");
            for (String chick : chicks) {
                System.out.println("  - " + chick + " вчиться клювати зерно");
            }
        } else {
            System.out.println("У квочки " + chicken.getName() + " немає пташенят для навчання");
        }
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        BroodHen other = (BroodHen) obj;
        return chicken.equals(other.chicken) && chicks.equals(other.chicks);
    }

    @Override
    public int hashCode() {
        return chicken.hashCode() + chicks.hashCode();
    }

    @Override
    public String toString() {
        return String.format("Квочка %s [пташенят: %d, захищає: %s]",
            chicken.toString(), chicks.size(), isProtecting ? "так" : "ні");
    }
}

public class Lr1_2 {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("=== Лабораторна робота 1 - Завдання 2 ===");
        System.out.println("Система класів: Птах, Курка, Зозуля, Квочка");
        System.out.println("Студент групи ТВ-43, прізвище Step, варіант 16\n");

        // Створення об'єктів
        Chicken chicken1 = new Chicken("Квочка Клуша", "біла", 2.1);
        Chicken chicken2 = new Chicken("Курка Пеструшка", "строката", 1.8);
        Cuckoo cuckoo1 = new Cuckoo("Зозуля Кукушка", "сіра", 0.8, true);
        Cuckoo cuckoo2 = new Cuckoo("Зозуля Лісова", "коричнева", 0.9, false);

        // Створення квочки
        BroodHen broodHen = new BroodHen(chicken1);

        System.out.println("=== Демонстрація роботи з курками ===");
        System.out.println(chicken1);
        System.out.println(chicken2);

        // Курки несуть яйця
        for (int i = 0; i < 3; i++) {
            chicken1.layEgg();
        }

        for (int i = 0; i < 2; i++) {
            chicken2.layEgg();
        }

        System.out.println("\nСпівають:");
        chicken1.sing();
        chicken2.sing();

        System.out.println("\nПробують літати:");
        chicken1.fly();
        chicken2.fly();

        System.out.println("\n=== Демонстрація роботи з зозулями ===");
        System.out.println(cuckoo1);
        System.out.println(cuckoo2);

        System.out.println("\nСпівають:");
        cuckoo1.sing();
        cuckoo2.sing();

        System.out.println("\nЛітають:");
        cuckoo1.fly();
        cuckoo2.fly();

        System.out.println("\nМіграція:");
        cuckoo1.migrate("Африка");
        cuckoo2.migrate("Південна Америка");

        System.out.println("\nПідкидання яєць:");
        cuckoo1.layEggInNest("дрозда");
        cuckoo2.layEggInNest("вівчарика");

        System.out.println("\n=== Демонстрація роботи квочки ===");
        System.out.println(broodHen);

        System.out.println("\nКвочка співає:");
        broodHen.sing();

        System.out.println("\nКвочка несе яйця:");
        broodHen.layEgg();
        broodHen.layEgg();

        System.out.println("\nКвочка висиджує яйця та виводить пташенят:");
        broodHen.broodEggs();

        System.out.println("\nСтан квочки після виведення пташенят:");
        System.out.println(broodHen);

        System.out.println("\nКвочка співає після появи пташенят:");
        broodHen.sing();

        System.out.println("\nКвочка дбає про пташенят:");
        broodHen.protectChicks();
        broodHen.feedChicks();
        broodHen.teachChicks();

        System.out.println("\n=== Демонстрація поліморфізму ===");
        Bird[] birds = {chicken2, cuckoo1, cuckoo2};

        System.out.println("Всі птахи співають:");
        for (Bird bird : birds) {
            bird.sing();
        }

        System.out.println("\nВсі птахи літають:");
        for (Bird bird : birds) {
            bird.fly();
        }

        System.out.println("\n=== Перевірка equals та hashCode ===");
        Chicken chicken3 = new Chicken("Квочка Клуша", "біла", 2.1);
        System.out.println("chicken1.equals(chicken3): " + chicken1.equals(chicken3));
        System.out.println("chicken1.hashCode(): " + chicken1.hashCode());
        System.out.println("chicken3.hashCode(): " + chicken3.hashCode());

        scanner.close();
    }
}