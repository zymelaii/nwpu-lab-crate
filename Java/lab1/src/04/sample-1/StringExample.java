class StringExample {
    public static void main(String args[]) {
        //! 使用 equals 方法判断 s1 与 s2 是否相同
        String s1 = new String("you are a student");
        String s2 = new String("how are you");
        if (/* COMPLETE */ s1.equals(s2)) {
            System.out.println("s1 与 s2 相同");
        } else {
            System.out.println("s1 与 s2 不相同");
        }

        //! 判断 s3 的前缀是否是 "220302"
        String s3 = new String("22030219851022024");
        if (/* COMPLETE */ s3.startsWith("220302")) {
            System.out.println("吉林省的身份证");
        }

        //! 按着字典序 s4 大于 s5 的表达式
        String s4 = new String("你"), s5 = new String("我");
        if (/* COMPLETE */ s4.compareTo(s5) > 0) {
            System.out.println("按字典序 s4 大于 s5");
        } else {
            System.out.println("按字典序 s4 小于 s5");
        }

        //! 获取 path 中最后出现目录分隔符号的位置
        int position = 0;
        String path = "C:\\java\\jsp\\A.java";
        position = /* COMPLETE */ path.lastIndexOf("\\");
        System.out.println("\"C:\\java\\jsp\\A.java\" 中最后出现 \"\\\" 的位置：" + position);

        //! 获取path中 "A.java" 子字符串
        String fileName = /* COMPLETE */ path.substring(position + 1);
        System.out.println("\"C:\\java\\jsp\\A.java\" 中含有的文件名：" + fileName);

        //! 将 s6 转化成 int 型数据
        String s6 = new String("100");
        int n1 = /* COMPLETE */ Integer.parseInt(s6);

        //! 将 s7 转化成 double 型数据
        String s7 = new String("123.678");
        double n2 = /* COMPLETE */ Double.parseDouble(s7);

        //! String 调用 valuOf(int n) 方法将 m 转化为字符串对象
        double m = n1 + n2;
        System.out.println(m);
        String s8 = /* COMPLETE */ String.valueOf(m);
        position = s8.indexOf(".");
        String temp = s8.substring(position + 1);
        System.out.println("数字 " + m + " 有 " + temp.length() + " 位小数");

        //! 将 s8 存放到数组 a 中
        char a[] = /* COMPLETE */ s8.toCharArray();
        for (int i = a.length - 1; i >= 0; --i) {
            System.out.print(" " + a[i]);
        }
    }
}
