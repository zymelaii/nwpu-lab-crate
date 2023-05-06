public class ConvExceptTest {
    public static void EXPECT_THROW(Runnable call) {
        boolean pass = false;
        try {
            call.run();
        } catch (NumberFormatException e) {
            pass = true;
            System.out.println("caught NumberFormatException: " + e.getMessage());
        } finally {
            System.out.println("EXPECT_THROW: " + (pass ? "PASSED" : "FAILED"));
        }
    }

    public static void EXPECT_NO_THROW(Runnable call) {
        boolean pass = true;
        try {
            call.run();
        } catch (NumberFormatException e) {
            pass = false;
            System.out.println("caught NumberFormatException: " + e.getMessage());
        } finally {
            System.out.println("EXPECT_NO_THROW: " + (pass ? "PASSED" : "FAILED"));
        }
    }

    public static void main(String[] args) {
        EXPECT_THROW(() -> {
            Integer.parseInt(null);
        });
        EXPECT_THROW(() -> {
            Integer.parseInt("±123");
        });
        EXPECT_THROW(() -> {
            Integer.parseInt("-++123");
        });
        EXPECT_THROW(() -> {
            Integer.parseInt(Integer.toString(Integer.MAX_VALUE) + "0");
        });
        EXPECT_NO_THROW(() -> {
            Integer.parseInt("114514");
        });
    }
}
