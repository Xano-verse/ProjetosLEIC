package hva.habitat;

import hva.calculators.TreeCalculator;
import java.io.Serializable;

public abstract class Tree implements Serializable {
    private String id;
    private String name;
    private double age;
    private int baseDifficulty;
    private TreeCalculator calculator;
    // private String biologicalCycle;
    private CleaningEffort cleaningEffort;
    private BiologicalCycle biologicalCycle;

    public Tree(String id, String name, double age, int baseDifficulty, CleaningEffort cleaningEffort,
            BiologicalCycle biologicalCycle, TreeCalculator calculator) {
        this.id = id;
        this.name = name;
        this.age = age;
        this.baseDifficulty = baseDifficulty;
        this.cleaningEffort = cleaningEffort;
        this.biologicalCycle = biologicalCycle;
        this.calculator = calculator;
    }

    public abstract void updateBiologicalCycle(Season season);
    public abstract void updateCleaningEffort(Season season);
    public abstract int getCleaningEffort();
    
    public void incrementTreeAge(){
        age += 0.25;
    }
    
    public int getCleaningEffort(Season season) {
        return cleaningEffort.getEffort(season);
    }

    public String getBiologicalCycle(Season season) {
        return biologicalCycle.getCycle(season);
    }
    
    public String getTreeId() {
        return id;
    }
    
    public String getName() {
        return name;
    }

    public double getAge() {
        return age;
    }

    public int getBaseDifficulty() {
        return baseDifficulty;
    }

    public TreeCalculator getCalculator() {
        return calculator;
    }
    

    
    @Override
    public String toString(){
        return "ÁRVORE|" + id + "|" +  name + "|" + (int) age + "|" + baseDifficulty;
    }

}