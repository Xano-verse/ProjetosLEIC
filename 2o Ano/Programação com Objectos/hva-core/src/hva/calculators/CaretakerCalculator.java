package hva.calculators;
import java.io.Serializable;
import java.util.List;

import hva.habitat.Habitat;

public interface CaretakerCalculator extends Serializable{
    public int satisfaction(List<Habitat> habitats);
}
