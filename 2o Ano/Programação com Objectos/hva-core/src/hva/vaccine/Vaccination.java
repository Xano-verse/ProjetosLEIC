package hva.vaccine;

import hva.employee.Veterinarian;
import hva.animal.Species;
import java.io.Serializable;

public class Vaccination implements Serializable{
    private Vaccine vaccine;
    private Veterinarian veterinarian;
    private Species species;

    public Vaccination(Vaccine vaccine, Veterinarian veterinarian, Species species) {
        this.vaccine = vaccine;
        this.veterinarian = veterinarian;
        this.species = species;
    }


    public Vaccine getVaccine() {
        return vaccine;
    }

    public Veterinarian getVeterinarian() {
        return veterinarian;
    }

    public Species getSpecies() {
        return species;
    }

   @Override
   public String toString() {
      return "REGISTO-VACINA|" + vaccine.getId() + "|" + veterinarian.getId() + "|" + species.getId(); 
      
   }

}
