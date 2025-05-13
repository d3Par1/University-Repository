import java.util.Scanner;

public class Lr22 {
    public static void main(String[] args) {
        try (Scanner sc = new Scanner(System.in)) {

            System.out.println("Task 3: Radian to tangent and cotangent comparison");
            for (int i = 1; i <= 3; i++) {
                System.out.print("Enter radian measure for set " + i + ": ");
                double radian = sc.nextDouble();
                task3(radian);
            }

            System.out.println("\nTask 4: Triangle possibility check");
            task4(8, 13.3, 1.1);
            task4(3, 5.36, 0.55);
            task4(1.9, 0.9, 0.9);
        }
    }

    public static void task3(double radian) {
        double tanValue = Math.tan(radian);
        double cotValue = 1 / tanValue;
        if (Math.abs(tanValue) > Math.abs(cotValue)) {
            System.out.printf("The tangent value %.4f is larger than the cotangent value %.4f.%n", tanValue, cotValue);
        } else {
            System.out.printf("The cotangent value %.4f is larger than the tangent value %.4f.%n", cotValue, tanValue);
        }
    }

    public static void task4(double a, double b, double c) {
        if (a + b > c && a + c > b && b + c > a) {
            System.out.printf("Triangle with sides a = %.2f, b = %.2f, c = %.2f can be formed.%n", a, b, c);
        } else {
            System.out.printf("Triangle with sides a = %.2f, b = %.2f, c = %.2f cannot be formed.%n", a, b, c);
        }
    }
}

