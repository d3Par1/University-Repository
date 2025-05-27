import java.io.*;
import java.util.*;
import org.junit.Before;
import org.junit.After;
import org.junit.Test;
import static org.junit.Assert.*;

// Розширений клас VolleyballTeamManager з функцією запису у файлclass EnhancedVolleyballTeamManager extends VolleyballTeamManager {

    // Метод для запису списку гравців у файл
    public void savePlayersToFile(String fileName) throws IOException {
        BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));

        for (VolleyballPlayer player : getTeam()) {
            writer.write(player.getLastName() + ", " +
                    player.getFirstName() + ", " +
                    player.getAge() + ", " +
                    player.getHeight() + "\n");
        }

        writer.close();

        System.out.println("Збережено " + getTeam().size() + " гравців у файл " + fileName);
    }
}

// Клас тестів для EnhancedVolleyballTeamManager
public class VolleyballTeamManagerTest {

    private EnhancedVolleyballTeamManager manager;
    private final String TEST_FILE = "test_players.txt";
    private final String OUTPUT_FILE = "output_players.txt";

    @Before
    public void setUp() throws IOException {
        manager = new EnhancedVolleyballTeamManager();

        // Створення тестового файлу
        BufferedWriter writer = new BufferedWriter(new FileWriter(TEST_FILE));
        writer.write("Тест1, Гравець1, 20, 190\n");
        writer.write("Тест2, Гравець2, 21, 191\n");
        writer.write("Тест3, Гравець3, 22, 192\n");
        writer.write("Тест4, Гравець4, 23, 193\n");
        writer.write("Тест5, Гравець5, 24, 194\n");
        writer.write("Тест6, Гравець6, 25, 195\n");
        writer.close();
    }

    @After
    public void tearDown() {
        // Видалення тестових файлів
        new File(TEST_FILE).delete();
        new File(OUTPUT_FILE).delete();
    }

    // 1) Тест методу завантаження гравців з файлу
    @Test
    public void testLoadPlayersFromFile() throws IOException {
        // Тест 1: Завантаження з коректного файлу
        manager.loadPlayersFromFile(TEST_FILE);
        assertEquals("Розмір команди має бути 6", 6, manager.getTeam().size());
        assertEquals("Перевірка першого гравця", "Тест1", manager.getTeam().get(0).getLastName());
        assertEquals("Перевірка останнього гравця", "Тест6", manager.getTeam().get(5).getLastName());

        // Тест 2: Завантаження з неіснуючого файлу
        try {
            manager.loadPlayersFromFile("nonexistent_file.txt");
            fail("Очікувалася помилка FileNotFoundException");
        } catch (FileNotFoundException e) {
            // Очікувана поведінка
        }

        // Тест 3: Завантаження порожнього файлу
        BufferedWriter writer = new BufferedWriter(new FileWriter("empty_file.txt"));
        writer.close();

        manager.loadPlayersFromFile("empty_file.txt");
        assertEquals("Розмір команди має бути 0 після завантаження порожнього файлу",
                0, manager.getTeam().size());

        new File("empty_file.txt").delete();
    }

    // 2) Тест методу отримання гравців з непарними індексами
    @Test
    public void testGetPlayersWithOddIndices() throws IOException {
        // Тест 1: Стандартний випадок
        manager.loadPlayersFromFile(TEST_FILE);
        VolleyballPlayer[] oddPlayers = manager.getPlayersWithOddIndices();

        assertEquals("Має бути 3 гравці з непарними індексами", 3, oddPlayers.length);
        assertEquals("Перевірка першого гравця з непарним індексом", "Тест2", oddPlayers[0].getLastName());
        assertEquals("Перевірка другого гравця з непарним індексом", "Тест4", oddPlayers[1].getLastName());
        assertEquals("Перевірка третього гравця з непарним індексом", "Тест6", oddPlayers[2].getLastName());

        // Тест 2: Порожній список
        manager = new EnhancedVolleyballTeamManager();
        oddPlayers = manager.getPlayersWithOddIndices();

        assertEquals("Порожній масив для порожнього списку", 0, oddPlayers.length);

        // Тест 3: Список з одним елементом
        VolleyballPlayer singlePlayer = new VolleyballPlayer("ОдинТест", "Гравець", 30, 200);
        manager.getTeam().add(singlePlayer);
        oddPlayers = manager.getPlayersWithOddIndices();

        assertEquals("Порожній масив для списку з одним елементом (індекс 0)", 0, oddPlayers.length);
    }

