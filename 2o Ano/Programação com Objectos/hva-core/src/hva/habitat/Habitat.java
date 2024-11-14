package hva.habitat;

import hva.animal.Animal;
import java.io.Serializable;
import java.util.Map;
import java.util.TreeMap;
import java.util.HashMap;
import java.util.List;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;

public class Habitat implements Serializable {
    private String id;
    private String name;
    private int area;
    private int treeAmount;
    private int population;
    private int numberOfCaretakers;
    private Map<String, Tree> trees = new TreeMap<String, Tree>();
    private Map<String, Animal> animals = new HashMap<String, Animal>();
    private Map<String, Integer> speciesInfluence = new HashMap<String, Integer>();
    List<String> speciesIds = new ArrayList<>();
    private int same = 0;

    public Habitat(String id, String name, int area) {
        this.id = id;
        this.name = name;
        this.area = area;
    }

    public void addAnimal(Animal animal) {
        animals.put(animal.getId(), animal);
        speciesIds.add(animal.getSpecies().getId());
        population++;
    }

    public void removeAnimal(Animal animal) {
        animals.remove(animal.getId());
        speciesIds.remove(animal.getSpecies().getId());
        population--;
    }

    public void addTree(Tree tree) {
        trees.put(tree.getTreeId(), tree);
        treeAmount++;
    }

    public void increaseNumberOfCaretakers() {
        numberOfCaretakers++;
    }

    public void decreaseNumberOfCaretakers() {
        numberOfCaretakers--;
    }

    
    public Integer getSpecieInfluence(String idSpecies) {
        return speciesInfluence.getOrDefault(idSpecies, 0);
    }

    public void changehabitatInfluence(String s, String influence) {
        switch (influence) {
            case "POS" -> speciesInfluence.put(s, 20);
            case "NEG" -> speciesInfluence.put(s, -20);
            case "NEU" -> speciesInfluence.put(s, 0);
        }
    }

    public int same(String idSpecies) {
        same = Collections.frequency(speciesIds, idSpecies);
        return same - 1;
    }

    public int different(String idSpecies) {
        return speciesIds.size() - same;
    }

    public void changeArea(int newArea) {
        this.area = newArea;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public int getArea() {
        return area;
    }

    public int getTreeAmount() {
        return treeAmount;
    }
    
    public int getPopulation() {
        return population;
    }
    
    public int getNumberOfCaretakers() {
        return numberOfCaretakers;
    }
    
    public Collection<Tree> getTrees() {
        return trees.values();
    }

    public Collection<Animal> getAnimals() {
        return animals.values();
    }
    
    @Override
    public String toString() {
        String habitatsPlusTrees = "HABITAT|" + id + "|" + name + "|" + area + "|" + treeAmount + "\n";

        for (Tree t : trees.values()) {
            habitatsPlusTrees += t.toString() + "\n";
        }

        // Remover o último \n (há sempre um \n a remover)
        habitatsPlusTrees = habitatsPlusTrees.substring(0, habitatsPlusTrees.length() - 1);

        return habitatsPlusTrees;
    }

}
