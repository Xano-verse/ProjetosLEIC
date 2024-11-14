package hva.employee;

import hva.animal.Species;
import hva.calculators.VeterinarianCalculator;
import hva.vaccine.Vaccination;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class Veterinarian extends Employee {
    private List<Vaccination> vaccinations = new ArrayList<Vaccination>();
    private List<Species> species = new ArrayList<Species>();
    private VeterinarianCalculator calculator;

    public Veterinarian(String id, String name, VeterinarianCalculator calculator) {
        super(id, name);
        this.calculator = calculator;
    }

    public void addSpecies(Species s) {
        species.add(s);
    }

    public void removeSpecies(Species s) {
        species.remove(s);
    }

    public void addVaccination(Vaccination v) {
        vaccinations.add(v);
    }

    @Override
    public int getSatisfaction() {
        return calculator.satisfaction(species);
    }

    @Override
    public String getFunction() {
        return "VET";
    }

    
    public List<Vaccination> getVaccinations() {
        return vaccinations;
    }
    
    public List<Species> getSpecies() {
        return species;
    }
    
    public VeterinarianCalculator getVeterinarianCalculator() {
        return calculator;
    }

    public void setVeterinarianCalculator(VeterinarianCalculator calculator) {
        this.calculator = calculator;
    }

    @Override
    public String toString() {
        String speciesIds = species.stream()
                .map(Species::getId)
                .collect(Collectors.joining(","));
        if (!"".equals(speciesIds)) {
            return "VET|" + super.toString() + "|" + speciesIds;
        }
        return "VET|" + super.toString();
    }
}
