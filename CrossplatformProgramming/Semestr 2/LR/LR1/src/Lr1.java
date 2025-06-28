import java.util.Scanner;

// Клас для точки на площині
class Point {
    private double x, y;

    public Point(double x, double y) {
        this.x = x;
        this.y = y;
    }

    public double getX() { return x; }
    public double getY() { return y; }
    public void setX(double x) { this.x = x; }
    public void setY(double y) { this.y = y; }

    // Відстань між двома точками
    public double distanceTo(Point other) {
        return Math.sqrt(Math.pow(this.x - other.x, 2) + Math.pow(this.y - other.y, 2));
    }

    // Поворот точки відносно іншої точки на заданий кут
    public Point rotate(Point center, double angle) {
        double cos = Math.cos(Math.toRadians(angle));
        double sin = Math.sin(Math.toRadians(angle));

        double dx = this.x - center.x;
        double dy = this.y - center.y;

        double newX = center.x + dx * cos - dy * sin;
        double newY = center.y + dx * sin + dy * cos;

        return new Point(newX, newY);
    }

    @Override
    public String toString() {
        return String.format("(%.2f, %.2f)", x, y);
    }
}

// Клас для трикутника
class Triangle {
    private Point a, b, c;

    public Triangle(Point a, Point b, Point c) {
        this.a = a;
        this.b = b;
        this.c = c;
    }

    // Геттери та сеттери
    public Point getA() { return a; }
    public Point getB() { return b; }
    public Point getC() { return c; }
    public void setA(Point a) { this.a = a; }
    public void setB(Point b) { this.b = b; }
    public void setC(Point c) { this.c = c; }

    // Довжини сторін
    public double getSideA() { return b.distanceTo(c); }  // a - протилежна до вершини A
    public double getSideB() { return a.distanceTo(c); }  // b - протилежна до вершини B
    public double getSideC() { return a.distanceTo(b); }  // c - протилежна до вершини C

    // Площа трикутника (формула Герона)
    public double getArea() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();
        double s = (a + b + c) / 2; // напівпериметр
        return Math.sqrt(s * (s - a) * (s - b) * (s - c));
    }

    // Периметр
    public double getPerimeter() {
        return getSideA() + getSideB() + getSideC();
    }

    // Висоти
    public double getHeightToA() { return 2 * getArea() / getSideA(); }
    public double getHeightToB() { return 2 * getArea() / getSideB(); }
    public double getHeightToC() { return 2 * getArea() / getSideC(); }

    // Медіани
    public double getMedianToA() {
        double b = getSideB();
        double c = getSideC();
        double a = getSideA();
        return 0.5 * Math.sqrt(2 * b * b + 2 * c * c - a * a);
    }

    public double getMedianToB() {
        double a = getSideA();
        double c = getSideC();
        double b = getSideB();
        return 0.5 * Math.sqrt(2 * a * a + 2 * c * c - b * b);
    }

    public double getMedianToC() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();
        return 0.5 * Math.sqrt(2 * a * a + 2 * b * b - c * c);
    }

    // Бісектриси
    public double getBisectorToA() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();
        return Math.sqrt(b * c * (1 - Math.pow(a / (b + c), 2)));
    }

    public double getBisectorToB() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();
        return Math.sqrt(a * c * (1 - Math.pow(b / (a + c), 2)));
    }

    public double getBisectorToC() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();
        return Math.sqrt(a * b * (1 - Math.pow(c / (a + b), 2)));
    }

    // Радіус вписаного кола
    public double getInradius() {
        return getArea() / (getPerimeter() / 2);
    }

    // Радіус описаного кола
    public double getCircumradius() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();
        return (a * b * c) / (4 * getArea());
    }

    // Центр описаного кола
    public Point getCircumcenter() {
        double x1 = a.getX(), y1 = a.getY();
        double x2 = b.getX(), y2 = b.getY();
        double x3 = c.getX(), y3 = c.getY();

        double d = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));

        double ux = ((x1*x1 + y1*y1) * (y2 - y3) + (x2*x2 + y2*y2) * (y3 - y1) + (x3*x3 + y3*y3) * (y1 - y2)) / d;
        double uy = ((x1*x1 + y1*y1) * (x3 - x2) + (x2*x2 + y2*y2) * (x1 - x3) + (x3*x3 + y3*y3) * (x2 - x1)) / d;

        return new Point(ux, uy);
    }

    // Тип трикутника
    public String getTriangleType() {
        double a = getSideA();
        double b = getSideB();
        double c = getSideC();

        String result = "";

        // За сторонами
        if (Math.abs(a - b) < 0.001 && Math.abs(b - c) < 0.001) {
            result += "рівносторонній";
        } else if (Math.abs(a - b) < 0.001 || Math.abs(b - c) < 0.001 || Math.abs(a - c) < 0.001) {
            result += "рівнобедрений";
        } else {
            result += "різносторонній";
        }

        // За кутами
        double[] sides = {a, b, c};
        java.util.Arrays.sort(sides);
        double maxSide = sides[2];
        double sumSquares = sides[0] * sides[0] + sides[1] * sides[1];
        double maxSquare = maxSide * maxSide;

        if (Math.abs(sumSquares - maxSquare) < 0.001) {
            result += ", прямокутний";
        } else if (sumSquares > maxSquare) {
            result += ", гострокутний";
        } else {
            result += ", тупокутний";
        }

        return result;
    }

    // Поворот трикутника відносно вершини
    public Triangle rotateAroundVertex(char vertex, double angle) {
        Point center;
        switch (vertex) {
            case 'A': center = a; break;
            case 'B': center = b; break;
            case 'C': center = c; break;
            default: throw new IllegalArgumentException("Невірна вершина");
        }

        Point newA = a.rotate(center, angle);
        Point newB = b.rotate(center, angle);
        Point newC = c.rotate(center, angle);

        return new Triangle(newA, newB, newC);
    }

    // Поворот трикутника відносно центру описаного кола
    public Triangle rotateAroundCircumcenter(double angle) {
        Point center = getCircumcenter();

        Point newA = a.rotate(center, angle);
        Point newB = b.rotate(center, angle);
        Point newC = c.rotate(center, angle);

        return new Triangle(newA, newB, newC);
    }

    // Перевірка рівності двох трикутників
    public boolean equals(Triangle other) {
        double[] thisSides = {getSideA(), getSideB(), getSideC()};
        double[] otherSides = {other.getSideA(), other.getSideB(), other.getSideC()};

        java.util.Arrays.sort(thisSides);
        java.util.Arrays.sort(otherSides);

        for (int i = 0; i < 3; i++) {
            if (Math.abs(thisSides[i] - otherSides[i]) > 0.001) {
                return false;
            }
        }
        return true;
    }

    @Override
    public String toString() {
        return String.format("Трикутник: A%s, B%s, C%s", a, b, c);
    }
}

