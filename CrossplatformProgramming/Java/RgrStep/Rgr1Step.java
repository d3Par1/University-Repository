import java.io.*;
import java.util.*;

public class Rgr1Step {
    // Кількість вершин у графі
    private static final int V = 8;

    // Функція для знаходження вершини з мінімальним значенням ключа
    // з множини вершин, ще не включених у MST
    private static int minKey(int[] key, boolean[] mstSet) {
        int min = Integer.MAX_VALUE, minIndex = -1;

        for (int v = 0; v < V; v++) {
            if (!mstSet[v] && key[v] < min) {
                min = key[v];
                minIndex = v;
            }
        }

        return minIndex;
    }

    // Функція для виведення побудованого MST, збереженого в parent[]
    private static void printMST(int[] parent, int[][] graph, PrintWriter writer) {
        System.out.println("Ребро \tВага");
        writer.println("Ребро \tВага");
        int totalWeight = 0;

        for (int i = 1; i < V; i++) {
            if (parent[i] != -1) {
                System.out.println(parent[i] + " - " + i + "\t" + graph[i][parent[i]]);
                writer.println(parent[i] + " - " + i + "\t" + graph[i][parent[i]]);
                totalWeight += graph[i][parent[i]];
            }
        }

        System.out.println("Загальна вага MST: " + totalWeight);
        writer.println("Загальна вага MST: " + totalWeight);
    }

    // Допоміжна функція для DFS обходу компоненти зв'язності
    private static void dfs(int[][] graph, int v, boolean[] visited, List<Integer> component) {
        visited[v] = true;
        component.add(v);

        for (int i = 0; i < V; i++) {
            if (graph[v][i] > 0 && !visited[i]) {
                dfs(graph, i, visited, component);
            }
        }
    }

