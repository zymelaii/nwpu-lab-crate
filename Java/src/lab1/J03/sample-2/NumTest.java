public class NumTest {
    public static void main(String args[]) {
        System.out.println("在1~1000可被3与7整除的为");
        for (int n = 1; n <= 1000; ++n) {
            if (n % 3 == 0 && n % 7 == 0) {
                System.out.print(n + " ");
            }
        }
        System.out.println(" ");
    }
}
