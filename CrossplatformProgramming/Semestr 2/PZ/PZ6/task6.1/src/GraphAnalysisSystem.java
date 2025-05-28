import java.util.*;

// Клас для роботи з неорієнтованими графами
class UndirectedGraph {
    private int[][] adjacencyMatrix;
    private int vertices;
    private Random random;

    public UndirectedGraph(int vertices) {
        this.vertices = vertices;
        this.adjacencyMatrix = new int[vertices][vertices];
        this.random = new Random();
        generateRandomGraph();
    }

    // Конструктор з існуючою матрицею суміжності
    public UndirectedGraph(int[][] matrix) {
        this.vertices = matrix.length;
        this.adjacencyMatrix = new int[vertices][vertices];
        for (int i = 0; i < vertices; i++) {
            System.arraycopy(matrix[i], 0, this.adjacencyMatrix[i], 0, vertices);
        }
    }

    // Генерація випадкового графа
    private void generateRandomGraph() {
        // Генеруємо симетричну матрицю для неорієнтованого графа
        for (int i = 0; i < vertices; i++) {
            for (int j = i + 1; j < vertices; j++) {
                // Ймовірність створення ребра ~30%
                if (random.nextDouble() < 0.3) {
                    adjacencyMatrix[i][j] = 1;
                    adjacencyMatrix[j][i] = 1;
                }
            }
        }

        // Забезпечуємо зв'язність графа
        ensureConnectivity();
    }

    // Забезпечення зв'язності графа
    private void ensureConnectivity() {
        boolean[] visited = new boolean[vertices];
        List<Integer> component = new ArrayList<>();

        // Знаходимо першу компоненту зв'язності
        dfs(0, visited, component);

        // Якщо не всі вершини відвідані, з'єднуємо компоненти
        for (int i = 0; i < vertices; i++) {
            if (!visited[i]) {
                adjacencyMatrix[component.get(0)][i] = 1;
                adjacencyMatrix[i][component.get(0)] = 1;
                component.clear();
                dfs(i, visited, component);
            }
        }
    }

    // Глибокий пошук для знаходження компонент зв'язності
    private void dfs(int vertex, boolean[] visited, List<Integer> component) {
        visited[vertex] = true;
        component.add(vertex);

        for (int i = 0; i < vertices; i++) {
            if (adjacencyMatrix[vertex][i] == 1 && !visited[i]) {
                dfs(i, visited, component);
            }
        }
    }

    // Обчислення степеня вершини
    public int getDegree(int vertex) {
        int degree = 0;
        for (int i = 0; i < vertices; i++) {
            degree += adjacencyMatrix[vertex][i];
        }
        return degree;
    }

    // Знаходження вершини з мінімальним степенем
    public int getMinDegreeVertex() {
        int minDegree = Integer.MAX_VALUE;
        int minVertex = 0;

        for (int i = 0; i < vertices; i++) {
            int degree = getDegree(i);
            if (degree < minDegree) {
                minDegree = degree;
                minVertex = i;
            }
        }
        return minVertex;
    }

    // Знаходження вершини з максимальним степенем
    public int getMaxDegreeVertex() {
        int maxDegree = -1;
        int maxVertex = 0;

        for (int i = 0; i < vertices; i++) {
            int degree = getDegree(i);
            if (degree > maxDegree) {
                maxDegree = degree;
                maxVertex = i;
            }
        }
        return maxVertex;
    }

    // Алгоритм Дейкстри для знаходження найкоротшого шляху
    public List<Integer> shortestPath(int start, int end) {
        int[] distance = new int[vertices];
        int[] previous = new int[vertices];
        boolean[] visited = new boolean[vertices];

        Arrays.fill(distance, Integer.MAX_VALUE);
        Arrays.fill(previous, -1);
        distance[start] = 0;

        for (int i = 0; i < vertices; i++) {
            int u = -1;
            for (int v = 0; v < vertices; v++) {
                if (!visited[v] && (u == -1 || distance[v] < distance[u])) {
                    u = v;
                }
            }

            if (distance[u] == Integer.MAX_VALUE) break;
            visited[u] = true;

            for (int v = 0; v < vertices; v++) {
                if (adjacencyMatrix[u][v] == 1 && !visited[v]) {
                    int alt = distance[u] + 1;
                    if (alt < distance[v]) {
                        distance[v] = alt;
                        previous[v] = u;
                    }
                }
            }
        }

        // Відновлення шляху
        List<Integer> path = new ArrayList<>();
        if (distance[end] == Integer.MAX_VALUE) {
            return path; // Шлях не існує
        }

        int current = end;
        while (current != -1) {
            path.add(0, current);
            current = previous[current];
        }

        return path;
    }

