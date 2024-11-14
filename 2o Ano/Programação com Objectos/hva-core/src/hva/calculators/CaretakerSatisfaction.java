
package hva.calculators;

import hva.habitat.Habitat;
import hva.habitat.Tree;
import java.util.Collection;
import java.util.List;

public class CaretakerSatisfaction implements CaretakerCalculator {
    @Override
    public int satisfaction(List<Habitat> habitats) {
        return (int) Math.round(300 - workload(habitats));
    }

    public double workload(List<Habitat> habitats) {
        double workload = 0;
        for (Habitat h : habitats) {
            workload += habitatWorkload(h) / h.getNumberOfCaretakers();
        }
        return workload;
    }

    public double habitatWorkload(Habitat h) {
        double workload = h.getArea() + 3 * h.getPopulation();
        double cleaningEffort = 0;
        Collection<Tree> habitatTrees = h.getTrees();
        for (Tree t : habitatTrees) {
            cleaningEffort += t.getCleaningEffort();
        }
        return workload + cleaningEffort;
    }
}
