package hva.habitat;

import hva.calculators.TreeCleaningEffort;

public class EvergreenTree extends Tree {

    public EvergreenTree(String id, String name, double age, int baseDifficulty) {
        super(id, name, age, baseDifficulty, new EvergreenTreeCleaningEffort(), new EvergreenTreeBiologicalCycle(), new TreeCleaningEffort());
    }

    private String biologicalCycle;
    private int cleaningEffort;

    @Override
    public void updateBiologicalCycle(Season season) {
        biologicalCycle = getBiologicalCycle(season);
    }
    
    @Override
    public void updateCleaningEffort(Season season) {
        cleaningEffort = getCleaningEffort(season);
    }

    @Override
    public int getCleaningEffort(){
        return cleaningEffort;
    }

    @Override
    public String toString() {
        return super.toString() + "|PERENE|" + biologicalCycle;

    }

}
