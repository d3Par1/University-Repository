import java.util.*;

// Клас Book для зберігання інформації про книгу
class Book {
    private String title;
    private String author;
    private String publisher;
    private int year;
    private float price;

    public Book(String title, String author, String publisher, int year, float price) {
        this.title = title;
        this.author = author;
        this.publisher = publisher;
        this.year = year;
        this.price = price;
    }

    // Гетери
    public String getTitle() { return title; }
    public String getAuthor() { return author; }
    public String getPublisher() { return publisher; }
    public int getYear() { return year; }
    public float getPrice() { return price; }

    // Сетери
    public void setTitle(String title) { this.title = title; }
    public void setAuthor(String author) { this.author = author; }
    public void setPublisher(String publisher) { this.publisher = publisher; }
    public void setYear(int year) { this.year = year; }
    public void setPrice(float price) { this.price = price; }

    @Override
    public String toString() {
        return String.format("Назва: '%s', Автор: %s, Видавництво: %s, Рік: %d, Ціна: %.2f грн",
                title, author, publisher, year, price);
    }
}

// Система управління каталогом книг
class BookCatalogSystem {
    private HashMap<Integer, Book> bookCatalog;
    private Scanner scanner;
    private int nextISBN;

    public BookCatalogSystem() {
        this.bookCatalog = new HashMap<>();
        this.scanner = new Scanner(System.in);
        this.nextISBN = 1001;
        initializeBooks();
    }

    // Ініціалізація каталогу тестовими книгами
    private void initializeBooks() {
        bookCatalog.put(1001, new Book("Кобзар", "Тарас Шевченко", "Наукова думка", 2020, 250.0f));
        bookCatalog.put(1002, new Book("Лісова пісня", "Леся Українка", "Школа", 2019, 180.0f));
        bookCatalog.put(1003, new Book("Тіні забутих предків", "Михайло Коцюбинський", "Веселка", 2021, 320.0f));
        bookCatalog.put(1004, new Book("Енеїда", "Іван Котляревський", "Освіта", 2018, 290.0f));
        bookCatalog.put(1005, new Book("Маруся", "Григорій Квітка-Основ'яненко", "Школа", 2020, 200.0f));
        nextISBN = 1006;
    }

    public void start() {
        System.out.println("=== Каталог книг магазину ===");

        while (true) {
            displayMenu();
            int choice = getChoice();

            switch (choice) {
                case 1:
                    displayAllBooks();
                    break;
                case 2:
                    changeBookPrice();
                    break;
                case 3:
                    addBook();
                    break;
                case 4:
                    removeBook();
                    break;
                case 5:
                    sortBooksByPublisher();
                    break;
                case 6:
                    sortBooksByPrice();
                    break;
                case 7:
                    searchBook();
                    break;
                case 8:
                    System.out.println("До побачення!");
                    return;
                default:
                    System.out.println("Невірний вибір. Спробуйте ще раз.");
            }
            System.out.println();
        }
    }

    private void displayMenu() {
        System.out.println("\n1. Показати всі книги");
        System.out.println("2. Змінити ціну книги");
        System.out.println("3. Додати книгу");
        System.out.println("4. Видалити книгу");
        System.out.println("5. Сортувати за видавництвом");
        System.out.println("6. Сортувати за ціною");
        System.out.println("7. Пошук книги");
        System.out.println("8. Вийти");
        System.out.print("Виберіть опцію: ");
    }

    private int getChoice() {
        try {
            return scanner.nextInt();
        } catch (Exception e) {
            scanner.nextLine();
            return -1;
        }
    }

    private void displayAllBooks() {
        if (bookCatalog.isEmpty()) {
            System.out.println("Каталог порожній");
            return;
        }

        System.out.println("\n=== Каталог книг ===");
        for (Map.Entry<Integer, Book> entry : bookCatalog.entrySet()) {
            System.out.println("ISBN: " + entry.getKey() + " - " + entry.getValue());
        }
    }

    private void changeBookPrice() {
        displayAllBooks();
        if (bookCatalog.isEmpty()) return;

        System.out.print("Введіть ISBN книги для зміни ціни: ");
        int isbn = scanner.nextInt();

        if (!bookCatalog.containsKey(isbn)) {
            System.out.println("Книга з таким ISBN не знайдена");
            return;
        }

        Book book = bookCatalog.get(isbn);
        System.out.println("Поточна книга: " + book);
        System.out.print("Введіть нову ціну: ");
        float newPrice = scanner.nextFloat();

        book.setPrice(newPrice);
        System.out.println("Ціну змінено. Оновлена інформація: " + book);
    }

    private void addBook() {
        scanner.nextLine(); // очищення буфера

        System.out.print("Введіть назву книги: ");
        String title = scanner.nextLine();

        System.out.print("Введіть автора: ");
        String author = scanner.nextLine();

        System.out.print("Введіть видавництво: ");
        String publisher = scanner.nextLine();

        System.out.print("Введіть рік видання: ");
        int year = scanner.nextInt();

        System.out.print("Введіть ціну: ");
        float price = scanner.nextFloat();

        Book newBook = new Book(title, author, publisher, year, price);
        bookCatalog.put(nextISBN, newBook);

        System.out.println("Книгу додано з ISBN: " + nextISBN);
        System.out.println(newBook);
        nextISBN++;
    }

