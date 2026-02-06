import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

class Movie {
    private String title;
    private String director;
    private String genre;

    public Movie(String title, String director, String genre) {
        this.title = title;
        this.director = director;
        this.genre = genre;
    }

    public String getTitle() {
        return title;
    }

    public String getDirector() {
        return director;
    }

    public String getGenre() {
        return genre;
    }

    @Override
    public String toString() {
        return "Фільм: " + title + ", Режисер: " + director + ", Жанр: " + genre;
    }
}

class Cinema {
    private List<Movie> movies;

    public Cinema() {
        movies = new ArrayList<>();
    }

    public void addMovie(Movie movie) {
        movies.add(movie);
        System.out.println("Фільм успішно додано до списку.");
    }

    public void removeMovie(String title) {
        boolean removed = false;

        for (int i = 0; i < movies.size(); i++) {
            if (movies.get(i).getTitle().equalsIgnoreCase(title)) {
                movies.remove(i);
                removed = true;
                System.out.println("Фільм \"" + title + "\" успішно видалено.");
                break;
            }
        }

        if (!removed) {
            System.out.println("Фільм \"" + title + "\" не знайдено в списку.");
        }
    }

    public List<Movie> findMoviesByTitle(String title) {
        List<Movie> found = new ArrayList<>();

        for (Movie movie : movies) {
            if (movie.getTitle().toLowerCase().contains(title.toLowerCase())) {
                found.add(movie);
            }
        }

        return found;
    }

    public List<Movie> findMoviesByDirector(String director) {
        List<Movie> found = new ArrayList<>();

        for (Movie movie : movies) {
            if (movie.getDirector().toLowerCase().contains(director.toLowerCase())) {
                found.add(movie);
            }
        }

        return found;
    }

    public List<Movie> findMoviesByGenre(String genre) {
        List<Movie> found = new ArrayList<>();

        for (Movie movie : movies) {
            if (movie.getGenre().toLowerCase().contains(genre.toLowerCase())) {
                found.add(movie);
            }
        }

        return found;
    }

    public void displayAllMovies() {
        if (movies.isEmpty()) {
            System.out.println("Список фільмів порожній.");
        } else {
            System.out.println("Список усіх фільмів:");
            for (int i = 0; i < movies.size(); i++) {
                System.out.println((i + 1) + ". " + movies.get(i));
            }
        }
    }
}

public class CinemaApp {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Cinema cinema = new Cinema();

        cinema.addMovie(new Movie("Зелена миля", "Френк Дарабонт", "Драма"));
        cinema.addMovie(new Movie("Втеча з Шоушенка", "Френк Дарабонт", "Драма"));
        cinema.addMovie(new Movie("Хрещений батько", "Френсіс Форд Коппола", "Кримінал"));
        cinema.addMovie(new Movie("Темний лицар", "Крістофер Нолан", "Бойовик"));
        cinema.addMovie(new Movie("Інтерстеллар", "Крістофер Нолан", "Наукова фантастика"));

        int choice;
        do {
            System.out.println("\n==== Меню кінотеатру ====");
            System.out.println("1. Додати новий фільм");
            System.out.println("2. Видалити фільм");
            System.out.println("3. Знайти фільми за назвою");
            System.out.println("4. Знайти фільми за режисером");
            System.out.println("5. Знайти фільми за жанром");
            System.out.println("6. Показати всі фільми");
            System.out.println("0. Вихід");
            System.out.print("Виберіть опцію: ");

            choice = scanner.nextInt();
            scanner.nextLine();

            switch (choice) {
                case 1:
                    System.out.print("Введіть назву фільму: ");
                    String title = scanner.nextLine();
                    System.out.print("Введіть режисера: ");
                    String director = scanner.nextLine();
                    System.out.print("Введіть жанр: ");
                    String genre = scanner.nextLine();

                    cinema.addMovie(new Movie(title, director, genre));
                    break;

                case 2:
                    System.out.print("Введіть назву фільму для видалення: ");
                    String titleToRemove = scanner.nextLine();
                    cinema.removeMovie(titleToRemove);
                    break;

                case 3:
                    System.out.print("Введіть назву фільму для пошуку: ");
                    String searchTitle = scanner.nextLine();
                    List<Movie> foundByTitle = cinema.findMoviesByTitle(searchTitle);

                    if (foundByTitle.isEmpty()) {
                        System.out.println("Фільмів з такою назвою не знайдено.");
                    } else {
                        System.out.println("Знайдені фільми:");
                        for (Movie movie : foundByTitle) {
                            System.out.println(movie);
                        }
                    }
                    break;

                case 4:
                    System.out.print("Введіть режисера для пошуку: ");
                    String searchDirector = scanner.nextLine();
                    List<Movie> foundByDirector = cinema.findMoviesByDirector(searchDirector);

                    if (foundByDirector.isEmpty()) {
                        System.out.println("Фільмів цього режисера не знайдено.");
                    } else {
                        System.out.println("Знайдені фільми:");
                        for (Movie movie : foundByDirector) {
                            System.out.println(movie);
                        }
                    }
                    break;

                case 5:
                    System.out.print("Введіть жанр для пошуку: ");
                    String searchGenre = scanner.nextLine();
                    List<Movie> foundByGenre = cinema.findMoviesByGenre(searchGenre);

                    if (foundByGenre.isEmpty()) {
                        System.out.println("Фільмів цього жанру не знайдено.");
                    } else {
                        System.out.println("Знайдені фільми:");
                        for (Movie movie : foundByGenre) {
                            System.out.println(movie);
                        }
                    }
                    break;

                case 6:
                    cinema.displayAllMovies();
                    break;

                case 0:
                    System.out.println("Дякуємо за використання програми кінотеатру!");
                    break;

                default:
                    System.out.println("Невірний вибір. Спробуйте ще раз.");
            }

        } while (choice != 0);

        scanner.close();
    }
}
