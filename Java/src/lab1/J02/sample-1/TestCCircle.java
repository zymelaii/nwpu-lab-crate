class CCircle {
    double pi;
    double radius;

    double getRadius() {
        return radius;
    }

    void setCircle(double r, double p) {
        pi = p;
        radius = r;
    }
}

public class TestCCircle {
    public static void main(String args[]) {
        CCircle cir1 = new CCircle();
        cir1.setCircle(2.0, 3.1416);
        System.out.println("radius=" + cir1.getRadius());
    }
}
