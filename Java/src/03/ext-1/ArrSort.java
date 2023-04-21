import java.util.*;

public class ArrSort {
    public static void EXPECT_TRUE(boolean cond) {
        System.out.println("EXPECT_TRUE: " + (cond ? "PASSED" : "FAILED"));
    }

    public static String arrayToString(int[] array) {
        String tmp = new String();
        for (int e : array) {
            tmp += e + " ";
        }
        return tmp.trim();
    }

    public static void sort(int[] array) {
        for (int i = 0; i < array.length; ++i) {
            int k = i;
            for (int j = k + 1; j < array.length; ++j) {
                if (array[j] < array[k]) {
                    k = j;
                }
            }
            if (k != i) {
                int t = array[i];
                array[i] = array[k];
                array[k] = t;
            }
        }
    }

    public static int indexOf(int value, int[] array, boolean sorted) {
        if (sorted) {
            //! NOTE: 重值随机返回
            int lo = 0, hi = array.length, m;
            while (lo < hi) {
                m = (lo + hi) / 2;
                if (array[m] > value) {
                    hi = m;
                } else if (array[m] < value) {
                    lo = m + 1;
                } else {
                    return m;
                }
            }
        } else {
            //! NOTE: 重值总是返回第一项
            for (int i = 0; i < array.length; ++i) {
                if (array[i] == value) {
                    return i;
                }
            }
        }
        return -1;
    }

    public static void main(String[] args) {
        int array[] = new int[5];
        array[0] = 10;
        array[1] = 20;
        array[2] = -9;
        array[3] = 8;
        array[4] = 98;

        HashMap<Integer, Integer> lookup = new HashMap<Integer, Integer>();
        for (int i = 0; i < array.length; ++i) {
            lookup.put(array[i], i);
        }

        System.out.println("原数组：" + arrayToString(array));

        EXPECT_TRUE(indexOf(Integer.MAX_VALUE, array, false) == -1);
        for (int e : lookup.keySet()) {
            EXPECT_TRUE(array[indexOf(e, array, false)] == e);
        }

        sort(array);
        System.out.println("升序排序后的数组：" + arrayToString(array));

        EXPECT_TRUE(indexOf(Integer.MAX_VALUE, array, true) == -1);
        for (int e : lookup.keySet()) {
            EXPECT_TRUE(array[indexOf(e, array, true)] == e);
        }
    }
}
