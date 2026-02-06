import java.io.*;
import java.util.*;

// Клас, що представляє гравця волейбольної команди
class VolleyballPlayer {
    private String lastName;
    private String firstName;
    private int age;
    private int height; // У сантиметрах

    // Конструктор
    public VolleyballPlayer(String lastName, String firstName, int age, int height) {
        this.lastName = lastName;
        this.firstName = firstName;
        this.age = age;
        this.height = height;
    }

    // Гетери і сетери
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

    public int getAge() {
        return age;
    }

    public void setAge(int age) {
        this.age = age;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    // Перевизначення toString() для зручного виведення інформації про гравця
    @Override
    public String toString() {
        return lastName + " " + firstName + ", вік: " + age + ", зріст: " + height + " см";
    }
}

// Клас для управління волейбольною командою
class VolleyballTeamManager {
    private LinkedList<VolleyballPlayer> team;

    // Конструктор
    public VolleyballTeamManager() {
        team = new LinkedList<>();
    }

    // 1) Метод для зчитування гравців з файлу
    public void loadPlayersFromFile(String fileName) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(fileName));

        // Очищаємо поточну команду
        team.clear();

        String line;
        int count = 0;

        while ((line = reader.readLine()) != null && count < 6) {
            String[] parts = line.split(",");
            if (parts.length == 4) {
                String lastName = parts[0].trim();
                String firstName = parts[1].trim();
                int age = Integer.parseInt(parts[2].trim());
                int height = Integer.parseInt(parts[3].trim());

                VolleyballPlayer player = new VolleyballPlayer(lastName, firstName, age, height);
                team.add(player);
                count++;
            }
        }

        reader.close();

        System.out.println("Завантажено " + team.size() + " гравців з файлу " + fileName);
    }

    // 2) Метод для отримання масиву об'єктів з непарними індексами
    public VolleyballPlayer[] getPlayersWithOddIndices() {
        List<VolleyballPlayer> oddPlayers = new ArrayList<>();

        for (int i = 0; i < team.size(); i++) {
            if (i % 2 != 0) { // Непарний індекс
                oddPlayers.add(team.get(i));
            }
        }

        return oddPlayers.toArray(new VolleyballPlayer[0]);
    }

    // 3) Метод для отримання списку в зворотному порядку
    public LinkedList<VolleyballPlayer> getReversedTeam() {
        LinkedList<VolleyballPlayer> reversedTeam = new LinkedList<>();

        // Додаємо елементи у зворотному порядку
        for (int i = team.size() - 1; i >= 0; i--) {
            reversedTeam.add(team.get(i));
        }

        return reversedTeam;
    }

    // 4) Метод для заміни об'єкта під індексом 1
    public void replacePlayerAtIndex1(VolleyballPlayer player) {
        if (team.size() > 1) {
            team.set(1, player);
            System.out.println("Гравця під індексом 1 замінено на " + player);
        } else {
            System.out.println("Помилка: в команді немає гравця з індексом 1");
        }
    }

    // 5) Метод для додавання двох об'єктів на початок і кінець списку
    public void addPlayersToStartAndEnd(VolleyballPlayer startPlayer, VolleyballPlayer endPlayer) {
        team.addFirst(startPlayer);
        team.addLast(endPlayer);
        System.out.println("Додано гравців на початок і кінець команди");
    }

    // 6) Метод для видалення двох об'єктів за індексами
    public void removePlayersByIndices(int index1, int index2) {
        // Сортуємо індекси за спаданням, щоб спочатку видалити більший індекс
        // Це потрібно, бо після видалення першого елемента індекси зміщуються
        int higher = Math.max(index1, index2);
        int lower = Math.min(index1, index2);

        if (higher < team.size()) {
            team.remove(higher);

            if (lower < team.size()) { // Перевіряємо після першого видалення
                team.remove(lower);
                System.out.println("Видалено гравців за індексами " + index1 + " та " + index2);
            } else {
                System.out.println("Помилка: індекс " + lower + " виходить за межі списку після першого видалення");
            }
        } else {
            System.out.println("Помилка: індекс " + higher + " виходить за межі списку");
        }
    }

    // Метод для виведення списку гравців
    public void printTeam(String title) {
        System.out.println("\n" + title);
        System.out.println("-----------------------------------------------------");

        int index = 0;
        for (VolleyballPlayer player : team) {
            System.out.println(index + ". " + player);
            index++;
        }

        System.out.println("-----------------------------------------------------");
    }

    // Метод для створення файлу з гравцями (для тестування)
    public void createTestFile(String fileName) throws IOException {
        BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));

        writer.write("Ковальчук, Олександр, 22, 195\n");
        writer.write("Петренко, Максим, 24, 187\n");
        writer.write("Шевченко, Іван, 21, 200\n");
        writer.write("Мельник, Дмитро, 23, 192\n");
        writer.write("Бондаренко, Андрій, 25, 198\n");
        writer.write("Іваненко, Віктор, 20, 189\n");

        writer.close();

        System.out.println("Тестовий файл " + fileName + " створено");
    }

    // Геттер для команди (для тестування)
    public LinkedList<VolleyballPlayer> getTeam() {
        return team;
    }
}

public class VolleyballTeamSystem {
    public static void main(String[] args) {
        VolleyballTeamManager manager = new VolleyballTeamManager();

        try {
            // Створення тестового файлу
            String fileName = "volleyball_players.txt";
            manager.createTestFile(fileName);

            // 1) Завантаження гравців з файлу
            manager.loadPlayersFromFile(fileName);
            manager.printTeam("Початковий склад команди");

            // 2) Отримання гравців з непарними індексами
            VolleyballPlayer[] oddPlayers = manager.getPlayersWithOddIndices();
            System.out.println("\nГравці з непарними індексами:");
            for (int i = 0; i < oddPlayers.length; i++) {
                System.out.println(i + ". " + oddPlayers[i]);
            }

            // 3) Отримання списку в зворотному порядку
            LinkedList<VolleyballPlayer> reversedTeam = manager.getReversedTeam();
            System.out.println("\nКоманда у зворотному порядку:");
            int index = 0;
            for (VolleyballPlayer player : reversedTeam) {
                System.out.println(index + ". " + player);
                index++;
            }

            // 4) Заміна гравця під індексом 1
            VolleyballPlayer newPlayer = new VolleyballPlayer("Лисенко", "Віталій", 26, 194);
            manager.replacePlayerAtIndex1(newPlayer);
            manager.printTeam("Команда після заміни гравця під індексом 1");

            // 5) Додавання гравців на початок і кінець списку
            VolleyballPlayer startPlayer = new VolleyballPlayer("Гончаренко", "Євген", 19, 190);
            VolleyballPlayer endPlayer = new VolleyballPlayer("Федоренко", "Роман", 27, 196);
            manager.addPlayersToStartAndEnd(startPlayer, endPlayer);
            manager.printTeam("Команда після додавання гравців на початок і кінець");

            // 6) Видалення гравців за індексами
            manager.removePlayersByIndices(2, 5);
            manager.printTeam("Команда після видалення гравців за індексами 2 та 5");

        } catch (IOException e) {
            System.err.println("Помилка роботи з файлом: " + e.getMessage());
        }
    }
}