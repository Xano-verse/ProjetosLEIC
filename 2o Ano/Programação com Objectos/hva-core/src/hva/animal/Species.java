package hva.animal;

import java.io.Serializable;

public class Species implements Serializable {
    private String id;
    private String name;
    private int amount;
    private int numberOfVeterinarians;

    public Species(String id, String name) {
        this.id = id;
        this.name = name;
    }


    public void increaseAmount() {
        amount++;
    }

    public void decreaseAmount() {
        amount--;
    }

    public void increaseNumberOfVeterinarians() {
        numberOfVeterinarians++;
    }
    
    public void decreaseNumberOfVeterinarians() {
        numberOfVeterinarians--;
    }
    
    public String getId() {
        return this.id;
    }
    
    public String getName() {
        return name;
    }
    
    public int getAmount() {
        return amount;
    }
    
    public int getNumberOfVeterinarians() {
        return numberOfVeterinarians;
    }
}