    // 3) Тест методу отримання списку в зворотному порядку
    @Test
    public void testGetReversedTeam() throws IOException {
        // Тест 1: Стандартний випадок
        manager.loadPlayersFromFile(TEST_FILE);
        LinkedList<VolleyballPlayer> reversedTeam = manager.getReversedTeam();

        assertEquals("Розмір зворотного списку має бути таким же",
                manager.getTeam().size(), reversedTeam.size());
        assertEquals("Перший елемент зворотного списку має бути останнім у оригінальному",
                manager.getTeam().getLast().getLastName(), reversedTeam.getFirst().getLastName());
        assertEquals("Останній елемент зворотного списку має бути першим у оригінальному",
                manager.getTeam().getFirst().getLastName(), reversedTeam.getLast().getLastName());

        // Тест 2: Порожній список
        manager = new EnhancedVolleyballTeamManager();
        reversedTeam = manager.getReversedTeam();

        assertEquals("Зворотний порожній список має бути порожнім", 0, reversedTeam.size());

        // Тест 3: Список з одним елементом
        VolleyballPlayer singlePlayer = new VolleyballPlayer("ОдинТест", "Гравець", 30, 200);
        manager.getTeam().add(singlePlayer);
        reversedTeam = manager.getReversedTeam();

        assertEquals("Зворотний список з одним елементом має містити один елемент",
                1, reversedTeam.size());
        assertEquals("Перевірка єдиного елемента",
                singlePlayer.getLastName(), reversedTeam.getFirst().getLastName());
    }

    // 4) Тест методу заміни гравця під індексом 1
    @Test
    public void testReplacePlayerAtIndex1() throws IOException {
        // Тест 1: Стандартний випадок
        manager.loadPlayersFromFile(TEST_FILE);
        VolleyballPlayer newPlayer = new VolleyballPlayer("Новий", "Гравець", 28, 197);
        manager.replacePlayerAtIndex1(newPlayer);

        assertEquals("Гравець під індексом 1 має бути замінений",
                newPlayer.getLastName(), manager.getTeam().get(1).getLastName());
        assertEquals("Розмір команди не повинен змінюватися", 6, manager.getTeam().size());

        // Тест 2: Список менше 2 елементів
        manager = new EnhancedVolleyballTeamManager();
        VolleyballPlayer singlePlayer = new VolleyballPlayer("ОдинТест", "Гравець", 30, 200);
        manager.getTeam().add(singlePlayer);

        manager.replacePlayerAtIndex1(newPlayer); // Має вивести помилку, але не викинути виняток
        assertEquals("Розмір команди має залишитися 1", 1, manager.getTeam().size());
        assertEquals("Єдиний гравець не повинен бути змінений",
                singlePlayer.getLastName(), manager.getTeam().get(0).getLastName());

        // Тест 3: Порожній список
        manager = new EnhancedVolleyballTeamManager();
        manager.replacePlayerAtIndex1(newPlayer); // Має вивести помилку, але не викинути виняток
        assertEquals("Розмір команди має залишитися 0", 0, manager.getTeam().size());
    }

    // 5) Тест методу додавання гравців на початок і кінець списку
    @Test
    public void testAddPlayersToStartAndEnd() throws IOException {
        // Тест 1: Стандартний випадок
        manager.loadPlayersFromFile(TEST_FILE);
        int initialSize = manager.getTeam().size();

        VolleyballPlayer startPlayer = new VolleyballPlayer("Початок", "Гравець", 18, 185);
        VolleyballPlayer endPlayer = new VolleyballPlayer("Кінець", "Гравець", 29, 199);
        manager.addPlayersToStartAndEnd(startPlayer, endPlayer);

        assertEquals("Розмір команди має збільшитися на 2",
                initialSize + 2, manager.getTeam().size());
        assertEquals("Перший гравець має бути доданий на початок",
                startPlayer.getLastName(), manager.getTeam().getFirst().getLastName());
        assertEquals("Останній гравець має бути доданий в кінець",
                endPlayer.getLastName(), manager.getTeam().getLast().getLastName());

        // Тест 2: Порожній список
        manager = new EnhancedVolleyballTeamManager();
        manager.addPlayersToStartAndEnd(startPlayer, endPlayer);

        assertEquals("Розмір порожньої команди має стати 2", 2, manager.getTeam().size());
        assertEquals("Перший гравець має бути доданий на початок",
                startPlayer.getLastName(), manager.getTeam().getFirst().getLastName());
        assertEquals("Останній гравець має бути доданий в кінець",
                endPlayer.getLastName(), manager.getTeam().getLast().getLastName());

        // Тест 3: Список з одним елементом
        manager = new EnhancedVolleyballTeamManager();
        VolleyballPlayer singlePlayer = new VolleyballPlayer("ОдинТест", "Гравець", 30, 200);
        manager.getTeam().add(singlePlayer);
        manager.addPlayersToStartAndEnd(startPlayer, endPlayer);

        assertEquals("Розмір команди має стати 3", 3, manager.getTeam().size());
        assertEquals("Перший гравець має бути доданий на початок",
                startPlayer.getLastName(), manager.getTeam().getFirst().getLastName());
        assertEquals("Середній гравець має залишитися на місці",
                singlePlayer.getLastName(), manager.getTeam().get(1).getLastName());
        assertEquals("Останній гравець має бути доданий в кінець",
                endPlayer.getLastName(), manager.getTeam().getLast().getLastName());
    }

