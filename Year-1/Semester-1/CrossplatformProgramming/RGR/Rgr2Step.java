import java.io.*;
import java.util.*;

public class Rgr2Step {
    // Функція для генерації випадкового 4-значного числа з різними цифрами
    private static int generateRandomNumber() {
        List<Integer> digits = new ArrayList<>(Arrays.asList(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
        Collections.shuffle(digits);

        // Переконуємося, що перша цифра не 0
        if (digits.get(0) == 0) {
            // Знаходимо першу ненульову цифру і міняємо місцями
            for (int i = 1; i < digits.size(); i++) {
                if (digits.get(i) != 0) {
                    Collections.swap(digits, 0, i);
                    break;
                }
            }
        }

        // Створюємо 4-значне число з першими 4 цифрами
        int number = 0;
        for (int i = 0; i < 4; i++) {
            number = number * 10 + digits.get(i);
        }

        return number;
    }

    // Функція для підрахунку биків і корів
    private static int[] countBullsAndCows(int secretNumber, int guess) {
        int[] result = new int[2]; // [бики, корови]

        // Перетворюємо числа на рядки для легшого порівняння цифр
        String secretStr = String.valueOf(secretNumber);
        String guessStr = String.format("%04d", guess); // Better formatting for leading zeros

        // Підраховуємо биків (правильна цифра на правильній позиції)
        for (int i = 0; i < 4; i++) {
            if (secretStr.charAt(i) == guessStr.charAt(i)) {
                result[0]++; // Збільшуємо биків
            }
        }

        // Підраховуємо загальну кількість збігів цифр
        int[] secretCount = new int[10]; // Кількість кожної цифри в секретному числі
        int[] guessCount = new int[10]; // Кількість кожної цифри в здогадці

        for (int i = 0; i < 4; i++) {
            secretCount[secretStr.charAt(i) - '0']++;
            guessCount[guessStr.charAt(i) - '0']++;
        }

        int totalMatches = 0;
        for (int i = 0; i < 10; i++) {
            totalMatches += Math.min(secretCount[i], guessCount[i]);
        }

        // Корови = загальні збіги - бики
        result[1] = totalMatches - result[0];

        return result;
    }

    // Функція для перевірки, чи містить число унікальні цифри
    private static boolean hasUniqueDigits(int number) {
        String numStr = String.format("%04d", number); // Handle potential leading zeros
        Set<Character> digits = new HashSet<>();

        for (char digit : numStr.toCharArray()) {
            if (!digits.add(digit)) {
                return false;
            }
        }

        return numStr.length() == 4; // Ensure we actually have 4 digits
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int secretNumber = generateRandomNumber();
        int attempts = 0;
        boolean guessed = false;
        final int MAX_ATTEMPTS = 10; // Maximum number of attempts

        try (PrintWriter gameWriter = new PrintWriter(new FileWriter("game_result.txt"))) {
            System.out.println("Ласкаво просимо до гри «Корови та бики»!");
            System.out.println("Спробуйте вгадати 4-значне число з різними цифрами.");
            System.out.println("У вас є " + MAX_ATTEMPTS + " спроб. Щоб вийти, введіть 'вихід'.");
            gameWriter.println("Гра «Корови та бики»");

            while (!guessed && attempts < MAX_ATTEMPTS) {
                System.out.print("Введіть вашу здогадку (4-значне число) [" + (attempts + 1) + "/"
                        + MAX_ATTEMPTS + "]: ");

                // Перевіряємо чи гравець хоче вийти
                if (scanner.hasNextLine()) {
                    String input = scanner.nextLine().trim();

                    // Перевірка на команду виходу з гри
                    if (input.equalsIgnoreCase("вихід")) {
                        System.out.println("Гра завершена. Загадане число було: " + secretNumber);
                        gameWriter.println(
                                "Гра завершена користувачем. Секретне число: " + secretNumber);
                        break;
                    }

                    // Перевіряємо, чи ввід є числом
                    try {
                        int guess = Integer.parseInt(input);
                        attempts++;

                        // Перевіряємо, чи це 4-значне число
                        if (guess < 1000 || guess > 9999) {
                            System.out.println("Будь ласка, введіть 4-значне число.");
                            attempts--; // Не рахуємо цю спробу
                            continue;
                        }

                        // Перевіряємо, чи містить число унікальні цифри
                        if (!hasUniqueDigits(guess)) {
                            System.out.println("Будь ласка, введіть число з унікальними цифрами.");
                            attempts--; // Не рахуємо цю спробу
                            continue;
                        }

                        int[] result = countBullsAndCows(secretNumber, guess);
                        int bulls = result[0];
                        int cows = result[1];

                        String cowsWord = getCorrectWordForm(cows, "корова", "корови", "корів");
                        String bullsWord = getCorrectWordForm(bulls, "бик", "бики", "биків");

                        System.out.println("Спроба " + attempts + ": " + cows + " " + cowsWord
                                + ", " + bulls + " " + bullsWord);
                        gameWriter.println("Спроба " + attempts + ": " + guess + " -> " + cows + " "
                                + cowsWord + ", " + bulls + " " + bullsWord);

                        if (bulls == 4) {
                            guessed = true;
                            System.out.println("Вітаємо! Ви вгадали число " + secretNumber + " за "
                                    + attempts + " спроб.");
                            gameWriter.println("Число вгадано за " + attempts + " спроб!");
                        }
                    } catch (NumberFormatException e) {
                        System.out.println(
                                "Будь ласка, введіть дійсне число або 'вихід' для завершення гри.");
                    }
                }
            }

            // Додаємо секретне число в кінці гри, а не на початку
            gameWriter.println("Секретне число: " + secretNumber);

            if (!guessed && attempts >= MAX_ATTEMPTS) {
                System.out.println("На жаль, ви вичерпали всі " + MAX_ATTEMPTS
                        + " спроб. Загадане число було: " + secretNumber);
                gameWriter.println("Ліміт спроб вичерпано. Секретне число: " + secretNumber);
            }
        } catch (IOException e) {
            System.err.println("Помилка при створенні файлу результатів гри: " + e.getMessage());
        } finally {
            scanner.close();
        }
    }

    // Функція для отримання правильної форми слова залежно від числа
    private static String getCorrectWordForm(int number, String form1, String form2, String form5) {
        int remainder10 = number % 10;
        int remainder100 = number % 100;

        if (remainder100 >= 11 && remainder100 <= 19) {
            return form5;
        }

        if (remainder10 == 1) {
            return form1;
        }

        if (remainder10 >= 2 && remainder10 <= 4) {
            return form2;
        }

        return form5;
    }
}
