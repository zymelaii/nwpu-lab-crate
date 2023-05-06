import java.util.*;
import java.text.*;

public class Book {
    private String title_;
    private Date pubDate_;
    private int totalWords_;

    public Book(String title, String pubDate, int totalWords) throws ParseException {
        DateFormat fmt = new SimpleDateFormat("yyyy-MM-dd");

        title_ = title;
        pubDate_ = fmt.parse(pubDate);
        totalWords_ = totalWords;
    }

    public String getTitle() {
        return title_;
    }

    public Date getPublicationDate() {
        return pubDate_;
    }

    public int getTotalWords() {
        return totalWords_;
    }

    public double price() {
        //! NOTE: 售价参考的日期系数为实际购买日
        return priceAt(new Date());
    }

    public double priceAt(Date date) {
        double e = totalWords_ * 3.5e-2 * getPriceResolution(date);
        //! NOTE: 单本书籍的售价保留到分
        return Math.round(e * 100) / 100.0;
    }

    private double getPriceResolution(Date date) {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        return cal.get(Calendar.MONTH) >= 6 ? 1.18 : 1.2;
    }
}
