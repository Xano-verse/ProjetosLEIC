package hva.habitat;

import hva.calculators.TreeCleaningEffort;

public class DeciduousTree extends Tree {
    
    public DeciduousTree(String id, String name, double age, int baseDifficulty) {
        super(id, name, age, baseDifficulty, new DeciduousTreeCleaningEffort(), new DeciduousTreeBiologicalCycle(), new TreeCleaningEffort());
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
        return super.toString() + "|CADUCA|" + biologicalCycle;

    }

}