    // Функція для знаходження компонент зв'язності
    private static List<List<Integer>> findConnectedComponents(int[][] graph) {
        List<List<Integer>> components = new ArrayList<>();
        boolean[] visited = new boolean[V];

        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                List<Integer> component = new ArrayList<>();
                dfs(graph, i, visited, component);
                components.add(component);
            }
        }

        return components;
    }

    // Функція для реалізації алгоритму Прима для MST
    private static void primMST(int[][] graph, PrintWriter writer) {
        // Масив для зберігання побудованого MST
        int[] parent = new int[V];

        // Значення ключів, що використовуються для вибору ребра з мінімальною вагою
        int[] key = new int[V];

        // Для представлення набору вершин, включених у MST
        boolean[] mstSet = new boolean[V];

        // Ініціалізуємо всі ключі як INFINITE
        for (int i = 0; i < V; i++) {
            key[i] = Integer.MAX_VALUE;
            mstSet[i] = false;
            parent[i] = -1;
        }

        // Перевіряємо, чи граф є зв'язним
        List<List<Integer>> components = findConnectedComponents(graph);

        if (components.size() == 1) {
            System.out.println("Граф є зв'язним.");
            writer.println("Граф є зв'язним.");

            // Завжди включаємо першу вершину в MST
            key[0] = 0; // Робимо ключ 0, щоб ця вершина була обрана першою
            parent[0] = -1; // Перша вершина завжди є коренем MST

            // MST матиме V вершин
            for (int count = 0; count < V - 1; count++) {
                // Вибираємо вершину з мінімальним ключем з множини вершин ще не включених у MST
                int u = minKey(key, mstSet);

                // Якщо жодна вершина не досяжна, то граф є незв'язним
                if (u == -1) {
                    break;
                }

                // Додаємо обрану вершину до множини MST
                mstSet[u] = true;

                // Оновлюємо значення ключа та індекс батьківської вершини для суміжних вершин
                for (int v = 0; v < V; v++) {
                    if (graph[u][v] != 0 && !mstSet[v] && graph[u][v] < key[v]) {
                        parent[v] = u;
                        key[v] = graph[u][v];
                    }
                }
            }

            // Виводимо побудований MST
            printMST(parent, graph, writer);
        } else {
            System.out.println("Граф є незв'язним.");
            writer.println("Граф є незв'язним.");

            System.out.println("Кількість компонент зв'язності: " + components.size());
            writer.println("Кількість компонент зв'язності: " + components.size());

            // Виводимо компоненти зв'язності
            for (int i = 0; i < components.size(); i++) {
                System.out.println("Компонента " + (i + 1) + ":");
                writer.println("Компонента " + (i + 1) + ":");

                System.out.print("Вершини: ");
                writer.print("Вершини: ");

                for (int vertex : components.get(i)) {
                    System.out.print(vertex + " ");
                    writer.print(vertex + " ");
                }
                System.out.println();
                writer.println();

                // Знаходимо MST для кожної компоненти окремо
                if (components.get(i).size() > 1) {
                    // Побудова MST для компоненти
                    buildComponentMST(graph, components.get(i), writer);
                }
            }
        }
    }

    // Функція для побудови MST для окремої компоненти
    private static void buildComponentMST(int[][] graph, List<Integer> component,
            PrintWriter writer) {
        int size = component.size();
        if (size <= 1)
            return;

        int[] parent = new int[V];
        int[] key = new int[V];
        boolean[] mstSet = new boolean[V];

        for (int i = 0; i < V; i++) {
            key[i] = Integer.MAX_VALUE;
            mstSet[i] = false;
            parent[i] = -1;
        }

        // Починаємо з першої вершини компоненти
        int startVertex = component.get(0);
        key[startVertex] = 0;

        for (int count = 0; count < size - 1; count++) {
            int u = -1;
            int minKey = Integer.MAX_VALUE;

            // Знаходимо вершину з мінімальним ключем з компоненти
            for (int v : component) {
                if (!mstSet[v] && key[v] < minKey) {
                    minKey = key[v];
                    u = v;
                }
            }

            if (u == -1)
                break;

            mstSet[u] = true;

            // Оновлюємо ключі для вершин, що належать цій компоненті
            for (int v : component) {
                if (graph[u][v] != 0 && !mstSet[v] && graph[u][v] < key[v]) {
                    parent[v] = u;
                    key[v] = graph[u][v];
                }
            }
        }

        // Виводимо MST для компоненти
        System.out.println("MST для компоненти:");
        writer.println("MST для компоненти:");

        int componentWeight = 0;
        for (int v : component) {
            if (parent[v] != -1) {
                System.out.println(parent[v] + " - " + v + "\t" + graph[v][parent[v]]);
                writer.println(parent[v] + " - " + v + "\t" + graph[v][parent[v]]);
                componentWeight += graph[v][parent[v]];
            }
        }

        System.out.println("Загальна вага компоненти MST: " + componentWeight);
        writer.println("Загальна вага компоненти MST: " + componentWeight);
    }

    // Функція для генерації випадкового графа
    private static int[][] generateRandomGraph() {
        int[][] graph = new int[V][V];
        Random rand = new Random();

        int edgeCount = 0;
        int maxAttempts = 100; // Запобігає нескінченному циклу
        int attempts = 0;

        while (edgeCount < 12 && attempts < maxAttempts) {
            int i = rand.nextInt(V);
            int j = rand.nextInt(V);
            if (i != j && graph[i][j] == 0) {
                int weight = rand.nextInt(10) + 1;
                graph[i][j] = weight;
                graph[j][i] = weight; // Неорієнтований граф
                edgeCount++;
            }
            attempts++;
        }

        return graph;
    }

    // Функція для генерації випадкового зв'язного графа
    private static int[][] generateConnectedGraph() {
        int[][] graph = new int[V][V];
        Random rand = new Random();

        // Створюємо остовне дерево для забезпечення зв'язності
        for (int i = 1; i < V; i++) {
            int j = rand.nextInt(i); // З'єднуємо з будь-якою попередньою вершиною
            int weight = rand.nextInt(10) + 1; // Випадкова вага від 1 до 10
            graph[i][j] = weight;
            graph[j][i] = weight; // Неорієнтований граф
        }

        // Додаємо решту ребер, щоб досягти загальної кількості 12
        int edgeCount = V - 1; // Вже додано V-1 ребер для остовного дерева
        while (edgeCount < 12) {
            int i = rand.nextInt(V);
            int j = rand.nextInt(V);
            if (i != j && graph[i][j] == 0) {
                int weight = rand.nextInt(10) + 1;
                graph[i][j] = weight;
                graph[j][i] = weight;
                edgeCount++;
            }
        }

        return graph;
    }

    // Функція для генерації незв'язного графа
    private static int[][] generateDisconnectedGraph() {
        int[][] graph = new int[V][V];
        Random rand = new Random();

        // Забезпечуємо зв'язність всередині компонент
        // Перша компонента (0-3)
        for (int i = 1; i < 4; i++) {
            int j = rand.nextInt(i);
            int weight = rand.nextInt(10) + 1;
            graph[i][j] = weight;
            graph[j][i] = weight;
        }

        // Друга компонента (4-7)
        for (int i = 5; i < 8; i++) {
            int j = 4 + rand.nextInt(i - 4);
            int weight = rand.nextInt(10) + 1;
            graph[i][j] = weight;
            graph[j][i] = weight;
        }

        // Підрахунок доданих ребер
        int edgeCount = 6; // 3 ребра в першій компоненті + 3 ребра в другій компоненті

        // Додавання решти ребер до досягнення 12
        while (edgeCount < 12) {
            // Випадково вибираємо, до якої компоненти додати ребро
            int component = rand.nextInt(2);
            int i, j;

            if (component == 0) {
                // Додаємо ребро до першої компоненти (0-3)
                i = rand.nextInt(4);
                j = rand.nextInt(4);
            } else {
                // Додаємо ребро до другої компоненти (4-7)
                i = 4 + rand.nextInt(4);
                j = 4 + rand.nextInt(4);
            }

            // Переконуємося, що не додаємо петлю або дублікат
            if (i != j && graph[i][j] == 0) {
                int weight = rand.nextInt(10) + 1;
                graph[i][j] = weight;
                graph[j][i] = weight;
                edgeCount++;
            }
        }

        return graph;
    }

    // Функція для виведення графа
    private static void printGraph(int[][] graph, PrintWriter writer) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                System.out.print(graph[i][j] + " ");
                writer.print(graph[i][j] + " ");
            }
            System.out.println();
            writer.println();
        }
    }

    // Функція для збереження графа у файл
    private static void saveGraphToFile(int[][] graph, String filename) {
        try (PrintWriter writer = new PrintWriter(new FileWriter(filename))) {
            for (int i = 0; i < V; i++) {
                for (int j = 0; j < V; j++) {
                    writer.print(graph[i][j] + " ");
                }
                writer.println();
            }
        } catch (IOException e) {
            System.err.println("Помилка при записі у файл: " + e.getMessage());
        }
    }

    // Функція для завантаження графа з файлу
    private static int[][] loadGraphFromFile(String filename) {
        int[][] graph = new int[V][V];
        try (Scanner scanner = new Scanner(new File(filename))) {
            for (int i = 0; i < V; i++) {
                for (int j = 0; j < V; j++) {
                    if (scanner.hasNextInt()) {
                        graph[i][j] = scanner.nextInt();
                    }
                }
            }
        } catch (FileNotFoundException e) {
            System.err.println("Файл не знайдено: " + e.getMessage());
        }
        return graph;
    }

    public static void main(String[] args) {
        try (PrintWriter resultWriter = new PrintWriter(new FileWriter("result1.txt"))) {
            // Тестовий випадок 1: Випадковий граф
            resultWriter.println("Тестовий випадок 1: Випадковий граф");
            System.out.println("Тестовий випадок 1: Випадковий граф");

            int[][] randomGraph = generateRandomGraph();
            saveGraphToFile(randomGraph, "graph1.txt");

            resultWriter.println("Матриця суміжності:");
            System.out.println("Матриця суміжності:");
            printGraph(randomGraph, resultWriter);

            resultWriter.println("Мінімальне остовне дерево (алгоритм Прима):");
            System.out.println("Мінімальне остовне дерево (алгоритм Прима):");
            primMST(randomGraph, resultWriter);

            // Тестовий випадок 2: Зв'язний граф
            resultWriter.println("\nТестовий випадок 2: Зв'язний граф");
            System.out.println("\nТестовий випадок 2: Зв'язний граф");

            int[][] connectedGraph = generateConnectedGraph();
            saveGraphToFile(connectedGraph, "graph2.txt");

            resultWriter.println("Матриця суміжності:");
            System.out.println("Матриця суміжності:");
            printGraph(connectedGraph, resultWriter);

            resultWriter.println("Мінімальне остовне дерево (алгоритм Прима):");
            System.out.println("Мінімальне остовне дерево (алгоритм Прима):");
            primMST(connectedGraph, resultWriter);

            // Тестовий випадок 3: Незв'язний граф
            resultWriter.println("\nТестовий випадок 3: Незв'язний граф");
            System.out.println("\nТестовий випадок 3: Незв'язний граф");

            int[][] disconnectedGraph = generateDisconnectedGraph();
            saveGraphToFile(disconnectedGraph, "graph3.txt");

            resultWriter.println("Матриця суміжності:");
            System.out.println("Матриця суміжності:");
            printGraph(disconnectedGraph, resultWriter);

            resultWriter.println("Мінімальне остовне дерево (алгоритм Прима):");
            System.out.println("Мінімальне остовне дерево (алгоритм Прима):");
            primMST(disconnectedGraph, resultWriter);

        } catch (IOException e) {
            System.err.println("Помилка при створенні файлу результатів: " + e.getMessage());
        }
    }
}
