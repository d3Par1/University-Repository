import java.io.*;
import java.util.*;

public class Rgr2Step {
    // Функція для генерації випадкового 4-значного числа з різними цифрами
    private static int generateRandomNumber() {
        List<Integer> digits = new ArrayList<>(Arrays.asList(0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
        Collections.shuffle(digits);
        
        // Переконуємося, що перша цифра не 0 (щоб зберегти 4-значне число)
        if (digits.get(0) == 0) {
            // Міняємо місцями з ненульовою цифрою
            for (int i = 1; i < digits.size(); i++) {
                if (digits.get(i) != 0) {
                    int temp = digits.get(0);
                    digits.set(0, digits.get(i));
                    digits.set(i, temp);
                    break;
                }
            }
        }
        
        // Створюємо 4-значне число з першими 4 цифрами
        return digits.get(0) * 1000 + digits.get(1) * 100 + digits.get(2) * 10 + digits.get(3);
    }
    
    // Функція для підрахунку биків і корів
    private static int[] countBullsAndCows(int secretNumber, int guess) {
        int[] result = new int[2]; // [бики, корови]
        
        // Перетворюємо числа на рядки для легшого порівняння цифр
        String secretStr = String.valueOf(secretNumber);
        String guessStr = String.valueOf(guess);
        
        // Додаємо провідні нулі, якщо необхідно
        while (guessStr.length() < 4) {
            guessStr = "0" + guessStr;
        }
        
        // Підраховуємо биків (правильна цифра на правильній позиції)
        for (int i = 0; i < 4; i++) {
            if (secretStr.charAt(i) == guessStr.charAt(i)) {
                result[0]++; // Збільшуємо биків
            }
        }
        
        // Підраховуємо загальну кількість збігів цифр
        int[] secretCount = new int[10]; // Кількість кожної цифри в секретному числі
        int[] guessCount = new int[10];  // Кількість кожної цифри в здогадці
        
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
        String numStr = String.valueOf(number);
        Set<Character> digits = new HashSet<>();
        
        for (char digit : numStr.toCharArray()) {
            if (!digits.add(digit)) {
                return false;
            }
        }
        
        return true;
    }
    
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int secretNumber = generateRandomNumber();
        int attempts = 0;
        boolean guessed = false;
        
        try (PrintWriter gameWriter = new PrintWriter(new FileWriter("game_result.txt"))) {
            System.out.println("Ласкаво просимо до гри «Корови та бики»!");
            System.out.println("Спробуйте вгадати 4-значне число з різними цифрами.");
            gameWriter.println("Гра «Корови та бики»");
            gameWriter.println("Секретне число: " + secretNumber);
            
            while (!guessed) {
                System.out.print("Введіть вашу здогадку (4-значне число): ");
                
                // Перевіряємо, чи ввід є числом
                if (!scanner.hasNextInt()) {
                    System.out.println("Будь ласка, введіть дійсне число.");
                    scanner.next(); // Відкидаємо невірний ввід
                    continue;
                }
                
                int guess = scanner.nextInt();
                attempts++;
                
                // Перевіряємо, чи це 4-значне число
                if (guess < 1000 || guess > 9999) {
                    System.out.println("Будь ласка, введіть 4-значне число.");
                    continue;
                }
                
                // Перевіряємо, чи містить число унікальні цифри
                if (!hasUniqueDigits(guess)) {
                    System.out.println("Будь ласка, введіть число з унікальними цифрами.");
                    continue;
                }
                
                int[] result = countBullsAndCows(secretNumber, guess);
                int bulls = result[0];
                int cows = result[1];
                
                String cowsWord = getCorrectWordForm(cows, "корова", "корови", "корів");
                String bullsWord = getCorrectWordForm(bulls, "бик", "бики", "биків");
                
                System.out.println("Спроба " + attempts + ": " + cows + " " + cowsWord + ", " + bulls + " " + bullsWord);
                gameWriter.println("Спроба " + attempts + ": " + guess + " -> " + cows + " " + cowsWord + ", " + bulls + " " + bullsWord);
                
                if (bulls == 4) {
                    guessed = true;
                    System.out.println("Вітаємо! Ви вгадали число " + secretNumber + " за " + attempts + " спроб.");
                    gameWriter.println("Число вгадано за " + attempts + " спроб!");
                }
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