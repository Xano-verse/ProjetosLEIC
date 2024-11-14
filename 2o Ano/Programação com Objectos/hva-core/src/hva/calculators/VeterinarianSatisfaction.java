package hva.calculators;

import hva.animal.Species;
import java.util.List;

public class VeterinarianSatisfaction implements VeterinarianCalculator {
    @Override
    public int satisfaction(List<Species> species) {
        return (int) Math.round(20 - workload(species));
    }

    public double workload(List<Species> species) {
        double workload = 0;
        for (Species s : species) {
            workload += (s.getAmount() / s.getNumberOfVeterinarians());
        }
        return workload;
    }

}
