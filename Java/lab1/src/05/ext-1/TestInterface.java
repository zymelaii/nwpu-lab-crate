interface ISpeakable {
    public void speak();
}

interface IRunnable {
    public void run();
}

abstract class AbstractCreature {
    public abstract void speak();

    public abstract void run();
}

class Dog implements ISpeakable, IRunnable {
    @Override
    public void speak() {
        System.out.println("狗的声音：汪、汪！");
    }

    @Override
    public void run() {
        System.out.println("狗用四肢跑步");
    }
}

class Bird implements ISpeakable {
    @Override
    public void speak() {
        System.out.println("鸟的声音：啾啾啾！");
    }
}

class Person implements ISpeakable, IRunnable {
    @Override
    public void speak() {
        System.out.println("人们见面时经常说：您好！");
    }

    @Override
    public void run() {
        System.out.println("人用两腿跑步");
    }
}

class CDog extends AbstractCreature {
    @Override
    public void speak() {
        System.out.println("狗的声音：汪、汪！");
    }

    @Override
    public void run() {
        System.out.println("狗用四肢跑步");
    }
}

class CBird extends AbstractCreature {
    @Override
    public void speak() {
        System.out.println("鸟的声音：啾啾啾！");
    }

    @Override
    public void run() {
        System.out.println("鸟用翅膀飞行");
    }
}

class CPerson extends AbstractCreature {
    @Override
    public void speak() {
        System.out.println("人们见面时经常说：您好！");
    }

    @Override
    public void run() {
        System.out.println("人用两腿跑步");
    }
}

public class TestInterface {
    public static void main(String[] args) {
        Dog dog = new Dog();
        dog.speak();
        dog.run();

        Bird bird = new Bird();
        bird.speak();

        Person person = new Person();
        person.speak();
        person.run();

        AbstractCreature[] creatures = { new CDog(), new CBird(), new CPerson() };
        for (AbstractCreature creature : creatures) {
            creature.speak();
            creature.run();
        }
    }
}
