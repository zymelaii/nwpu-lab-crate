import MainPackage.SubClass;

public class DemoPackage {
    public static void main(String[] args) {
        SubClass object = new SubClass(23333);
        System.out.println("new value: " + object.getValue());
    }
}
