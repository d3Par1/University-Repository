import java.util.Scanner;

public class Lr2_1 {
    enum GameDifficulty {
        EASY("Легкий рівень - для початківців"),
        MEDIUM("Середній рівень - для досвідчених гравців"),
        HARD("Складний рівень - для експертів");

        private final String description;

        GameDifficulty(String description) {
            this.description = description;
        }

        public String getDescription() {
            return description;
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.println("Оцініть ваш досвід у відеоіграх:");
        System.out.println("1 - Я новачок / рідко граю");
        System.out.println("2 - Маю середній досвід / граю регулярно");
        System.out.println("3 - Я експерт / граю дуже часто");
        System.out.print("Ваш вибір (1-3): ");

        int experienceLevel = scanner.nextInt();
        GameDifficulty selectedDifficulty;

        switch (experienceLevel) {
            case 1:
                selectedDifficulty = GameDifficulty.EASY;
                break;
            case 2:
                selectedDifficulty = GameDifficulty.MEDIUM;
                break;
            case 3:
                selectedDifficulty = GameDifficulty.HARD;
                break;
            default:
                System.out.println("Невірний вибір. Встановлено рівень за замовчуванням (MEDIUM).");
                selectedDifficulty = GameDifficulty.MEDIUM;
                break;
        }

        System.out.println("\nДля вас встановлено: " + selectedDifficulty);
        System.out.println(selectedDifficulty.getDescription());

        scanner.close();
    }
}
