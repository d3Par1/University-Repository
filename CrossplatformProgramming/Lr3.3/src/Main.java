import java.util.Scanner;

public class Main {
public static void main(String[] args) {
    Scanner scanner = new Scanner(System.in);

    System.out.print("Enter value for x1: ");
    double x1 = scanner.nextDouble();
    System.out.print("Enter value for x2: ");
    double x2 = scanner.nextDouble();

    System.out.printf("Results for x1 = %.4f: %.4f\n", x1, task3(x1));
    System.out.printf("Results for x2 = %.4f: %.4f\n", x2, task3(x2));

    scanner.close();
}

public static double task3(double x) {
    double result = 0.0;

    for (int k = 1; k <= 8; k++) {
        result += Math.pow(-1, k) * Math.pow(x, 2 * k - 1) / (Math.pow(2, k) * factorial(k));
    }

    return result;
}

public static int factorial(int n) {
    int fact = 1;
    for (int i = 2; i <= n; i++) {
        fact *= i;
    }
    return fact;
}
}

