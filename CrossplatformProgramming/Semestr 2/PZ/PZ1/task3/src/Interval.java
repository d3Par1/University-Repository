public class Interval {
    private double leftBound;
    private double rightBound;

    // Конструктор з параметрами
    public Interval(double leftBound, double rightBound) {
        // Перевірка коректності інтервалу
        if (leftBound > rightBound) {
            // Якщо ліва межа більша за праву, міняємо їх місцями
            this.leftBound = rightBound;
            this.rightBound = leftBound;
        } else {
            this.leftBound = leftBound;
            this.rightBound = rightBound;
        }
    }

    // Конструктор без параметрів (інтервал [0, 0])
    public Interval() {
        this(0, 0);
    }

    // Гетери та сетери
    public double getLeftBound() {
        return leftBound;
    }

    public void setLeftBound(double leftBound) {
        if (leftBound <= rightBound) {
            this.leftBound = leftBound;
        } else {
            this.leftBound = rightBound;
            this.rightBound = leftBound;
        }
    }

    public double getRightBound() {
        return rightBound;
    }

    public void setRightBound(double rightBound) {
        if (rightBound >= leftBound) {
            this.rightBound = rightBound;
        } else {
            this.rightBound = leftBound;
            this.leftBound = rightBound;
        }
    }

    // Метод для обчислення довжини інтервалу
    public double length() {
        return rightBound - leftBound;
    }

    // Метод для зміщення інтервалу вліво на задану величину
    public void shiftLeft(double value) {
        leftBound -= value;
        rightBound -= value;
    }

    // Метод для зміщення інтервалу вправо на задану величину
    public void shiftRight(double value) {
        leftBound += value;
        rightBound += value;
    }

    // Метод для стиснення/розтягнення інтервалу на заданий коефіцієнт
    public void scale(double coefficient) {
        if (coefficient <= 0) {
            System.out.println("Коефіцієнт має бути додатнім числом.");
            return;
        }

        double center = (leftBound + rightBound) / 2;
        double halfLength = length() / 2;

        // Розрахунок нових меж з урахуванням коефіцієнта
        leftBound = center - (halfLength * coefficient);
        rightBound = center + (halfLength * coefficient);
    }

    // Метод для порівняння двох інтервалів
    public static int compare(Interval interval1, Interval interval2) {
        double length1 = interval1.length();
        double length2 = interval2.length();

        if (length1 < length2) {
            return -1; // перший інтервал менший
        } else if (length1 > length2) {
            return 1;  // перший інтервал більший
        } else {
            return 0;  // інтервали рівні
        }
    }

    // Метод для обчислення суми двох інтервалів (об'єднання)
    public static Interval sum(Interval interval1, Interval interval2) {
        double newLeft = Math.min(interval1.leftBound, interval2.leftBound);
        double newRight = Math.max(interval1.rightBound, interval2.rightBound);
        return new Interval(newLeft, newRight);
    }

    // Метод для обчислення різниці двох інтервалів (перетин)
    public static Interval difference(Interval interval1, Interval interval2) {
        double newLeft = Math.max(interval1.leftBound, interval2.leftBound);
        double newRight = Math.min(interval1.rightBound, interval2.rightBound);

        // Якщо інтервали не перетинаються, повертаємо нульовий інтервал
        if (newLeft > newRight) {
            return new Interval(0, 0);
        }

        return new Interval(newLeft, newRight);
    }

    // Метод для виведення інтервалу
    @Override
    public String toString() {
        return "[" + leftBound + ", " + rightBound + "]";
    }

    // Демонстрація роботи класу
    public static void main(String[] args) {
        // Створення інтервалів
        Interval interval1 = new Interval(2, 8);
        Interval interval2 = new Interval(5, 12);
        Interval interval3 = new Interval(); // [0, 0]
        Interval interval4 = new Interval(10, 3); // Буде перетворено на [3, 10]

        // Виведення початкових інтервалів
        System.out.println("Початкові інтервали:");
        System.out.println("Інтервал 1: " + interval1);
        System.out.println("Інтервал 2: " + interval2);
        System.out.println("Інтервал 3: " + interval3);
        System.out.println("Інтервал 4: " + interval4);

        // Довжини інтервалів
        System.out.println("\nДовжини інтервалів:");
        System.out.println("Довжина інтервалу 1: " + interval1.length());
        System.out.println("Довжина інтервалу 2: " + interval2.length());
        System.out.println("Довжина інтервалу 4: " + interval4.length());

        // Зміщення інтервалу
        interval1.shiftRight(3);
        System.out.println("\nІнтервал 1 після зміщення вправо на 3: " + interval1);

        interval2.shiftLeft(2);
        System.out.println("Інтервал 2 після зміщення вліво на 2: " + interval2);

        // Стиснення/розтягнення інтервалу
        interval1.scale(0.5); // Стиснення в 2 рази
        System.out.println("\nІнтервал 1 після стиснення в 2 рази: " + interval1);

        interval2.scale(2); // Розтягнення в 2 рази
        System.out.println("Інтервал 2 після розтягнення в 2 рази: " + interval2);

        // Порівняння інтервалів
        System.out.println("\nПорівняння інтервалів:");
        int comparison = Interval.compare(interval1, interval2);
        if (comparison < 0) {
            System.out.println("Інтервал 1 менший за інтервал 2");
        } else if (comparison > 0) {
            System.out.println("Інтервал 1 більший за інтервал 2");
        } else {
            System.out.println("Інтервали 1 і 2 рівні");
        }

        // Сума (об'єднання) інтервалів
        Interval sumInterval = Interval.sum(interval1, interval2);
        System.out.println("\nСума інтервалів 1 і 2: " + sumInterval);

        // Різниця (перетин) інтервалів
        Interval diffInterval = Interval.difference(interval1, interval2);
        System.out.println("Різниця (перетин) інтервалів 1 і 2: " + diffInterval);

        // Тест з неперетинними інтервалами
        Interval interval5 = new Interval(1, 3);
        Interval interval6 = new Interval(7, 9);

        System.out.println("\nІнтервал 5: " + interval5);
        System.out.println("Інтервал 6: " + interval6);

        Interval diffInterval2 = Interval.difference(interval5, interval6);
        System.out.println("Різниця (перетин) інтервалів 5 і 6: " + diffInterval2);
    }
}