package hva.calculators;

import hva.animal.Species;
import java.io.Serializable;
import java.util.List;

public interface VeterinarianCalculator extends Serializable {
    public int satisfaction(List<Species> species);
}