    private void removeBook() {
        displayAllBooks();
        if (bookCatalog.isEmpty()) return;

        System.out.print("Введіть ISBN книги для видалення: ");
        int isbn = scanner.nextInt();

        Book removed = bookCatalog.remove(isbn);
        if (removed != null) {
            System.out.println("Книгу видалено: " + removed);
        } else {
            System.out.println("Книга з таким ISBN не знайдена");
        }
    }

    private void sortBooksByPublisher() {
        if (bookCatalog.isEmpty()) {
            System.out.println("Каталог порожній");
            return;
        }

        // Створюємо ArrayList для сортування
        ArrayList<Map.Entry<Integer, Book>> bookList = new ArrayList<>(bookCatalog.entrySet());

        // Сортуємо за видавництвом
        bookList.sort(Comparator.comparing(entry -> entry.getValue().getPublisher()));

        System.out.println("\n=== Книги, відсортовані за видавництвом ===");
        for (Map.Entry<Integer, Book> entry : bookList) {
            System.out.println("ISBN: " + entry.getKey() + " - " + entry.getValue());
        }
    }

    private void sortBooksByPrice() {
        if (bookCatalog.isEmpty()) {
            System.out.println("Каталог порожній");
            return;
        }

        // Створюємо ArrayList для сортування
        ArrayList<Map.Entry<Integer, Book>> bookList = new ArrayList<>(bookCatalog.entrySet());

        System.out.println("Сортування за ціною:");
        System.out.println("1. За зростанням");
        System.out.println("2. За спаданням");
        System.out.print("Ваш вибір: ");
        int choice = scanner.nextInt();

        if (choice == 1) {
            bookList.sort(Comparator.comparing(entry -> entry.getValue().getPrice()));
            System.out.println("\n=== Книги, відсортовані за ціною (за зростанням) ===");
        } else if (choice == 2) {
            bookList.sort(Comparator.comparing((Map.Entry<Integer, Book> entry) -> entry.getValue().getPrice()).reversed());
            System.out.println("\n=== Книги, відсортовані за ціною (за спаданням) ===");
        } else {
            System.out.println("Невірний вибір");
            return;
        }

        for (Map.Entry<Integer, Book> entry : bookList) {
            System.out.println("ISBN: " + entry.getKey() + " - " + entry.getValue());
        }
    }

    private void searchBook() {
        scanner.nextLine(); // очищення буфера
        System.out.println("Пошук за:");
        System.out.println("1. Назвою");
        System.out.println("2. Автором");
        System.out.println("3. Видавництвом");
        System.out.print("Ваш вибір: ");

        int searchType = scanner.nextInt();
        scanner.nextLine(); // очищення буфера

        System.out.print("Введіть текст для пошуку: ");
        String searchTerm = scanner.nextLine().toLowerCase();

        System.out.println("\n=== Результати пошуку ===");
        boolean found = false;

        for (Map.Entry<Integer, Book> entry : bookCatalog.entrySet()) {
            Book book = entry.getValue();
            boolean matches = false;

            switch (searchType) {
                case 1:
                    matches = book.getTitle().toLowerCase().contains(searchTerm);
                    break;
                case 2:
                    matches = book.getAuthor().toLowerCase().contains(searchTerm);
                    break;
                case 3:
                    matches = book.getPublisher().toLowerCase().contains(searchTerm);
                    break;
            }

            if (matches) {
                System.out.println("ISBN: " + entry.getKey() + " - " + book);
                found = true;
            }
        }

        if (!found) {
            System.out.println("Книг не знайдено");
        }
    }

    // Додатковий метод для отримання статистики
    public void displayStatistics() {
        if (bookCatalog.isEmpty()) {
            System.out.println("Каталог порожній");
            return;
        }

        System.out.println("\n=== Статистика каталогу ===");
        System.out.println("Загальна кількість книг: " + bookCatalog.size());

        // Знаходимо найдорожчу і найдешевшу книги
        float maxPrice = Float.MIN_VALUE;
        float minPrice = Float.MAX_VALUE;
        Book mostExpensive = null;
        Book cheapest = null;
        float totalPrice = 0;

        for (Book book : bookCatalog.values()) {
            if (book.getPrice() > maxPrice) {
                maxPrice = book.getPrice();
                mostExpensive = book;
            }
            if (book.getPrice() < minPrice) {
                minPrice = book.getPrice();
                cheapest = book;
            }
            totalPrice += book.getPrice();
        }

        System.out.println("Найдорожча книга: " + mostExpensive);
        System.out.println("Найдешевша книга: " + cheapest);
        System.out.printf("Середня ціна: %.2f грн%n", totalPrice / bookCatalog.size());
    }
}

// Основний клас
public class BookCatalogApp {
    public static void main(String[] args) {
        BookCatalogSystem system = new BookCatalogSystem();
        system.start();
    }
}