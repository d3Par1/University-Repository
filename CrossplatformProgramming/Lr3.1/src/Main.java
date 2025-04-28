import java.text.DecimalFormat;

public class Main {
    public static void main(String[] args) {
        double start = 3.5;
        double end = 10.5;
        double step = 0.2;

        task1(start, end, step);
    }

    public static void task1(double start, double end, double step) {
        DecimalFormat xFormat = new DecimalFormat("0.0");
        DecimalFormat yzFormat = new DecimalFormat("0.0000");

        System.out.println(" x      |    y(x)       |    z(x)      ");
        System.out.println("---------------------------------------");

        for (double x = start; x <= end; x += step) {
            double y = (1 / Math.exp(Math.sqrt(x))) * Math.sin(x);

            double z = Math.tan(Math.sqrt(x)) * Math.sin(x - Math.PI / 2);

            System.out.printf(" %5s  | %10s    | %10s%n", xFormat.format(x), yzFormat.format(y),
                    yzFormat.format(z));
        }
    }
}
