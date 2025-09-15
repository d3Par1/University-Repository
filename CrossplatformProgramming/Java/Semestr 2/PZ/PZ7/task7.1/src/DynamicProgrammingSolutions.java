import java.util.*;

public class DynamicProgrammingSolutions {

    public static class Task1MinPathSum {

        public static class PathResult {
            int minCost;
            List<String> path;

            PathResult(int cost, List<String> path) {
                this.minCost = cost;
                this.path = path;
            }
        }

        public static PathResult findMinPath(int[][] matrix) {
            int rows = matrix.length;
            int cols = matrix[0].length;

            int[][] dp = new int[rows][cols];

            dp[0][0] = matrix[0][0];

            for (int j = 1; j < cols; j++) {
                dp[0][j] = dp[0][j-1] + matrix[0][j];
            }

            for (int i = 1; i < rows; i++) {
                dp[i][0] = dp[i-1][0] + matrix[i][0];
            }

            for (int i = 1; i < rows; i++) {
                for (int j = 1; j < cols; j++) {
                    dp[i][j] = Math.min(dp[i-1][j], dp[i][j-1]) + matrix[i][j];
                }
            }

            List<String> path = new ArrayList<>();
            int i = rows - 1, j = cols - 1;

            while (i > 0 || j > 0) {
                path.add(0, "(" + i + "," + j + ")");

                if (i == 0) {
                    j--;
                } else if (j == 0) {
                    i--;
                } else {
                    if (dp[i-1][j] < dp[i][j-1]) {
                        i--;
                    } else {
                        j--;
                    }
                }
            }
            path.add(0, "(0,0)");

            return new PathResult(dp[rows-1][cols-1], path);
        }

        public static void solve() {
            System.out.println("=== Задача 1: Маршрут з мінімальною вартістю ===");

            int[][] matrix = {
                    {1, 3, 1, 5},
                    {2, 1, 4, 2},
                    {5, 2, 1, 3},
                    {1, 1, 2, 1}
            };

            System.out.println("Матриця вартості:");
            printMatrix(matrix);

            PathResult result = findMinPath(matrix);

            System.out.println("Мінімальна вартість: " + result.minCost);
            System.out.println("Оптимальний шлях: " + String.join(" -> ", result.path));
        }

        private static void printMatrix(int[][] matrix) {
            for (int[] row : matrix) {
                for (int val : row) {
                    System.out.printf("%3d ", val);
                }
                System.out.println();
            }
        }
    }

    public static class Task2StairClimbing {

        public static long countWays(int n, Set<Integer> forbidden) {
            long[] dp = new long[n + 1];
            dp[0] = 1;

            for (int i = 1; i <= n; i++) {
                if (forbidden.contains(i)) {
                    dp[i] = 0;
                } else {
                    if (i >= 1) dp[i] += dp[i - 1];

                    if (i >= 2) dp[i] += dp[i - 2];
                }
            }

            return dp[n];
        }

        public static void solve() {
            System.out.println("\n=== Задача 2: Драбина з забороненими сходинками ===");

            Scanner scanner = new Scanner(System.in);

            System.out.print("Введіть кількість сходинок (10-45): ");
            int n = scanner.nextInt();

            System.out.println("Введіть 5 заборонених сходинок:");
            Set<Integer> forbidden = new HashSet<>();
            for (int i = 0; i < 5; i++) {
                System.out.print("Заборонена сходинка " + (i + 1) + ": ");
                int step = scanner.nextInt();
                forbidden.add(step);
            }

            List<Integer> sortedForbidden = new ArrayList<>(forbidden);
            Collections.sort(sortedForbidden);

            boolean hasConsecutive = false;
            for (int i = 0; i < sortedForbidden.size() - 1; i++) {
                if (sortedForbidden.get(i + 1) - sortedForbidden.get(i) == 1) {
                    hasConsecutive = true;
                    break;
                }
            }

            if (hasConsecutive) {
                System.out.println("Увага: є послідовні заборонені сходинки, що може унеможливити підйом!");
            }

            long ways = countWays(n, forbidden);
            System.out.println("Кількість способів дістатися вершини: " + ways);

            System.out.println("\nТестовий приклад (n=15, заборонені: 3, 7, 9, 12, 14):");
            Set<Integer> testForbidden = Set.of(3, 7, 9, 12, 14);
            long testWays = countWays(15, testForbidden);
            System.out.println("Кількість способів: " + testWays);
        }
    }

    public static class Task3MaxSubarray {

        public static class SubarrayResult {
            int maxSum;
            int start;
            int end;

            SubarrayResult(int sum, int start, int end) {
                this.maxSum = sum;
                this.start = start;
                this.end = end;
            }
        }

        public static SubarrayResult findMaxSubarray(int[] arr) {
            int maxSum = arr[0];
            int currentSum = arr[0];
            int start = 0, end = 0, tempStart = 0;

            for (int i = 1; i < arr.length; i++) {
                if (currentSum < 0) {
                    currentSum = arr[i];
                    tempStart = i;
                } else {
                    currentSum += arr[i];
                }

                if (currentSum > maxSum) {
                    maxSum = currentSum;
                    start = tempStart;
                    end = i;
                }
            }

            return new SubarrayResult(maxSum, start, end);
        }

        public static void solve() {
            System.out.println("\n=== Задача 3: Максимальна сума підмасиву ===");

            int[] testArray = {-2, 1, -3, 4, -1, 2, 1, -5, 4};

            System.out.println("Тестовий масив: " + Arrays.toString(testArray));

            SubarrayResult result = findMaxSubarray(testArray);

            System.out.println("Максимальна сума: " + result.maxSum);
            System.out.print("Підмасив: [");
            for (int i = result.start; i <= result.end; i++) {
                System.out.print(testArray[i]);
                if (i < result.end) System.out.print(", ");
            }
            System.out.println("]");
            System.out.println("Індекси: від " + result.start + " до " + result.end);

            System.out.println("\nДодаткові тести:");

            int[][] testCases = {
                    {5, -3, 5},
                    {-1, -2, -3, -4},
                    {1, 2, 3, 4, 5},
                    {-2, -1, -3}
            };

            for (int i = 0; i < testCases.length; i++) {
                System.out.println("Тест " + (i + 1) + ": " + Arrays.toString(testCases[i]));
                SubarrayResult testResult = findMaxSubarray(testCases[i]);
                System.out.println("Максимальна сума: " + testResult.maxSum);
                System.out.print("Підмасив: [");
                for (int j = testResult.start; j <= testResult.end; j++) {
                    System.out.print(testCases[i][j]);
                    if (j < testResult.end) System.out.print(", ");
                }
                System.out.println("]\n");
            }
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("=== Динамічне програмування ===");

        while (true) {
            System.out.println("\nВиберіть задачу:");
            System.out.println("1. Маршрут з мінімальною вартістю");
            System.out.println("2. Драбина з забороненими сходинками");
            System.out.println("3. Максимальна сума підмасиву");
            System.out.println("4. Показати всі задачі");
            System.out.println("5. Вийти");
            System.out.print("Ваш вибір: ");

            int choice = scanner.nextInt();

            switch (choice) {
                case 1:
                    Task1MinPathSum.solve();
                    break;
                case 2:
                    Task2StairClimbing.solve();
                    break;
                case 3:
                    Task3MaxSubarray.solve();
                    break;
                case 4:
                    Task1MinPathSum.solve();
                    Task2StairClimbing.solve();
                    Task3MaxSubarray.solve();
                    break;
                case 5:
                    System.out.println("До побачення!");
                    return;
                default:
                    System.out.println("Невірний вибір");
            }
        }
    }
}