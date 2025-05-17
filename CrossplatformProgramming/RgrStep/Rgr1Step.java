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
            // Розглядаємо лише ті вершини, які ще не включені в MST
            for (int v = 0; v < V; v++) {
                // graph[u][v] не дорівнює нулю лише для суміжних вершин
                // mstSet[v] є false для вершин ще не включених у MST
                // Оновлюємо ключ лише якщо graph[u][v] менше за key[v]
                if (graph[u][v] != 0 && !mstSet[v] && graph[u][v] < key[v]) {
                    parent[v] = u;
                    key[v] = graph[u][v];
                }
            }
        }
        
        // Перевіряємо, чи MST містить усі вершини (граф є зв'язним)
        boolean connected = true;
        for (int i = 0; i < V; i++) {
            if (parent[i] == -1 && i != 0) {
                connected = false;
                break;
            }
        }
        
        if (connected) {
            System.out.println("Граф є зв'язним.");
            writer.println("Граф є зв'язним.");
        } else {
            System.out.println("Граф є незв'язним.");
            writer.println("Граф є незв'язним.");
            
            // Визначаємо компоненти зв'язності
            boolean[] visited = new boolean[V];
            int components = 0;
            
            for (int i = 0; i < V; i++) {
                if (!visited[i]) {
                    components++;
                    System.out.println("Компонента " + components + ":");
                    writer.println("Компонента " + components + ":");
                    
                    // Відображаємо вершини в цій компоненті
                    StringBuilder verticesInComponent = new StringBuilder();
                    for (int j = 0; j < V; j++) {
                        if (mstSet[j] && (j == i || parent[j] == i || parent[i] == j)) {
                            visited[j] = true;
                            verticesInComponent.append(j).append(" ");
                        }
                    }
                    
                    System.out.println("Вершини: " + verticesInComponent.toString());
                    writer.println("Вершини: " + verticesInComponent.toString());
                }
            }
        }
        
        // Виводимо побудований MST
        printMST(parent, graph, writer);
    }
    
    // Функція для генерації випадкового графа
    private static int[][] generateRandomGraph() {
        int[][] graph = new int[V][V];
        Random rand = new Random();
        
        int edgeCount = 0;
        while (edgeCount < 12) {
            int i = rand.nextInt(V);
            int j = rand.nextInt(V);
            if (i != j && graph[i][j] == 0) {
                int weight = rand.nextInt(10) + 1;
                graph[i][j] = weight;
                graph[j][i] = weight; // Неорієнтований граф
                edgeCount++;
            }
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
        
        // Створюємо дві окремі компоненти
        // Перша компонента: вершини 0-3
        for (int i = 0; i < 3; i++) {
            for (int j = i + 1; j < 4; j++) {
                if (rand.nextBoolean()) {
                    int weight = rand.nextInt(10) + 1;
                    graph[i][j] = weight;
                    graph[j][i] = weight;
                }
            }
        }
        
        // Друга компонента: вершини 4-7
        for (int i = 4; i < 7; i++) {
            for (int j = i + 1; j < 8; j++) {
                if (rand.nextBoolean()) {
                    int weight = rand.nextInt(10) + 1;
                    graph[i][j] = weight;
                    graph[j][i] = weight;
                }
            }
        }
        
        // Забезпечуємо зв'язність всередині компонент
        if (graph[0][1] == 0) {
            graph[0][1] = rand.nextInt(10) + 1;
            graph[1][0] = graph[0][1];
        }
        if (graph[0][2] == 0) {
            graph[0][2] = rand.nextInt(10) + 1;
            graph[2][0] = graph[0][2];
        }
        if (graph[0][3] == 0) {
            graph[0][3] = rand.nextInt(10) + 1;
            graph[3][0] = graph[0][3];
        }
        
        if (graph[4][5] == 0) {
            graph[4][5] = rand.nextInt(10) + 1;
            graph[5][4] = graph[4][5];
        }
        if (graph[4][6] == 0) {
            graph[4][6] = rand.nextInt(10) + 1;
            graph[6][4] = graph[4][6];
        }
        if (graph[4][7] == 0) {
            graph[4][7] = rand.nextInt(10) + 1;
            graph[7][4] = graph[4][7];
        }
        
        // Додаємо решту ребер до 12 загалом
        int edgeCount = 0;
        for (int i = 0; i < V; i++) {
            for (int j = i + 1; j < V; j++) {
                if (graph[i][j] > 0) {
                    edgeCount++;
                }
            }
        }
        
        // Додаємо ребра в межах компонент, а не між ними
        while (edgeCount < 12) {
            int component = rand.nextInt(2);
            int i, j;
            if (component == 0) {
                i = rand.nextInt(4);
                j = rand.nextInt(4);
            } else {
                i = 4 + rand.nextInt(4);
                j = 4 + rand.nextInt(4);
            }
            
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