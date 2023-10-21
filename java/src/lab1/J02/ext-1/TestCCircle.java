public class TestCCircle {
    public static void EXPECT_TRUE(boolean cond) {
        System.out.println("EXPECT_TRUE: " + (cond ? "PASSED" : "FAILED"));
    }

    public static void EXPECT_ANY_THROW(Runnable call) {
        boolean pass = false;
        try {
            call.run();
        } catch (Exception e) {
            pass = true;
        } finally {
            System.out.println("EXPECT_ANY_THROW: " + (pass ? "PASSED" : "FAILED"));
        }
    }

    public static void main(String args[]) {
        CCircle circle = null;

        circle = new CCircle();
        EXPECT_TRUE(circle.getRadius() == 1.0);

        circle = new CCircle(114.514);
        EXPECT_TRUE(circle.getRadius() == 114.514);

        circle.setRadius(2.0);
        EXPECT_TRUE(circle.getRadius() == 2.0);

        circle.setRadius(1.0);
        EXPECT_TRUE(circle.getArea() == CCircle.pi);
        circle.setRadius(2.0);
        EXPECT_TRUE(circle.getArea() == 4 * CCircle.pi);

        EXPECT_ANY_THROW(() -> {
            new CCircle().setRadius(-3);
        });
    }
}