    // 6) Тест методу видалення гравців за індексами
    @Test
    public void testRemovePlayersByIndices() throws IOException {
        // Тест 1: Стандартний випадок
        manager.loadPlayersFromFile(TEST_FILE);
        int initialSize = manager.getTeam().size();

        // Запам'ятовуємо гравців, які не повинні бути видалені
        VolleyballPlayer player0 = manager.getTeam().get(0);
        VolleyballPlayer player3 = manager.getTeam().get(3);

        manager.removePlayersByIndices(1, 2);

        assertEquals("Розмір команди має зменшитися на 2",
                initialSize - 2, manager.getTeam().size());
        assertEquals("Гравець з індексом 0 має залишитися на місці",
                player0.getLastName(), manager.getTeam().get(0).getLastName());
        assertEquals("Гравець з індексом 3 має переміститися на позицію 1",
                player3.getLastName(), manager.getTeam().get(1).getLastName());

        // Тест 2: Видалення за неіснуючими індексами
        manager = new EnhancedVolleyballTeamManager();
        manager.loadPlayersFromFile(TEST_FILE);
        initialSize = manager.getTeam().size();

        manager.removePlayersByIndices(10, 20); // Неіснуючі індекси
        assertEquals("Розмір команди не повинен змінюватися",
                initialSize, manager.getTeam().size());

        // Тест 3: Видалення з порожнього списку
        manager = new EnhancedVolleyballTeamManager();
        manager.removePlayersByIndices(0, 1);
        assertEquals("Розмір команди має залишитися 0", 0, manager.getTeam().size());
    }

    // Тест нового методу збереження у файл
    @Test
    public void testSavePlayersToFile() throws IOException {
        // Тест 1: Збереження стандартного списку
        manager.loadPlayersFromFile(TEST_FILE);
        manager.savePlayersToFile(OUTPUT_FILE);

        // Перевіряємо, що файл був створений
        File outputFile = new File(OUTPUT_FILE);
        assertTrue("Файл повинен бути створений", outputFile.exists());

        // Перевіряємо зміст файлу
        BufferedReader reader = new BufferedReader(new FileReader(OUTPUT_FILE));
        int lineCount = 0;
        String line;

        while ((line = reader.readLine()) != null) {
            lineCount++;
        }

        reader.close();

        assertEquals("Кількість рядків у файлі має відповідати розміру команди",
                manager.getTeam().size(), lineCount);

        // Тест 2: Збереження порожнього списку
        manager = new EnhancedVolleyballTeamManager();
        manager.savePlayersToFile(OUTPUT_FILE);

        reader = new BufferedReader(new FileReader(OUTPUT_FILE));
        lineCount = 0;

        while ((line = reader.readLine()) != null) {
            lineCount++;
        }

        reader.close();

        assertEquals("Файл повинен бути порожнім", 0, lineCount);

        // Тест 3: Завантаження збереженого файлу
        manager = new EnhancedVolleyballTeamManager();
        manager.loadPlayersFromFile(TEST_FILE);
        manager.savePlayersToFile(OUTPUT_FILE);

        EnhancedVolleyballTeamManager newManager = new EnhancedVolleyballTeamManager();
        newManager.loadPlayersFromFile(OUTPUT_FILE);

        assertEquals("Розмір завантаженої команди має відповідати оригінальній",
                manager.getTeam().size(), newManager.getTeam().size());
    }
}