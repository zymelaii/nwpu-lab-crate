public class CCircle {
    public static final double pi = 3.14159;

    private double radius_;

    public CCircle() {
        setRadius(1.0);
    }

    public CCircle(double radius) {
        setRadius(radius);
    }

    public void setRadius(double radius) {
        if (radius < 0) {
            throw new RuntimeException("radius of circles cannot be negative");
        }
        radius_ = radius;
    }

    public double getRadius() {
        return radius_;
    }

    public double getArea() {
        return pi * radius_ * radius_;
    }
}
