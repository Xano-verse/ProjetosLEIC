package hva.animal;

import hva.calculators.AnimalCalculator;
import hva.habitat.Habitat;
import hva.vaccine.Vaccination;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

public class Animal implements Serializable {
    private String id;
    private String name;
    private Species species;
    private Habitat habitat;
    //Para o toString mostly
    private List<String> healthHistory = new ArrayList<>();
    // Consultar vacinações de um animal
    private List<Vaccination> vaccinations = new ArrayList<>();
    private AnimalCalculator calculator;


    public Animal(String id, String name, Species species, Habitat habitat, AnimalCalculator calculator) {
        this.id = id;
        this.name = name;
        this.species = species;
        this.habitat = habitat;
        this.calculator = calculator;
    }

    
    public void addHealthEvent(double damage, boolean goodVaccine) {
        
        // damage = 0 porque sao especies iguais
        if(goodVaccine){
            healthHistory.add("NORMAL");
            // damage == 0 e sao especies diferentes 
        } else if(damage == 0) {
            healthHistory.add("CONFUSÃO");
            
        } else if(damage >= 1 && damage <= 4) {
            healthHistory.add("ACIDENTE");
            
        } else if(damage >= 5) {
            healthHistory.add("ERRO");
        }
        
    }
    
    public void addVaccination(Vaccination vaccination) {
        vaccinations.add(vaccination);
    }
    
    public String getId() {
        return id;
    }
    
    public String getName() {
        return this.name;
    }
    
    public Species getSpecies() {
        return species;
    }
    
    public Habitat getHabitat() {
        return habitat;
    }
    
    public List<String> getHealthHistory() {
        return healthHistory;
    }
    
    public List<Vaccination> getVaccinations() {
        return vaccinations;
    }
    
    public AnimalCalculator getCalculator() {
        return calculator;
    }
    
    public void setHabitat(Habitat habitat) {
        this.habitat = habitat;
    }

    public String toString() {
        if (healthHistory.isEmpty()) {
            return "ANIMAL|" + id + "|" + name + "|" + species.getId() + "|" + "VOID" + "|" + habitat.getId();
        }
        return "ANIMAL|" + id + "|" + name + "|" + species.getId() + "|" + String.join(",", healthHistory) + "|" + habitat.getId();
    }
}
