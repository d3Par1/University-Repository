public class Lr21 {
    public static void main(String[] args) {
        double x1_1 = -0.8, x1_2 = 0.2, x1_3 = 1.5;

        System.out.println("Task 1 results:");
        double result1 = task1(x1_1);
        double result2 = task1(x1_2);
        double result3 = task1(x1_3);

        System.out.printf("\nTask 1: For x = %.4f, y = %.4f" , x1_1, result1);
        System.out.printf("\nTask 1: For x = %.4f, y = %.4f" , x1_2, result2);
        System.out.printf("\nTask 1: For x = %.4f, y = %.4f" , x1_3, result3);

        double x2_1 = -0.5, x2_2 = 0.5, x2_3 = 1.0;
        double x2_4 = 1.2, x2_5 = -1.3, x2_6 = 2.3;
        double x2_7 = -0.8, x2_8 = 0.3, x2_9 = 0.9;

        System.out.println("\nTask 2 results:");
        task2(x2_1);
        task2(x2_2); 
        task2(x2_3);
        task2(x2_4);
        task2(x2_5);
        task2(x2_6);
        task2(x2_7);
        task2(x2_8);
        task2(x2_9);
    }
 

    public static double task1(double x) {
        double a = 2.3;
        double u = Math.sin(x + a);
        double y;

        if (u < -0.5) {
            y = Math.pow(Math.log(0.5 * a - u), 2);
        } else if (u >= -0.5 && u <= 1e-3) {
            y = (2 * x - a) / (7 * Math.PI + x + u);
        } else {
            y = Math.log(u + x) - Math.exp(x);
        }

        return y;
    }


    public static void task2(double x) {
        double a = 1.8;
        double b = 0.4;
        double y;

        if (x <= 0) {
            y = Math.pow(a + x, 2) - Math.pow(b + Math.sin(x), 3);
        } else if (x > 0 && x <= 1) {
            y = Math.pow(a + b * x, 1.0 / 3.0) + Math.cos(b * x);
        } else {
            y = Math.pow(x, 2) - Math.log(b + a * x);
        }

        System.out.printf("Task 2: For x = %.4f, y = %.4f%n", x, y);
    }
}
