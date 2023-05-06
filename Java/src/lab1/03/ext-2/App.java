import java.util.*;

class GradeMaintainer {
    private class Course {
        public String name;
        public double credit;
    };

    private class Record {
        public int courseId;
        public int studentId;
        public double grade;

        public double getGradeProduct(HashMap<Integer, Course> lookup) {
            //! NOTE: 根据规则，成绩参与换算时为整数；结合实际，策略为四舍五入
            int tmp = (int) Math.round(grade) - 45;
            int e = 0;
            if (tmp >= 40) {
                e = 4;
            } else if (tmp >= 30) {
                e = 3;
            } else if (tmp >= 15) {
                e = 2;
            } else if (tmp >= 0) {
                e = 1;
            }
            return e * lookup.get(courseId).credit;
        }
    }

    private ArrayList<Record> recordDB_ = new ArrayList<Record>();
    private HashMap<Integer, Course> courseDB_ = new HashMap<Integer, Course>();
    private HashMap<Integer, String> studentDB_ = new HashMap<Integer, String>();

    public Collection<String> studentSet() {
        return studentDB_.values();
    }

    public int getCourseId(String course) {
        for (Map.Entry<Integer, Course> e : courseDB_.entrySet()) {
            if (e.getValue().name.equals(course)) {
                return e.getKey();
            }
        }
        throw new RuntimeException("course `" + course + "` does not exists");
    }

    public int getStudentId(String student) {
        for (Map.Entry<Integer, String> e : studentDB_.entrySet()) {
            if (e.getValue().equals(student)) {
                return e.getKey();
            }
        }
        throw new RuntimeException("student `" + student + "` does not exists");
    }

    public void addCourse(String name, double credit) {
        try {
            int courseId = getCourseId(name);
            courseDB_.get(courseId).credit = credit;
        } catch (RuntimeException e) {
            Course course = new Course();
            course.name = name;
            course.credit = credit;
            courseDB_.put(courseDB_.size() + 1, course);
        }
    }

    public void put(String student, String course, double grade) {
        if (!studentDB_.containsValue(student)) {
            studentDB_.put(studentDB_.size() + 1, student);
        }

        //! NOTE: 不考虑同学生同课程重复录入数据的情况
        Record record = new Record();
        record.courseId = getCourseId(course);
        record.studentId = getStudentId(student);
        record.grade = grade;
        recordDB_.add(record);
    }

    public double getGPAOf(String student) {
        int studentId = getStudentId(student);
        double totalCredit = 0, gpa = 0;
        for (Record e : recordDB_) {
            if (e.studentId == studentId) {
                totalCredit += courseDB_.get(e.courseId).credit;
                gpa += e.getGradeProduct(courseDB_);
            }
        }
        return gpa / totalCredit;
    }
}

public class App {
    public static void main(String[] argv) {
        GradeMaintainer db = new GradeMaintainer();

        db.addCourse("数学", 4);
        db.addCourse("英语", 3.5);
        db.addCourse("数据结构", 3);
        db.addCourse("数据库", 3);

        db.put("张一", "数学", 71.5);
        db.put("张一", "英语", 80.4);
        db.put("张一", "数据结构", 95.5);
        db.put("李二", "数学", 78.5);
        db.put("李二", "数据结构", 54.5);
        db.put("李二", "数据库", 60.5);
        db.put("赵三", "数学", 88.5);
        db.put("赵三", "英语", 92.5);
        db.put("赵三", "数据结构", 71.5);

        for (String student : db.studentSet()) {
            System.out.printf("%s 的 GPA 为 %.4f\n", student, db.getGPAOf(student));
        }
    }
}
