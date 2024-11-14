package hva.calculators;
import java.io.Serializable;
import hva.animal.Animal;

public interface AnimalCalculator extends Serializable{
    public int satisfaction(Animal a);
}
