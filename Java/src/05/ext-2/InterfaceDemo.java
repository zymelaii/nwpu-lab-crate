interface IVehicle {
    public void start(double position);
    public void stop(double position);
}

class Bike implements IVehicle {
    @Override
    public void start(double position) {
        System.out.printf("您的共享单车即将在 %.2fm 处启动\n", position);
    }

    @Override
    public void stop(double position) {
        System.out.printf("您的共享单车即将在 %.2fm 处停下\n", position);
    }
}

class Bus implements IVehicle {
    @Override
    public void start(double position) {
        System.out.printf("您的宝宝巴士即将在 %.2fm 处启动\n", position);
    }

    @Override
    public void stop(double position) {
        System.out.printf("您的宝宝巴士即将在 %.2fm 处停下\n", position);
    }
}

public class InterfaceDemo { //!< NOTE: 我拒绝之前都是大驼峰现在莫名其妙来个小驼峰
    public static void main(String[] args) {
        IVehicle[] vehicles = {new Bike(), new Bus()};
        for (IVehicle vehicle : vehicles) {
            vehicle.start(114.514);
            vehicle.stop(1919.810);
        }
    }
}
