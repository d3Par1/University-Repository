import java.awt.geom.Point2D;

class Triangle {
    private Point2D.Double A, B, C;

    public Triangle(double ax, double ay, double bx, double by, double cx, double cy) {
        this.A = new Point2D.Double(ax, ay);
        this.B = new Point2D.Double(bx, by);
        this.C = new Point2D.Double(cx, cy);
    }

    public Point2D.Double getA() { return A; }
    public Point2D.Double getB() { return B; }
    public Point2D.Double getC() { return C; }

    public void setA(double x, double y) { this.A.setLocation(x, y); }
    public void setB(double x, double y) { this.B.setLocation(x, y); }
    public void setC(double x, double y) { this.C.setLocation(x, y); }

    private double distance(Point2D.Double p1, Point2D.Double p2) {
        return p1.distance(p2);
    }

    public double perimeter() {
        return distance(A, B) + distance(B, C) + distance(C, A);
    }

    public double area() {
        return Math.abs((A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y)) / 2.0);
    }

    public boolean isEqual(Triangle other) {
        return this.area() == other.area() && this.perimeter() == other.perimeter();
    }

    public boolean isEquilateral() {
        double ab = distance(A, B);
        double bc = distance(B, C);
        double ca = distance(C, A);
        return ab == bc && bc == ca;
    }

    public boolean isIsosceles() {
        double ab = distance(A, B);
        double bc = distance(B, C);
        double ca = distance(C, A);
        return ab == bc || bc == ca || ca == ab;
    }

    public boolean isRight() {
        double ab2 = Math.pow(distance(A, B), 2);
        double bc2 = Math.pow(distance(B, C), 2);
        double ca2 = Math.pow(distance(C, A), 2);
        return Math.abs(ab2 + bc2 - ca2) < 1e-9 ||
                Math.abs(bc2 + ca2 - ab2) < 1e-9 ||
                Math.abs(ca2 + ab2 - bc2) < 1e-9;
    }
}

public class Lr11Step {
    public static void main(String[] args) {
        Triangle t1 = new Triangle(0, 0, 3, 0, 3, 4);
        Triangle t2 = new Triangle(0, 0, 3, 0, 3, 4);

        System.out.println("Периметр: " + t1.perimeter());
        System.out.println("Площа: " + t1.area());
        System.out.println("Рівні трикутники? " + t1.isEqual(t2));
        System.out.println("Рівносторонній? " + t1.isEquilateral());
        System.out.println("Рівнобедрений? " + t1.isIsosceles());
        System.out.println("Прямокутний? " + t1.isRight());
    }
}