    // Знаходження найдовшого шляху (в неорієнтованому графі це складна задача)
    public List<Integer> longestPath(int start, int end) {
        // Для простоти використовуємо DFS для знаходження найдовшого простого шляху
        List<Integer> longestPath = new ArrayList<>();
        boolean[] visited = new boolean[vertices];
        List<Integer> currentPath = new ArrayList<>();

        findLongestPathDFS(start, end, visited, currentPath, longestPath);
        return longestPath;
    }

    private void findLongestPathDFS(int current, int target, boolean[] visited,
                                    List<Integer> currentPath, List<Integer> longestPath) {
        visited[current] = true;
        currentPath.add(current);

        if (current == target) {
            if (currentPath.size() > longestPath.size()) {
                longestPath.clear();
                longestPath.addAll(currentPath);
            }
        } else {
            for (int i = 0; i < vertices; i++) {
                if (adjacencyMatrix[current][i] == 1 && !visited[i]) {
                    findLongestPathDFS(i, target, visited, currentPath, longestPath);
                }
            }
        }

        visited[current] = false;
        currentPath.remove(currentPath.size() - 1);
    }

    // Знаходження мінімального перерізу (спрощена версія)
    public int findMinCut() {
        int minCut = Integer.MAX_VALUE;

        // Спробуємо всі можливі пари вершин як джерело і стік
        for (int source = 0; source < vertices; source++) {
            for (int sink = source + 1; sink < vertices; sink++) {
                int cut = maxFlow(source, sink);
                if (cut < minCut) {
                    minCut = cut;
                }
            }
        }

        return minCut;
    }

