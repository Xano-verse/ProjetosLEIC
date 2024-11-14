
package hva.calculators;

import hva.animal.Animal;
import hva.habitat.Habitat;

public class AnimalSatisfaction implements AnimalCalculator {
    @Override
    public int satisfaction(Animal a) {
        String speciesId = a.getSpecies().getId();
        Habitat h = a.getHabitat();
        return Math.round(
                20 + 3 * h.same(speciesId) - 2 * h.different(speciesId) + h.getArea() / h.getPopulation()
                        + h.getSpecieInfluence(speciesId));
    }
}
