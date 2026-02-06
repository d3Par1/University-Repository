import java.util.*;

// Клас Graph для роботи з графами через список ребер
public class Graph {
    private int[][] edges;      // Двовимірний масив для зберігання ребер
    private int vertexCount;    // Кількість вершин
    private int edgeCount;      // Кількість ребер
    private int maxEdges;       // Максимальна кількість ребер (розмір масиву)

    // Конструктор за замовчуванням
    public Graph() {
        this.maxEdges = 10;
        this.edges = new int[maxEdges][2];
        this.vertexCount = 0;
        this.edgeCount = 0;
    }

    // Конструктор, який приймає двовимірний масив ребер
    public Graph(int[][] edgeList) {
        this.edgeCount = edgeList.length;
        this.maxEdges = Math.max(edgeCount * 2, 10); // Запас для додавання нових ребер
        this.edges = new int[maxEdges][2];

        // Копіюємо ребра та знаходимо максимальну вершину
        int maxVertex = -1;
        for (int i = 0; i < edgeCount; i++) {
            this.edges[i][0] = edgeList[i][0];
            this.edges[i][1] = edgeList[i][1];
            maxVertex = Math.max(maxVertex, Math.max(edgeList[i][0], edgeList[i][1]));
        }
        this.vertexCount = maxVertex + 1;
    }

    // Метод для отримання матриці суміжності
    public int[][] getAdjacencyMatrix() {
        int[][] adjacencyMatrix = new int[vertexCount][vertexCount];

        for (int i = 0; i < edgeCount; i++) {
            int v1 = edges[i][0];
            int v2 = edges[i][1];
            adjacencyMatrix[v1][v2] = 1;
            adjacencyMatrix[v2][v1] = 1; // Для неорієнтованого графа
        }

        return adjacencyMatrix;
    }

    // Метод для додавання ребра
    public void addEdge(int vertex1, int vertex2) {
        // Перевіряємо, чи не існує вже таке ребро
        for (int i = 0; i < edgeCount; i++) {
            if ((edges[i][0] == vertex1 && edges[i][1] == vertex2) ||
                    (edges[i][0] == vertex2 && edges[i][1] == vertex1)) {
                System.out.println("Ребро між вершинами " + vertex1 + " і " + vertex2 + " вже існує");
                return;
            }
        }

        // Розширюємо масив при необхідності
        if (edgeCount >= maxEdges) {
            expandEdgesArray();
        }

        edges[edgeCount][0] = vertex1;
        edges[edgeCount][1] = vertex2;
        edgeCount++;

        // Оновлюємо кількість вершин
        vertexCount = Math.max(vertexCount, Math.max(vertex1, vertex2) + 1);

        System.out.println("Ребро між вершинами " + vertex1 + " і " + vertex2 + " додано");
    }

    // Метод для видалення ребра
    public void removeEdge(int vertex1, int vertex2) {
        for (int i = 0; i < edgeCount; i++) {
            if ((edges[i][0] == vertex1 && edges[i][1] == vertex2) ||
                    (edges[i][0] == vertex2 && edges[i][1] == vertex1)) {

                // Зміщуємо всі наступні ребра на одну позицію назад
                for (int j = i; j < edgeCount - 1; j++) {
                    edges[j][0] = edges[j + 1][0];
                    edges[j][1] = edges[j + 1][1];
                }
                edgeCount--;

                // Оновлюємо кількість вершин
                updateVertexCount();

                System.out.println("Ребро між вершинами " + vertex1 + " і " + vertex2 + " видалено");
                return;
            }
        }
        System.out.println("Ребро між вершинами " + vertex1 + " і " + vertex2 + " не знайдено");
    }

    // Метод для додавання вершини та з'єднання її з вершиною з найменшим степенем
    public void addVertex() {
        int newVertex = vertexCount;

        if (vertexCount == 0) {
            // Якщо граф порожній, просто додаємо вершину
            vertexCount = 1;
            System.out.println("Додано вершину " + newVertex + " (граф був порожній)");
            return;
        }

        // Знаходимо вершину з найменшим степенем
        int minDegreeVertex = findMinDegreeVertex();

        // Додаємо ребро між новою вершиною та вершиною з найменшим степенем
        addEdge(newVertex, minDegreeVertex);

        System.out.println("Додано вершину " + newVertex + " і з'єднано з вершиною " +
                minDegreeVertex + " (найменший степінь)");
    }

    // Метод для видалення вершини
    public void removeVertex(int vertex) {
        if (vertex < 0 || vertex >= vertexCount) {
            System.out.println("Вершина " + vertex + " не існує");
            return;
        }

        // Видаляємо всі ребра, що містять цю вершину
        for (int i = edgeCount - 1; i >= 0; i--) {
            if (edges[i][0] == vertex || edges[i][1] == vertex) {
                // Зміщуємо всі наступні ребра на одну позицію назад
                for (int j = i; j < edgeCount - 1; j++) {
                    edges[j][0] = edges[j + 1][0];
                    edges[j][1] = edges[j + 1][1];
                }
                edgeCount--;
            }
        }

        // Перенумеровуємо вершини (всі вершини з номерами більшими за видалену зменшуємо на 1)
        for (int i = 0; i < edgeCount; i++) {
            if (edges[i][0] > vertex) edges[i][0]--;
            if (edges[i][1] > vertex) edges[i][1]--;
        }

        vertexCount--;

        System.out.println("Вершину " + vertex + " та всі пов'язані з нею ребра видалено");
    }

