package hva.employee;

import java.io.Serializable;

public abstract class Employee implements Serializable{
    private String id;
    private String name;

    public Employee(String id, String name) {
        this.id = id;
        this.name = name;
    }

    public abstract int getSatisfaction();
    
    public abstract String getFunction();

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String toString() {
        return id + "|" + name;
    }

    

}
