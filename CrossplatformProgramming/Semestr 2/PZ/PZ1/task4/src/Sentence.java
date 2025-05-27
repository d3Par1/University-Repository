import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Sentence {
    private String text;

    // Конструктор з параметром
    public Sentence(String text) {
        this.text = text;
    }

    // Конструктор без параметрів
    public Sentence() {
        this("");
    }

    // Гетер і сетер для тексту
    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    // Метод для розбиття речення на слова
    public String[] splitIntoWords() {
        if (text.isEmpty()) {
            return new String[0];
        }

        // Видаляємо зайві пробіли та розділові знаки
        String cleanText = text.trim().replaceAll("[,.!?:;]", "");

        // Розбиваємо текст на слова за пробілами
        return cleanText.split("\\s+");
    }

    // Метод для додавання слова в кінець речення
    public void addWord(String word) {
        if (text.isEmpty()) {
            text = word;
        } else {
            text = text.trim() + " " + word;
        }
    }

    // Метод для видалення слова
    public void removeWord(String word) {
        // Отримуємо масив слів
        String[] words = splitIntoWords();
        StringBuilder newText = new StringBuilder();

        boolean isFirst = true;

        // Будуємо нове речення без вказаного слова
        for (String w : words) {
            if (!w.equalsIgnoreCase(word)) {
                if (!isFirst) {
                    newText.append(" ");
                }
                newText.append(w);
                isFirst = false;
            }
        }

        text = newText.toString();
    }

    // Метод для вставки слова після певного індексу
    public void insertWordAfter(int index, String word) {
        String[] words = splitIntoWords();

        if (index < 0 || index >= words.length) {
            System.out.println("Невірний індекс для вставки слова");
            return;
        }

        List<String> wordList = new ArrayList<>(Arrays.asList(words));
        wordList.add(index + 1, word);

        // Об'єднуємо слова назад у речення
        text = String.join(" ", wordList);
    }

    // Метод для отримання кількості літер в реченні
    public int countLetters() {
        // Видаляємо всі символи, крім літер, і рахуємо їх
        return text.replaceAll("[^а-яА-Яa-zA-Z]", "").length();
    }

    // Метод для отримання кількості слів
    public int countWords() {
        if (text.isEmpty()) {
            return 0;
        }
        return splitIntoWords().length;
    }

    // Метод для знаходження найдовшого слова
    public String getLongestWord() {
        String[] words = splitIntoWords();

        if (words.length == 0) {
            return "";
        }

        String longestWord = "";

        for (String word : words) {
            if (word.length() > longestWord.length()) {
                longestWord = word;
            }
        }

        return longestWord;
    }

    // Метод для знаходження найкоротшого слова
    public String getShortestWord() {
        String[] words = splitIntoWords();

        if (words.length == 0) {
            return "";
        }

        String shortestWord = words[0];

        for (String word : words) {
            if (word.length() < shortestWord.length()) {
                shortestWord = word;
            }
        }

        return shortestWord;
    }

    // Метод для перевірки чи є в реченні задане слово
    public boolean containsWord(String word) {
        String[] words = splitIntoWords();

        for (String w : words) {
            if (w.equalsIgnoreCase(word)) {
                return true;
            }
        }

        return false;
    }

    // Метод для виведення слова під заданим номером
    public String getWordByIndex(int index) {
        String[] words = splitIntoWords();

        if (index < 0 || index >= words.length) {
            return "Слово з таким індексом не існує";
        }

        return words[index];
    }

    // Метод для порівняння двох речень
    public static int compareSentences(Sentence sentence1, Sentence sentence2) {
        int wordCount1 = sentence1.countWords();
        int wordCount2 = sentence2.countWords();

        if (wordCount1 < wordCount2) {
            return -1; // перше речення коротше
        } else if (wordCount1 > wordCount2) {
            return 1;  // перше речення довше
        } else {
            return 0;  // речення мають однакову кількість слів
        }
    }

    // Демонстрація роботи класу
    public static void main(String[] args) {
        // Створення речень
        Sentence sentence1 = new Sentence("Це тестове речення для перевірки роботи класу.");
        Sentence sentence2 = new Sentence();
        sentence2.setText("Алгоритми та структури даних є важливими в програмуванні.");

        // Виведення початкових речень
        System.out.println("Початкові речення:");
        System.out.println("Речення 1: " + sentence1.getText());
        System.out.println("Речення 2: " + sentence2.getText());

        // Розбиття на слова
        System.out.println("\nСлова в реченні 1:");
        String[] words1 = sentence1.splitIntoWords();
        for (int i = 0; i < words1.length; i++) {
            System.out.println((i + 1) + ". " + words1[i]);
        }

        // Додавання слова
        sentence1.addWord("Чудово!");
        System.out.println("\nРечення 1 після додавання слова: " + sentence1.getText());

        // Видалення слова
        sentence2.removeWord("є");
        System.out.println("Речення 2 після видалення слова 'є': " + sentence2.getText());

        // Вставка слова
        sentence1.insertWordAfter(2, "нове");
        System.out.println("Речення 1 після вставки слова 'нове' після 3-го слова: " + sentence1.getText());

        // Кількість літер і слів
        System.out.println("\nСтатистика речень:");
        System.out.println("Речення 1:");
        System.out.println("- Кількість літер: " + sentence1.countLetters());
        System.out.println("- Кількість слів: " + sentence1.countWords());
        System.out.println("- Найдовше слово: " + sentence1.getLongestWord());
        System.out.println("- Найкоротше слово: " + sentence1.getShortestWord());

        System.out.println("\nРечення 2:");
        System.out.println("- Кількість літер: " + sentence2.countLetters());
        System.out.println("- Кількість слів: " + sentence2.countWords());
        System.out.println("- Найдовше слово: " + sentence2.getLongestWord());
        System.out.println("- Найкоротше слово: " + sentence2.getShortestWord());

        // Перевірка наявності слова
        String wordToCheck = "речення";
        System.out.println("\nПеревірка наявності слова '" + wordToCheck + "':");
        System.out.println("Речення 1: " + (sentence1.containsWord(wordToCheck) ? "містить" : "не містить"));
        System.out.println("Речення 2: " + (sentence2.containsWord(wordToCheck) ? "містить" : "не містить"));

        // Отримання слова за індексом
        int index = 2;
        System.out.println("\nСлово з індексом " + index + ":");
        System.out.println("Речення 1: " + sentence1.getWordByIndex(index));
        System.out.println("Речення 2: " + sentence2.getWordByIndex(index));

        // Порівняння речень
        System.out.println("\nПорівняння речень:");
        int comparison = Sentence.compareSentences(sentence1, sentence2);
        if (comparison < 0) {
            System.out.println("Речення 1 містить менше слів, ніж речення 2");
        } else if (comparison > 0) {
            System.out.println("Речення 1 містить більше слів, ніж речення 2");
        } else {
            System.out.println("Речення містять однакову кількість слів");
        }
    }
}