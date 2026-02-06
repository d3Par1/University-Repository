import java.util.Scanner;

public class BinaryConverter {

    // Метод для перевірки коректності двійкового числа
    public static boolean isValidBinary(String binary) {
        // Перевіряємо, чи всі символи є 0 або 1
        for (int i = 0; i < binary.length(); i++) {
            char c = binary.charAt(i);
            if (c != '0' && c != '1') {
                return false;
            }
        }
        return true;
    }

    // Метод для конвертації двійкового числа в десяткове
    public static int binaryToDecimal(String binary) {
        int decimal = 0;
        int power = 0;

        // Проходимо число з кінця
        for (int i = binary.length() - 1; i >= 0; i--) {
            if (binary.charAt(i) == '1') {
                decimal += Math.pow(2, power);
            }
            power++;
        }

        return decimal;
    }

    // Метод для конвертації десяткового числа в двійкове (повертає масив з 0 та 1)
    public static int[] decimalToBinary(int decimal) {
        // Якщо число 0, повертаємо масив з одним елементом 0
        if (decimal == 0) {
            return new int[]{0};
        }

        // Визначаємо кількість біт, потрібних для представлення числа
        int bitCount = 0;
        int temp = decimal;
        while (temp > 0) {
            temp /= 2;
            bitCount++;
        }

        // Створюємо масив потрібного розміру
        int[] binary = new int[bitCount];

        // Заповнюємо масив
        for (int i = bitCount - 1; i >= 0; i--) {
            binary[i] = decimal % 2;
            decimal /= 2;
        }

        return binary;
    }

    // Метод для конвертації десяткового числа в двійковий рядок
    public static String decimalToBinaryString(int decimal) {
        int[] binaryArray = decimalToBinary(decimal);
        StringBuilder binaryString = new StringBuilder();

        for (int bit : binaryArray) {
            binaryString.append(bit);
        }

        return binaryString.toString();
    }

    // Метод для виведення масиву бітів у вигляді рядка
    public static void printBinaryArray(int[] binary) {
        for (int bit : binary) {
            System.out.print(bit);
        }
        System.out.println();
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        // Частина 1: Конвертація з двійкової в десяткову систему
        System.out.println("Частина 1: Конвертація з двійкової в десяткову систему");

        String binaryInput;
        boolean validInput = false;

        do {
            System.out.print("Введіть число у двійковій системі числення (тільки 0 та 1): ");
            binaryInput = scanner.nextLine().trim();

            if (isValidBinary(binaryInput)) {
                validInput = true;
            } else {
                System.out.println("Помилка! Двійкове число повинно містити тільки символи 0 та 1. Спробуйте ще раз.");
            }
        } while (!validInput);

        int decimal = binaryToDecimal(binaryInput);
        System.out.println("Число у десятковій системі: " + decimal);

        // Частина 2: Конвертація з десяткової в двійкову систему
        System.out.println("\nЧастина 2: Конвертація з десяткової в двійкову систему");

        System.out.print("Введіть число у десятковій системі: ");
        int decimalInput = scanner.nextInt();

        int[] binaryArray = decimalToBinary(decimalInput);
        System.out.print("Число у двійковій системі (масив): ");
        printBinaryArray(binaryArray);

        String binaryString = decimalToBinaryString(decimalInput);
        System.out.println("Число у двійковій системі (рядок): " + binaryString);

        // Перевірка правильності конвертації
        System.out.println("\nПеревірка правильності конвертації:");
        System.out.println("Початкове десяткове число: " + decimalInput);
        System.out.println("Двійкове представлення: " + binaryString);
        System.out.println("Зворотна конвертація в десяткове: " + binaryToDecimal(binaryString));

        scanner.close();
    }
}