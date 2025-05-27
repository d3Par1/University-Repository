// Пакет для практичного завдання 7
package pz7;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * Клас для вирішення задач з динамічного програмування
 */
public class DynamicProgramming {

    /**
     * Задача 1: Знаходження мінімального шляху в матриці
     * @param matrix Матриця вартостей проходу
     * @return Масив, що містить мінімальну вартість і шлях
     */
    public static Object[] findMinimumPathInMatrix(int[][] matrix) {
        int n = matrix.length;
        int m = matrix[0].length;

        // Створюємо DP таблицю для зберігання мінімальних вартостей
        int[][] dp = new int[n][m];
        // Створюємо матрицю для відстеження шляху (0 - вправо, 1 - вниз)
        int[][] path = new int[n][m];

        // Заповнюємо першу клітинку
        dp[0][0] = matrix[0][0];

        // Заповнюємо перший рядок (можна рухатись тільки вправо)
        for (int j = 1; j < m; j++) {
            dp[0][j] = dp[0][j-1] + matrix[0][j];
            path[0][j] = 0; // Прийшли справа
        }

        // Заповнюємо перший стовпець (можна рухатись тільки вниз)
        for (int i = 1; i < n; i++) {
            dp[i][0] = dp[i-1][0] + matrix[i][0];
            path[i][0] = 1; // Прийшли зверху
        }

        // Заповнюємо решту DP таблиці
        for (int i = 1; i < n; i++) {
            for (int j = 1; j < m; j++) {
                // Вибираємо мінімум між шляхом зверху і зліва
                if (dp[i-1][j] <= dp[i][j-1]) {
                    dp[i][j] = dp[i-1][j] + matrix[i][j];
                    path[i][j] = 1; // Прийшли зверху
                } else {
                    dp[i][j] = dp[i][j-1] + matrix[i][j];
                    path[i][j] = 0; // Прийшли зліва
                }
            }
        }

        // Відновлюємо шлях
        List<int[]> pathCoordinates = new ArrayList<>();
        int i = n - 1, j = m - 1;
        pathCoordinates.add(new int[]{i, j});

        while (i > 0 || j > 0) {
            if (i == 0) {
                j--;
            } else if (j == 0) {
                i--;
            } else if (path[i][j] == 1) {
                i--;
            } else {
                j--;
            }
            pathCoordinates.add(0, new int[]{i, j});
        }

        return new Object[]{dp[n-1][m-1], pathCoordinates};
    }

    /**
     * Задача 2: Кількість способів піднятися сходами
     * @param n загальна кількість сходинок
     * @param forbidden масив заборонених сходинок
     * @return кількість способів дістатися вершини
     */
    public static long countWaysToClimbStairs(int n, int[] forbidden) {
        // Перевіряємо, чи заборонені сходинки не є послідовними
        Arrays.sort(forbidden);
        for (int i = 1; i < forbidden.length; i++) {
            if (forbidden[i] == forbidden[i-1] + 1) {
                throw new IllegalArgumentException("Заборонені сходинки не можуть бути послідовними");
            }
        }

        // Створюємо множину заборонених сходинок для швидкого пошуку
        Set<Integer> forbiddenSet = new HashSet<>();
        for (int step : forbidden) {
            forbiddenSet.add(step);
        }

        // Створюємо масив для DP
        long[] dp = new long[n + 1];

        // Базовий випадок
        dp[0] = 1; // Один спосіб опинитись на початку (нічого не робити)

        // Заповнюємо DP масив
        for (int i = 1; i <= n; i++) {
            if (forbiddenSet.contains(i)) {
                dp[i] = 0; // Не можна стояти на забороненій сходинці
            } else {
                dp[i] = (i >= 1 ? dp[i-1] : 0) + (i >= 2 ? dp[i-2] : 0);
            }
        }

        return dp[n];
    }

    /**
     * Задача 3: Максимальна сума підмасиву (алгоритм Кадане)
     * @param array вхідний масив
     * @return масив, що містить максимальну суму і індекси підмасиву
     */
    public static Object[] findMaximumSubarraySum(int[] array) {
        int n = array.length;

        // Поточна сума і максимальна сума
        int currentSum = array[0];
        int maxSum = array[0];

        // Індекси для відстеження оптимального підмасиву
        int currentStart = 0;
        int currentEnd = 0;
        int maxStart = 0;
        int maxEnd = 0;

        // Алгоритм Кадане
        for (int i = 1; i < n; i++) {
            // Вирішуємо, чи продовжувати поточний підмасив чи почати новий
            if (currentSum + array[i] > array[i]) {
                currentSum += array[i];
                currentEnd = i;
            } else {
                currentSum = array[i];
                currentStart = i;
                currentEnd = i;
            }

            // Оновлюємо максимальну суму, якщо поточна сума більша
            if (currentSum > maxSum) {
                maxSum = currentSum;
                maxStart = currentStart;
                maxEnd = currentEnd;
            }
        }

        // Створюємо підмасив, який має максимальну суму
        int[] subarray = Arrays.copyOfRange(array, maxStart, maxEnd + 1);

        return new Object[]{maxSum, subarray, maxStart, maxEnd};
    }

    /**
     * Головний метод для тестування
     */
    public static void main(String[] args) {
        // Тестування задачі 1
        System.out.println("Задача 1: Мінімальний шлях у матриці");
        int[][] matrix = {
                {1, 3, 1},
                {1, 5, 1},
                {4, 2, 1}
        };

        Object[] result1 = findMinimumPathInMatrix(matrix);
        int minCost = (int) result1[0];
        List<int[]> path = (List<int[]>) result1[1];

        System.out.println("Мінімальна вартість: " + minCost);
        System.out.print("Шлях: ");
        for (int[] point : path) {
            System.out.print("(" + point[0] + "," + point[1] + ") ");
        }
        System.out.println();

        // Тестування задачі 2
        System.out.println("\nЗадача 2: Підйом сходами");
        int n = 10;
        int[] forbidden = {3, 5, 7, 8, 9};
        long ways = countWaysToClimbStairs(n, forbidden);
        System.out.println("Кількість способів дістатися вершини " + n + " сходинок: " + ways);

        // Тестування задачі 3
        System.out.println("\nЗадача 3: Максимальна сума підмасиву");
        int[] array = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
        Object[] result3 = findMaximumSubarraySum(array);
        int maxSum = (int) result3[0];
        int[] subarray = (int[]) result3[1];
        int startIndex = (int) result3[2];
        int endIndex = (int) result3[3];

        System.out.println("Максимальна сума: " + maxSum);
        System.out.println("Підмасив: " + Arrays.toString(subarray));
        System.out.println("Індекси: від " + startIndex + " до " + endIndex);
    }
}