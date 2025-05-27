import java.util.Objects;

abstract class Bird {
    public abstract void fly();
    public abstract void sing();

    public void layEggs() {
        System.out.println(getClass().getSimpleName() + " відкладає яйця.");
    }

    @Override
    public boolean equals(Object obj) {
        return obj != null && this.getClass() == obj.getClass();
    }

    @Override
    public int hashCode() {
        return Objects.hash(getClass());
    }

    @Override
    public String toString() {
        return "Птах виду: " + getClass().getSimpleName();
    }
}

class Cuckoo extends Bird {
    @Override
    public void fly() {
        System.out.println("Зозуля літає.");
    }

    @Override
    public void sing() {
        System.out.println("Зозуля кує.");
    }
}

class Hen extends Bird {
    @Override
    public void fly() {
        System.out.println("Курка майже не літає.");
    }

    @Override
    public void sing() {
        System.out.println("Курка кудкудакає.");
    }
}

class BroodyHen extends Hen {
    public void incubateEggs() {
        System.out.println("Квочка висиджує пташенят.");
    }
}

public class Lr1_2 {
    public static void main(String[] args) {
        Cuckoo cuckoo = new Cuckoo();
        Hen hen = new Hen();
        BroodyHen broodyHen = new BroodyHen();

        System.out.println(cuckoo);
        cuckoo.fly();
        cuckoo.sing();
        cuckoo.layEggs();

        System.out.println(hen);
        hen.fly();
        hen.sing();
        hen.layEggs();

        System.out.println(broodyHen);
        broodyHen.fly();
        broodyHen.sing();
        broodyHen.layEggs();
        broodyHen.incubateEggs();
    }
}
