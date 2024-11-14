package hva.calculators;

import hva.habitat.Season;
import hva.habitat.Tree;

public class TreeCleaningEffort implements TreeCalculator{
    @Override
    public double cleaningEffort(Tree t, Season season) {
        int cleaningEffort = 0;
        cleaningEffort += (double) t.getBaseDifficulty() * t.getCleaningEffort(season) * Math.log(t.getAge() + 1);
        return cleaningEffort;
    }

}