    // Допоміжний метод для знаходження вершини з найменшим степенем
    private int findMinDegreeVertex() {
        int[] degrees = new int[vertexCount];

        // Обчислюємо степені всіх вершин
        for (int i = 0; i < edgeCount; i++) {
            degrees[edges[i][0]]++;
            degrees[edges[i][1]]++;
        }

        // Знаходимо вершину з найменшим степенем
        int minDegree = Integer.MAX_VALUE;
        int minVertex = 0;

        for (int i = 0; i < vertexCount; i++) {
            if (degrees[i] < minDegree) {
                minDegree = degrees[i];
                minVertex = i;
            }
        }

        return minVertex;
    }

    // Допоміжний метод для оновлення кількості вершин після видалення ребра
    private void updateVertexCount() {
        int maxVertex = -1;
        for (int i = 0; i < edgeCount; i++) {
            maxVertex = Math.max(maxVertex, Math.max(edges[i][0], edges[i][1]));
        }
        vertexCount = maxVertex + 1;
    }

    // Допоміжний метод для розширення масиву ребер
    private void expandEdgesArray() {
        maxEdges *= 2;
        int[][] newEdges = new int[maxEdges][2];
        for (int i = 0; i < edgeCount; i++) {
            newEdges[i][0] = edges[i][0];
            newEdges[i][1] = edges[i][1];
        }
        edges = newEdges;
    }

    // Метод для виведення інформації про граф
    public void printGraph() {
        System.out.println("=== Інформація про граф ===");
        System.out.println("Кількість вершин: " + vertexCount);
        System.out.println("Кількість ребер: " + edgeCount);

        if (edgeCount > 0) {
            System.out.println("Ребра:");
            for (int i = 0; i < edgeCount; i++) {
                System.out.println("  " + edges[i][0] + " -- " + edges[i][1]);
            }
        } else {
            System.out.println("Граф не містить ребер");
        }
    }

    // Метод для виведення матриці суміжності
    public void printAdjacencyMatrix() {
        int[][] matrix = getAdjacencyMatrix();

        System.out.println("Матриця суміжності:");
        System.out.print("   ");
        for (int i = 0; i < vertexCount; i++) {
            System.out.printf("%3d", i);
        }
        System.out.println();

        for (int i = 0; i < vertexCount; i++) {
            System.out.printf("%3d", i);
            for (int j = 0; j < vertexCount; j++) {
                System.out.printf("%3d", matrix[i][j]);
            }
            System.out.println();
        }
    }

    // Метод для обчислення степеня вершини
    public int getVertexDegree(int vertex) {
        if (vertex < 0 || vertex >= vertexCount) {
            return -1;
        }

        int degree = 0;
        for (int i = 0; i < edgeCount; i++) {
            if (edges[i][0] == vertex || edges[i][1] == vertex) {
                degree++;
            }
        }
        return degree;
    }

    // Метод для виведення степенів всіх вершин
    public void printVertexDegrees() {
        System.out.println("Степені вершин:");
        for (int i = 0; i < vertexCount; i++) {
            System.out.println("Вершина " + i + ": степінь " + getVertexDegree(i));
        }
    }

    // Гетери
    public int getVertexCount() { return vertexCount; }
    public int getEdgeCount() { return edgeCount; }
}

// Демонстраційна програма
class GraphDemo {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("=== Демонстрація роботи класу Graph ===");

        // Створення графа з тестовими даними
        int[][] testEdges = {
                {0, 1}, {1, 2}, {2, 3}, {0, 3}, {1, 3}
        };

        Graph graph = new Graph(testEdges);
        System.out.println("Створено граф з тестовими даними:");
        graph.printGraph();
        graph.printAdjacencyMatrix();
        graph.printVertexDegrees();

        while (true) {
            System.out.println("\n=== Меню ===");
            System.out.println("1. Показати граф");
            System.out.println("2. Показати матрицю суміжності");
            System.out.println("3. Додати ребро");
            System.out.println("4. Видалити ребро");
            System.out.println("5. Додати вершину");
            System.out.println("6. Видалити вершину");
            System.out.println("7. Показати степені вершин");
            System.out.println("8. Створити новий порожній граф");
            System.out.println("9. Вийти");
            System.out.print("Виберіть опцію: ");

            int choice = scanner.nextInt();

            switch (choice) {
                case 1:
                    graph.printGraph();
                    break;

                case 2:
                    if (graph.getVertexCount() > 0) {
                        graph.printAdjacencyMatrix();
                    } else {
                        System.out.println("Граф не містить вершин");
                    }
                    break;

                case 3:
                    System.out.print("Введіть першу вершину: ");
                    int v1 = scanner.nextInt();
                    System.out.print("Введіть другу вершину: ");
                    int v2 = scanner.nextInt();
                    graph.addEdge(v1, v2);
                    break;

                case 4:
                    System.out.print("Введіть першу вершину ребра для видалення: ");
                    v1 = scanner.nextInt();
                    System.out.print("Введіть другу вершину ребра для видалення: ");
                    v2 = scanner.nextInt();
                    graph.removeEdge(v1, v2);
                    break;

                case 5:
                    graph.addVertex();
                    break;

                case 6:
                    System.out.print("Введіть номер вершини для видалення: ");
                    int vertex = scanner.nextInt();
                    graph.removeVertex(vertex);
                    break;

                case 7:
                    if (graph.getVertexCount() > 0) {
                        graph.printVertexDegrees();
                    } else {
                        System.out.println("Граф не містить вершин");
                    }
                    break;

                case 8:
                    graph = new Graph();
                    System.out.println("Створено новий порожній граф");
                    break;

                case 9:
                    System.out.println("До побачення!");
                    return;

                default:
                    System.out.println("Невірний вибір");
            }
        }
    }
}