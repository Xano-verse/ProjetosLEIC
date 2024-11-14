package hva.employee;

import hva.calculators.CaretakerCalculator;
import hva.habitat.Habitat;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class Caretaker extends Employee {
    private List<Habitat> habitats = new ArrayList<Habitat>();
    private CaretakerCalculator calculator;

    public Caretaker(String id, String name, CaretakerCalculator calculator) {
        super(id, name);
        this.calculator = calculator;
    }

    public void addHabitat(Habitat h) {
        habitats.add(h);
    }

    public void removeHabitat(Habitat h) {
        habitats.remove(h);
    }

    @Override
    public int getSatisfaction(){
        return calculator.satisfaction(habitats);
    }
    

    @Override
    public String getFunction() {
        return "TRT";
    }

    public List<Habitat> getHabitats(){
        return habitats;
    }

    public CaretakerCalculator getCaretakerCalculator() {
        return calculator;
    }

    public void setCaretakerCalculator(CaretakerCalculator calculator) {
        this.calculator = calculator;
    }

    @Override
    public String toString() {
        String habitatsIds = habitats.stream()
                .map(Habitat::getId)
                .collect(Collectors.joining(","));
        if (!"".equals(habitatsIds)) {
            return "TRT|" + super.toString() + "|" + habitatsIds;
        }
        return "TRT|" + super.toString();
    }
}