    // Алгоритм Форда-Фалкерсона для знаходження максимального потоку
    private int maxFlow(int source, int sink) {
        int[][] capacity = new int[vertices][vertices];

        // Копіюємо матрицю суміжності як матрицю пропускних здатностей
        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                capacity[i][j] = adjacencyMatrix[i][j];
            }
        }

        int maxFlow = 0;
        int[] parent = new int[vertices];

        while (bfs(capacity, source, sink, parent)) {
            int pathFlow = Integer.MAX_VALUE;

            // Знаходимо мінімальну пропускну здатність на шляху
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                pathFlow = Math.min(pathFlow, capacity[u][v]);
            }

            // Оновлюємо пропускні здатності
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                capacity[u][v] -= pathFlow;
                capacity[v][u] += pathFlow;
            }

            maxFlow += pathFlow;
        }

        return maxFlow;
    }

    // BFS для знаходження шляху в залишковому графі
    private boolean bfs(int[][] capacity, int source, int sink, int[] parent) {
        boolean[] visited = new boolean[vertices];
        Queue<Integer> queue = new LinkedList<>();

        queue.offer(source);
        visited[source] = true;
        parent[source] = -1;

        while (!queue.isEmpty()) {
            int u = queue.poll();

            for (int v = 0; v < vertices; v++) {
                if (!visited[v] && capacity[u][v] > 0) {
                    queue.offer(v);
                    parent[v] = u;
                    visited[v] = true;
                    if (v == sink) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // Виведення матриці суміжності
    public void printAdjacencyMatrix() {
        System.out.println("Матриця суміжності:");
        System.out.print("   ");
        for (int i = 0; i < vertices; i++) {
            System.out.printf("%3d", i);
        }
        System.out.println();

        for (int i = 0; i < vertices; i++) {
            System.out.printf("%3d", i);
            for (int j = 0; j < vertices; j++) {
                System.out.printf("%3d", adjacencyMatrix[i][j]);
            }
            System.out.println();
        }
    }

    public int getVertices() {
        return vertices;
    }
}

// Основний клас для тестування
public class GraphAnalysisSystem {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("=== Система аналізу графів ===");
        System.out.print("Введіть кількість вершин графа (або 0 для використання тестового графа): ");
        int vertices = scanner.nextInt();

        UndirectedGraph graph;

        if (vertices == 0) {
            // Створення тестового графа
            int[][] testMatrix = {
                    {0, 1, 1, 0, 0},
                    {1, 0, 1, 1, 0},
                    {1, 1, 0, 1, 1},
                    {0, 1, 1, 0, 1},
                    {0, 0, 1, 1, 0}
            };
            graph = new UndirectedGraph(testMatrix);
            System.out.println("Використовується тестовий граф з 5 вершинами");
        } else {
            graph = new UndirectedGraph(vertices);
            System.out.println("Згенеровано випадковий граф з " + vertices + " вершинами");
        }

        // Виведення матриці суміжності
        graph.printAdjacencyMatrix();

        while (true) {
            System.out.println("\n=== Меню ===");
            System.out.println("1. Знайти вершину з мінімальним степенем");
            System.out.println("2. Знайти вершину з максимальним степенем");
            System.out.println("3. Знайти найкоротший шлях між вершинами");
            System.out.println("4. Знайти найдовший шлях між вершинами");
            System.out.println("5. Знайти мінімальний переріз");
            System.out.println("6. Показати степені всіх вершин");
            System.out.println("7. Згенерувати новий граф");
            System.out.println("8. Вийти");
            System.out.print("Виберіть опцію: ");

            int choice = scanner.nextInt();

            switch (choice) {
                case 1:
                    int minVertex = graph.getMinDegreeVertex();
                    System.out.printf("Вершина з мінімальним степенем: %d (степінь: %d)%n",
                            minVertex, graph.getDegree(minVertex));
                    break;

                case 2:
                    int maxVertex = graph.getMaxDegreeVertex();
                    System.out.printf("Вершина з максимальним степенем: %d (степінь: %d)%n",
                            maxVertex, graph.getDegree(maxVertex));
                    break;

                case 3:
                    System.out.print("Введіть початкову вершину: ");
                    int start = scanner.nextInt();
                    System.out.print("Введіть кінцеву вершину: ");
                    int end = scanner.nextInt();

                    if (start >= 0 && start < graph.getVertices() &&
                            end >= 0 && end < graph.getVertices()) {
                        List<Integer> shortPath = graph.shortestPath(start, end);
                        if (shortPath.isEmpty()) {
                            System.out.println("Шлях не існує");
                        } else {
                            System.out.println("Найкоротший шлях: " + shortPath);
                            System.out.println("Довжина шляху: " + (shortPath.size() - 1));
                        }
                    } else {
                        System.out.println("Невірні номери вершин");
                    }
                    break;

                case 4:
                    System.out.print("Введіть початкову вершину: ");
                    start = scanner.nextInt();
                    System.out.print("Введіть кінцеву вершину: ");
                    end = scanner.nextInt();

                    if (start >= 0 && start < graph.getVertices() &&
                            end >= 0 && end < graph.getVertices()) {
                        List<Integer> longPath = graph.longestPath(start, end);
                        if (longPath.isEmpty()) {
                            System.out.println("Шлях не існує");
                        } else {
                            System.out.println("Найдовший простий шлях: " + longPath);
                            System.out.println("Довжина шляху: " + (longPath.size() - 1));
                        }
                    } else {
                        System.out.println("Невірні номери вершин");
                    }
                    break;

                case 5:
                    int minCut = graph.findMinCut();
                    System.out.println("Мінімальний переріз: " + minCut + " ребер");
                    break;

                case 6:
                    System.out.println("Степені вершин:");
                    for (int i = 0; i < graph.getVertices(); i++) {
                        System.out.printf("Вершина %d: степінь %d%n", i, graph.getDegree(i));
                    }
                    break;

                case 7:
                    System.out.print("Введіть кількість вершин нового графа: ");
                    int newVertices = scanner.nextInt();
                    graph = new UndirectedGraph(newVertices);
                    System.out.println("Новий граф згенеровано");
                    graph.printAdjacencyMatrix();
                    break;

                case 8:
                    System.out.println("До побачення!");
                    return;

                default:
                    System.out.println("Невірний вибір");
            }
        }
    }
}