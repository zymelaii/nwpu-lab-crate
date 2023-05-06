class People {
    private double weight_;
    private double height_; //!< NOTE: 我拒绝服从要求 #1

    protected void setBodyStat(double weight, double height) {
        weight_ = weight;
        height_ = height;
    }

    protected double getWeight() {
        return weight_;
    }

    protected double getHeight() {
        return height_;
    }

    public People() {
        setBodyStat(173, 70);
    }

    public void greet() { //!< NOTE: 我拒绝服从要求 #2
        System.out.println("yayawawa");
    }

    public void averageHeight() {
        System.out.printf("Average height: %.1fcm\n", getHeight());
    }

    public void averageWeight() {
        System.out.printf("Average weight: %.2fkg\n", getWeight());
    }
}

class ChinaPeople extends People {
    public ChinaPeople() {
        setBodyStat(173, 67.34);
    }

    @Override
    public void greet() {
        System.out.println("你好，吃饭了吗");
    }

    @Override
    public void averageHeight() {
        System.out.printf("中国人的平均身高：%.1f厘米\n", getHeight());
    }

    @Override
    public void averageWeight() {
        System.out.printf("中国人的平均体重：%.2f公斤\n", getWeight());
    }

    public void chinaKungfu() { //!< NOTE: 我拒绝服从要求 #3
        System.out.println("坐如钟，站如松，睡如弓");
    }
}

class AmericanPeople extends People {
    public AmericanPeople() {
        setBodyStat(175, 89.8);
    }

    @Override
    public void greet() {
        System.out.println("How do you do");
    }

    @Override
    public void averageHeight() {
        System.out.printf("Average height of Americans：%.1fcm\n", getHeight());
    }

    @Override
    public void averageWeight() {
        System.out.printf("Average weight of Americans: %.2fkg\n", getWeight());
    }

    public void americanBoxing() {
        System.out.println("Jab, Cross, Hook, Uppercut");
    }
}

class PekingPeople extends ChinaPeople {
    public PekingPeople() {
        setBodyStat(174, 69.28);
    }

    @Override
    public void greet() {
        System.out.println("您好，这里是北京");
    }

    @Override
    public void averageHeight() {
        System.out.printf("北京人的平均身高：%.1f厘米\n", getHeight());
    }

    @Override
    public void averageWeight() {
        System.out.printf("北京人的平均体重：%.2f公斤\n", getWeight());
    }

    public void pekingOpera() { //!< NOTE: 我拒绝服从要求 #4
        System.out.println("生、旦、净、丑四大行");
    }
}

public class Example {
    public static void main(String args[]) {
        ChinaPeople chinaPeople = new ChinaPeople();
        AmericanPeople americanPeople = new AmericanPeople();
        PekingPeople pekingPeople = new PekingPeople();

        People[] peoples = { chinaPeople, americanPeople, pekingPeople };

        for (People people : peoples) {
            people.greet();
            people.averageHeight();
            people.averageWeight();
        }

        chinaPeople.chinaKungfu();
        americanPeople.americanBoxing();
        pekingPeople.pekingOpera();
        pekingPeople.chinaKungfu();
    }
}
