package hva.calculators;

import hva.habitat.Season;
import hva.habitat.Tree;


public interface TreeCalculator {
    public double cleaningEffort(Tree t, Season season);
}
