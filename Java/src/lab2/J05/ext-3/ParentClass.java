package MainPackage;

public class ParentClass {
    private int value_ = 857; //!< default value

    public int getValue() {
        return value_;
    }

    public void setValue(int value) {
        value_ = value;
    }

    public ParentClass() {
        System.out.println("value: " + getValue());
    }
}