public class Lr1 {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        // Створення трьох різних трикутників для демонстрації
        Triangle[] triangles = {
            new Triangle(new Point(0, 0), new Point(3, 0), new Point(0, 4)),      // Прямокутний
            new Triangle(new Point(0, 0), new Point(2, 0), new Point(1, Math.sqrt(3))), // Рівносторонній
            new Triangle(new Point(0, 0), new Point(4, 0), new Point(2, 1))       // Довільний
        };

        String[] names = {"Прямокутний трикутник", "Рівносторонній трикутник", "Довільний трикутник"};

        for (int i = 0; i < triangles.length; i++) {
            System.out.println("=== " + names[i] + " ===");
            Triangle t = triangles[i];

            System.out.println(t);
            System.out.printf("Сторони: a=%.2f, b=%.2f, c=%.2f\n",
                t.getSideA(), t.getSideB(), t.getSideC());
            System.out.printf("Площа: %.2f\n", t.getArea());
            System.out.printf("Периметр: %.2f\n", t.getPerimeter());

            System.out.printf("Висоти: ha=%.2f, hb=%.2f, hc=%.2f\n",
                t.getHeightToA(), t.getHeightToB(), t.getHeightToC());
            System.out.printf("Медіани: ma=%.2f, mb=%.2f, mc=%.2f\n",
                t.getMedianToA(), t.getMedianToB(), t.getMedianToC());
            System.out.printf("Бісектриси: la=%.2f, lb=%.2f, lc=%.2f\n",
                t.getBisectorToA(), t.getBisectorToB(), t.getBisectorToC());

            System.out.printf("Радіус вписаного кола: %.2f\n", t.getInradius());
            System.out.printf("Радіус описаного кола: %.2f\n", t.getCircumradius());
            System.out.printf("Центр описаного кола: %s\n", t.getCircumcenter());

            System.out.println("Тип: " + t.getTriangleType());

            // Демонстрація поворотів
            Triangle rotated1 = t.rotateAroundVertex('A', 45);
            System.out.println("Після повороту на 45° відносно вершини A:");
            System.out.println(rotated1);

            Triangle rotated2 = t.rotateAroundCircumcenter(90);
            System.out.println("Після повороту на 90° відносно центру описаного кола:");
            System.out.println(rotated2);

            System.out.println();
        }

        // Перевірка рівності трикутників
        System.out.println("=== Перевірка рівності трикутників ===");
        Triangle t1 = new Triangle(new Point(0, 0), new Point(3, 0), new Point(0, 4));
        Triangle t2 = new Triangle(new Point(1, 1), new Point(4, 1), new Point(1, 5));
        Triangle t3 = new Triangle(new Point(0, 0), new Point(2, 0), new Point(0, 2));

        System.out.println("Трикутник 1: " + t1);
        System.out.println("Трикутник 2: " + t2);
        System.out.println("Трикутник 3: " + t3);

        System.out.println("Трикутник 1 == Трикутник 2: " + t1.equals(t2));
        System.out.println("Трикутник 1 == Трикутник 3: " + t1.equals(t3));

        scanner.close();
    }
}