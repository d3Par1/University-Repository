public class FibonacciCalculator {
    // Порогове значення, при якому ми переходимо від рекурсії до ітерації
    private static final int THRESHOLD = 30; // Значення для прикладу, потрібно визначити експериментально

    // Гібридний метод для обчислення n-го числа Фібоначчі
    public static long fibonacci(int n) {
        // Перевірка на некоректні вхідні дані
        if (n < 0) {
            throw new IllegalArgumentException("Вхідне значення не може бути від'ємним");
        }

        // Базові випадки
        if (n == 0) return 0;
        if (n == 1) return 1;

        // Якщо n менше порогового значення, використовуємо рекурсію
        if (n <= THRESHOLD) {
            return fibonacciRecursive(n);
        }
        // Інакше використовуємо ітераційний підхід
        else {
            return fibonacciIterative(n);
        }
    }

    // Рекурсивний метод для обчислення числа Фібоначчі
    private static long fibonacciRecursive(int n) {
        if (n <= 1) return n;
        return fibonacciRecursive(n - 1) + fibonacciRecursive(n - 2);
    }

    // Ітераційний метод для обчислення числа Фібоначчі
    private static long fibonacciIterative(int n) {
        if (n <= 1) return n;

        long prev = 0;
        long current = 1;
        long next;

        for (int i = 2; i <= n; i++) {
            next = prev + current;
            prev = current;
            current = next;
        }

        return current;
    }

    // Метод для визначення оптимального порогового значення
    public static void findOptimalThreshold() {
        System.out.println("Експериментальне визначення порогового значення:");
        System.out.println("n\tРекурсивний (мс)\tІтеративний (мс)");

        for (int n = 20; n <= 45; n += 5) {
            // Вимірюємо час для рекурсивного методу
            long startTimeRecursive = System.currentTimeMillis();
            try {
                if (n <= 40) { // Обмежуємо n для рекурсії, щоб уникнути занадто довгих обчислень
                    fibonacciRecursive(n);
                }
                long endTimeRecursive = System.currentTimeMillis();
                long recursiveTime = endTimeRecursive - startTimeRecursive;

                // Вимірюємо час для ітеративного методу
                long startTimeIterative = System.currentTimeMillis();
                fibonacciIterative(n);
                long endTimeIterative = System.currentTimeMillis();
                long iterativeTime = endTimeIterative - startTimeIterative;

                System.out.println(n + "\t" + recursiveTime + "\t\t\t" + iterativeTime);

                // Якщо рекурсивний метод займає більше 10 секунд, вважаємо його неприйнятним
                if (recursiveTime > 10000) {
                    System.out.println("\nПорогове значення має бути встановлене на " + (n - 5));
                    System.out.println("При n >= " + n + " рекурсивний метод стає неприйнятним (> 10 секунд)");
                    break;
                }
            } catch (Exception e) {
                System.out.println(n + "\tСтек переповнений\t" + "N/A");
                System.out.println("\nПорогове значення має бути встановлене на " + (n - 5));
                break;
            }
        }
    }

    public static void main(String[] args) {
        // Демонстрація роботи гібридного методу
        System.out.println("Демонстрація роботи гібридного методу:");

        int[] testValues = {5, 10, 20, 30, 40, 45};

        for (int n : testValues) {
            long startTime = System.currentTimeMillis();
            long result = fibonacci(n);
            long endTime = System.currentTimeMillis();
            long executionTime = endTime - startTime;

            System.out.println("Fibonacci(" + n + ") = " + result + " (обчислено за " + executionTime + " мс)");
        }

        // Знаходження оптимального порогового значення
        System.out.println("\nВизначення оптимального порогового значення:");
        findOptimalThreshold();

        // Порівняння різних методів для певного n
        int n = 35;
        System.out.println("\nПорівняння методів для n = " + n + ":");

        // Рекурсивний метод
        long startRecursive = System.currentTimeMillis();
        try {
            long resultRecursive = fibonacciRecursive(n);
            long endRecursive = System.currentTimeMillis();
            System.out.println("Рекурсивний метод: " + resultRecursive +
                    " (обчислено за " + (endRecursive - startRecursive) + " мс)");
        } catch (Exception e) {
            System.out.println("Рекурсивний метод: помилка переповнення стеку");
        }

        // Ітеративний метод
        long startIterative = System.currentTimeMillis();
        long resultIterative = fibonacciIterative(n);
        long endIterative = System.currentTimeMillis();
        System.out.println("Ітеративний метод: " + resultIterative +
                " (обчислено за " + (endIterative - startIterative) + " мс)");

        // Гібридний метод
        long startHybrid = System.currentTimeMillis();
        long resultHybrid = fibonacci(n);
        long endHybrid = System.currentTimeMillis();
        System.out.println("Гібридний метод: " + resultHybrid +
                " (обчислено за " + (endHybrid - startHybrid) + " мс)");
    }
}