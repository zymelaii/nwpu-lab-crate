import java.text.*;

public class TestClass {
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

    public static void main(String[] args) {
        try {
            DateFormat fmt = new SimpleDateFormat("yyyy-MM-dd");

            Book book = new Book("Java 百炼成魔：开局吞噬 JVM 暴打天道", "2022-02-22", (int) 54e+3);
            System.out.printf(
                "书籍：《%s》 发行日期：%s 总字数：%.2fk\n",
                book.getTitle(),
                fmt.format(book.getPublicationDate()),
                book.getTotalWords() * 1e-3);

            EXPECT_ANY_THROW(() -> {
                try {
                    new Book("foobar", "666666", 0);
                } catch (ParseException e) {
                    throw new RuntimeException(e.getMessage());
                }
            });

            EXPECT_TRUE(book.priceAt(fmt.parse("2022-02-22")) == 54 * 35 * 1.2);
            EXPECT_TRUE(book.priceAt(fmt.parse("2022-06-30")) == 54 * 35 * 1.2);
            EXPECT_TRUE(book.priceAt(fmt.parse("2022-07-01")) == 54 * 35 * 1.18);
            EXPECT_TRUE(book.price() == book.priceAt(fmt.parse("2022-02-22")));
        } catch (ParseException e) {
            //! ignore the exception
        }
    }
}
