package hva;

import hva.animal.Animal;
import hva.animal.Species;
import hva.calculators.AnimalSatisfaction;
import hva.calculators.CaretakerSatisfaction;
import hva.calculators.VaccineCalculator;
import hva.calculators.VeterinarianSatisfaction;
import hva.employee.Caretaker;
import hva.employee.Employee;
import hva.employee.Veterinarian;
import hva.exceptions.DuplicateAnimalKeyException;
import hva.exceptions.DuplicateEmployeKeyException;
import hva.exceptions.DuplicateHabitatKeyException;
import hva.exceptions.DuplicateSpeciesKeyException;
import hva.exceptions.DuplicateSpeciesNameException;
import hva.exceptions.DuplicateTreeKeyException;
import hva.exceptions.DuplicateVaccineKeyException;
import hva.exceptions.ImportFileException;
import hva.exceptions.NoResponsibilityException;
import hva.exceptions.UnknownAnimalKeyException;
import hva.exceptions.UnknownEmployeKeyException;
import hva.exceptions.UnknownHabitatKeyException;
import hva.exceptions.UnknownSpeciesKeyException;
import hva.exceptions.UnknownVaccineKeyException;
import hva.exceptions.UnknownVeterinarianKeyException;
import hva.exceptions.UnrecognizedEntryException;
import hva.exceptions.VeterinarianNotAuthorizedException;
import hva.habitat.DeciduousTree;
import hva.habitat.EvergreenTree;
import hva.habitat.Habitat;
import hva.habitat.Season;
import hva.habitat.Tree;
import hva.vaccine.Vaccination;
import hva.vaccine.Vaccine;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.Serial;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

public class Hotel implements Serializable {

    @Serial
    private static final long serialVersionUID = 202407081733L;

    private Map<String, Animal> animals = new TreeMap<>(String.CASE_INSENSITIVE_ORDER);
    private Map<String, Tree> trees = new TreeMap<>(String.CASE_INSENSITIVE_ORDER);
    private Map<String, Habitat> habitats = new TreeMap<>(String.CASE_INSENSITIVE_ORDER);
    private Map<String, Vaccine> vaccines = new TreeMap<>(String.CASE_INSENSITIVE_ORDER);
    private Map<String, Species> species = new TreeMap<>(String.CASE_INSENSITIVE_ORDER);
    private Map<String, Employee> employees = new TreeMap<>(String.CASE_INSENSITIVE_ORDER);

    private List<Vaccination> allVaccinations = new ArrayList<>();
    private List<Vaccination> wrongVaccinations = new ArrayList<>();

    // Atributo para saber se houve alterações feitas ao Hotel ou não
    private boolean changed = false;
    private boolean caretakersExist = false; 
    private Season currentSeason = Season.SPRING;

    /**
     * Changes the current season.
     * 
     */
    public int advanceSeason() {
        switch (currentSeason) {
            case WINTER -> currentSeason = Season.SPRING;
            case SPRING -> currentSeason = Season.SUMMER;
            case SUMMER -> currentSeason = Season.AUTUMN;
            case AUTUMN -> currentSeason = Season.WINTER;
        }
        for (Tree t : trees.values()) {
            t.incrementTreeAge();
        }

        setChanged(true);
        return currentSeason.getSeasonIndex();
    }

    public int showGlobalSatisfaction() {
        int totalSatisfaction = 0;
        for (Animal animal : animals.values()) {
            try {
                totalSatisfaction += getAnimalSatisfaction(animal.getId());
            } catch (UnknownAnimalKeyException e) {
                e.printStackTrace();
            }
        }
        for (Employee employee : employees.values()) {
            try {
                totalSatisfaction += getEmployeeSatisfaction(employee.getId());
            } catch (UnknownEmployeKeyException e1) {
                e1.printStackTrace();
            }
        }
        return totalSatisfaction;
    }

    /**
     * Register a new Animal.
     * 
     * @param id
     * @param name
     * @param idSpecies
     * @param idHabitat
     * @throws UnknownSpeciesKeyException
     * @throws DuplicateAnimalKeyException
     */
    public void registerAnimal(String id, String name, String idSpecies, String idHabitat)
            throws DuplicateAnimalKeyException, UnknownSpeciesKeyException, UnknownHabitatKeyException {

        if (animals.containsKey(id)) {
            throw new DuplicateAnimalKeyException();
        }

        if (!habitats.containsKey(idHabitat)) {
            throw new UnknownHabitatKeyException();
        }
        if (!species.containsKey(idSpecies)) {
            throw new UnknownSpeciesKeyException();
        }

        Species s = species.get(idSpecies);
        Habitat h = habitats.get(idHabitat);
        Animal animal = new Animal(id, name, s, h, new AnimalSatisfaction());
        animals.put(id, animal);
        s.increaseAmount();
        h.addAnimal(animal);
        setChanged(true);
    }

    /**
     * Register a new Species.
     * 
     * @param id
     * @param name
     * @throws DuplicateSpeciesKeyException
     * @throws DuplicateSpeciesNameException
     */
    public void registerSpecies(String id, String name)
            throws DuplicateSpeciesNameException, DuplicateSpeciesKeyException {
        if (species.containsKey(id)) {
            throw new DuplicateSpeciesKeyException();
        }
        for (Species s : species.values()) {
            if (s.getName().equals(name)) {
                throw new DuplicateSpeciesNameException();
            }
        }

        Species newSpecies = new Species(id, name);
        species.put(id, newSpecies);
        setChanged(true);
    }

    /**
     * Register a new Habitat.
     * 
     * @param id
     * @param name
     * @param area
     * @throws DuplicateHabitatKeyException
     */
    public void registerHabitat(String id, String name, int area) throws DuplicateHabitatKeyException {
        if (habitats.containsKey(id)) {
            throw new DuplicateHabitatKeyException();
        }
        Habitat habitat = new Habitat(id, name, area);
        habitats.put(id, habitat);
        setChanged(true);
    }

    /**
     * Register a new Employee.
     * 
     * @param id
     * @param name
     * @param function
     * @throws DuplicateEmployeKeyException
     */
    public void registerEmployee(String id, String name, String function) throws DuplicateEmployeKeyException {
        if (employees.containsKey(id)) {
            throw new DuplicateEmployeKeyException();
        }

        if (function.equals("TRATADOR") || function.equals("TRT")) {
            Caretaker caretaker = new Caretaker(id, name, new CaretakerSatisfaction());
            employees.put(id, caretaker);
            caretakersExist = true;
        } else {
            Veterinarian veterinarian = new Veterinarian(id, name, new VeterinarianSatisfaction());
            employees.put(id, veterinarian);
        }
        setChanged(true);
    }

    /**
     * Register a new Tree.
     * 
     * @param id
     * @param name
     * @param age
     * @param difficulty
     * @param treeType
     * @throws DuplicateTreeKeyException
     */
    public void registerTree(String id, String name, double age, int difficulty, String treeType)
            throws DuplicateTreeKeyException {
        if (trees.containsKey(id)) {
            throw new DuplicateTreeKeyException();
        }

        if (treeType.equals("CADUCA")) {
            Tree deciduousTree = new DeciduousTree(id, name, age, difficulty);
            trees.put(id, deciduousTree);
            deciduousTree.updateBiologicalCycle(currentSeason);

        } else if (treeType.equals("PERENE")) {
            Tree evergreenTree = new EvergreenTree(id, name, age, difficulty);
            trees.put(id, evergreenTree);
            evergreenTree.updateBiologicalCycle(currentSeason);
        }
        // Tree tree = new Tree(idTree, name, age, difficulty, treeType);
        setChanged(true);

    }

    /**
     * Register a new Vaccine.
     * 
     * @param id
     * @param name
     * @param vaccineSpecies
     * @throws DuplicateVaccineKeyException
     */
    public void registerVaccine(String id, String name, List<String> vaccineSpecies)
            throws DuplicateVaccineKeyException, UnknownSpeciesKeyException {
        if (vaccines.containsKey(id)) {
            throw new DuplicateVaccineKeyException();
        }

        // Converter a lista de Ids para um mapa de Ids,Especies
        Map<String, Species> vaccineSpeciesMap = new TreeMap<>();
        for (String idSpecies : vaccineSpecies) {
            if (species.get(idSpecies) == null) {
                throw new UnknownSpeciesKeyException(idSpecies);
            }
            vaccineSpeciesMap.put(idSpecies, species.get(idSpecies));
        }

        Vaccine vaccine = new Vaccine(id, name, vaccineSpeciesMap);
        vaccines.put(id, vaccine);
        setChanged(true);
    }

    public void addResponsabilityToEmployee(String idEmployee, String idResponsability)
            throws NoResponsibilityException, UnknownEmployeKeyException {
        Employee employee = employees.get(idEmployee);
        if (employee == null) {
            throw new UnknownEmployeKeyException();
        }
        if (employee.getFunction().equals("VET")) {
            Veterinarian vet = (Veterinarian) employee;
            Species s = species.get(idResponsability);
            if (s == null) {
                throw new NoResponsibilityException();
            }
            s.increaseNumberOfVeterinarians();
            vet.addSpecies(s);
        } else {
            Caretaker crt = (Caretaker) employee;
            Habitat h = habitats.get(idResponsability);
            if (h == null) {
                throw new NoResponsibilityException();
            }
            h.increaseNumberOfCaretakers();
            crt.addHabitat(h);
        }
    }

    public void removeResponsabilityToEmployee(String idEmployee, String idResponsability)
            throws NoResponsibilityException, UnknownEmployeKeyException {
        Employee employee = employees.get(idEmployee);
        if (employee == null) {
            throw new UnknownEmployeKeyException();
        }
        if (employee.getFunction().equals("VET")) {
            Veterinarian vet = (Veterinarian) employee;
            Species s = species.get(idResponsability);
            if (s == null) {
                throw new NoResponsibilityException();
            }
            vet.removeSpecies(s);
            s.decreaseNumberOfVeterinarians();
        } else {
            Caretaker trt = (Caretaker) employee;
            Habitat h = habitats.get(idResponsability);
            if (h == null) {
                throw new NoResponsibilityException();
            }
            h.decreaseNumberOfCaretakers();
            trt.removeHabitat(h);
        }
    }

    public void transferAnimalToHabitat(String idAnimal, String idNewHabitat)
            throws UnknownAnimalKeyException, UnknownHabitatKeyException {
        if (!animals.containsKey(idAnimal))
            throw new UnknownAnimalKeyException();

        if (!habitats.containsKey(idNewHabitat))
            throw new UnknownHabitatKeyException();

        Animal animal = animals.get(idAnimal);
        Habitat oldHabitat = animal.getHabitat();
        Habitat newHabitat = habitats.get(idNewHabitat);

        // tirar animal do habitat antigo, adicionar animal ao habitat novo, atualizar
        // habitat no animal

        oldHabitat.removeAnimal(animal);
        newHabitat.addAnimal(animal);
        animal.setHabitat(newHabitat);
    }

    // Atraves do animal conseguimos o seu habitat atual e removemos desse e
    // adicionamos ao habitat do idHabitatDestino. also atualizar o atributo do
    // animal
    public void changeHabitatArea(String id, int newArea) throws UnknownHabitatKeyException {
        if (!habitats.containsKey(id)) {
            throw new UnknownHabitatKeyException();
        }

        Habitat h = habitats.get(id);
        h.changeArea(newArea);
    }

    public void changehabitatInfluence(String idHabitat, String idSpecies, String influence)
            throws UnknownHabitatKeyException, UnknownSpeciesKeyException {
        if (!species.containsKey(idSpecies)) {
            throw new UnknownSpeciesKeyException();
        }
        if (!habitats.containsKey(idHabitat)) {
            throw new UnknownHabitatKeyException();
        }
        habitats.get(idHabitat).changehabitatInfluence(idSpecies, influence);
    }

    // // Tree
    public Tree plantTreeInHabitat(String idHabitat, String idTree, String name,
            double age, int difficulty,
            String treeType) throws DuplicateTreeKeyException, UnknownHabitatKeyException {
        if (!habitats.containsKey(idHabitat)) {
            throw new UnknownHabitatKeyException();
        }
        registerTree(idTree, name, age, difficulty, treeType);
        Tree t = trees.get(idTree);
        habitats.get(idHabitat).addTree(t);
        t.updateBiologicalCycle(currentSeason);
        return t;
    }

    public int getAnimalSatisfaction(String id) throws UnknownAnimalKeyException {
        if (!animals.containsKey(id)) {
            throw new UnknownAnimalKeyException();
        }
        Animal a = animals.get(id);
        return a.getCalculator().satisfaction(a);
    }

    public int getEmployeeSatisfaction(String id) throws UnknownEmployeKeyException {
        if (!employees.containsKey(id)) {
            throw new UnknownEmployeKeyException();
        }
        Employee employee = employees.get(id);
        if(caretakersExist){
            for (Tree t : trees.values()){
                t.updateCleaningEffort(currentSeason);
            }
        }
        return employee.getSatisfaction();
    }

    // public void incrementTreeAge(Collection<?> c) {
    // }

    // // Vaccine
    public boolean vaccinateAnimal(String idVaccine, String idVeterinarian, String idAnimal)
            throws UnknownVeterinarianKeyException, VeterinarianNotAuthorizedException, UnknownVaccineKeyException {

        Animal animal = animals.get(idAnimal);
        Species animalSpecies = animal.getSpecies();

        if (!employees.containsKey(idVeterinarian) || !employees.get(idVeterinarian).getFunction().equals("VET"))
            throw new UnknownVeterinarianKeyException(idVeterinarian);

        // cast é feito para poder usar a funcao getSpecies() sendo que apenas pertence
        // ao veterinario
        Veterinarian veterinarian = (Veterinarian) employees.get(idVeterinarian);

        // ver se a lista de especies do veterinario contem a especie do animal
        if (!veterinarian.getSpecies().contains(animalSpecies))
            throw new VeterinarianNotAuthorizedException(idVeterinarian, animalSpecies.getId());

        if (!vaccines.containsKey(idVaccine))
            throw new UnknownVaccineKeyException();

        Vaccine vaccine = vaccines.get(idVaccine);

        // sera true se a especie do animal a vacinar estiver na lista de especies da
        // vacina
        boolean goodVaccine = vaccine.getSpecies().contains(animalSpecies);

        VaccineCalculator vaccineCalculator = new VaccineCalculator();
        animal.addHealthEvent(vaccineCalculator.damage(goodVaccine, animalSpecies, vaccine.getSpecies()), goodVaccine);

        Vaccination vaccination = new Vaccination(vaccine, veterinarian, animalSpecies);

        vaccine.addVaccination(vaccination);
        allVaccinations.add(vaccination);

        vaccine.incrementNumberOfApplications();

        // para as consultas depois
        veterinarian.addVaccination(vaccination);
        animal.addVaccination(vaccination);
        
        if (!goodVaccine)
            wrongVaccinations.add(vaccination);
    
        return goodVaccine;
    }

    /**
     * Return all the animals as an unmodifiable collection.
     * 
     * @return Collection<Animal> a collection of all animals
     */
    public Collection<Animal> showAllAnimals() {
        return Collections.unmodifiableCollection(animals.values());
    }

    /**
     * Return all the employees as an unmodifiable collection.
     * 
     * @return Collection<Employee> a collection of all employees
     */
    public Collection<Employee> showAllEmployees() {
        return Collections.unmodifiableCollection(employees.values());
    }

    public Collection<Vaccination> showMedicalActsByVeterinarian(String idVeterinarian)
            throws UnknownVeterinarianKeyException {
        if (!employees.containsKey(idVeterinarian) || !employees.get(idVeterinarian).getFunction().equals("VET"))
            throw new UnknownVeterinarianKeyException(idVeterinarian);

        // cast é feito para poder usar a funcao getVaccines() sendo que apenas pertence
        // ao veterinario
        Veterinarian veterinarian = (Veterinarian) employees.get(idVeterinarian);

        return Collections.unmodifiableCollection(veterinarian.getVaccinations());

    }

    public Collection<Vaccination> showMedicalActsOnAnimal(String idAnimal) throws UnknownAnimalKeyException {
        if (!animals.containsKey(idAnimal))
            throw new UnknownAnimalKeyException();

        Animal animal = animals.get(idAnimal);

        return Collections.unmodifiableCollection(animal.getVaccinations());
    }

    public Collection<Vaccination> showWrongVaccinations() {
        return Collections.unmodifiableCollection(wrongVaccinations);
    }

    /**
     * Return all the vaccines as an unmodifiable collection.
     * 
     * @return Collection<Vaccine> a collection of all the vaccines
     */
    public Collection<Vaccine> showAllVaccines() {
        return Collections.unmodifiableCollection(vaccines.values());
    }

    public Collection<Vaccination> showVaccinations() {
        return Collections.unmodifiableCollection(allVaccinations);
    }

    public Collection<Habitat> showAllHabitats() {
        return Collections.unmodifiableCollection(habitats.values());

    }

    /*
     * 
     * // ?!?!?!?!?!?!??!?!?!?!?!??!?!?!?!?!?
     * public String showAllHabitats() {
     * 
     * String habitatsPlusTrees = "";
     * 
     * for (Habitat h : habitats.values()) {
     * habitatsPlusTrees += h.toString();
     * habitatsPlusTrees += "\n";
     * 
     * for (Tree t : h.getTrees()) {
     * t.updateBiologicalCycle(currentSeason);
     * habitatsPlusTrees += t.toString();
     * habitatsPlusTrees += "\n";
     * }
     * }
     * 
     * // Remover o \n no final
     * if (habitatsPlusTrees.length() > 1) {
     * return habitatsPlusTrees.substring(0, habitatsPlusTrees.length() - 1);
     * } else {
     * return habitatsPlusTrees;
     * }
     * }
     */
    public Collection<Animal> showAnimalsInHabitat(String idHabitat) throws UnknownHabitatKeyException {
        if (!habitats.containsKey(idHabitat))
            throw new UnknownHabitatKeyException();

        Habitat habitat = habitats.get(idHabitat);

        return Collections.unmodifiableCollection(habitat.getAnimals());
    }

    /**
     * Return all the habitats as an unmodifiable collection.
     * 
     * @return Collection<Habitat> a collection of all habitats
     */
    // public Collection<Object> showAllHabitats() {
    // List<Object> habitatsPlusTrees = new ArrayList<>();

    // for (Habitat h : habitats.values()) {
    // habitatsPlusTrees.add(h);
    // Collection<Tree> habitatTrees = h.getTrees();
    // for (Tree t : habitatTrees) {
    // habitatsPlusTrees.add(t);
    // t.updateBiologicalCycle(currentSeason);
    // }
    // }
    // return Collections.unmodifiableCollection(habitatsPlusTrees);
    // }
    /**
     * Return all the vaccines as an unmodifiable collection.
     *
     * @param idHabitat
     * @return Collection<Tree> a collection of all the trees in the habitat
     */
    public Collection<Tree> showAllTreesInHabitat(String idHabitat) throws UnknownHabitatKeyException {
        // Repeticao de codigo!?!??!
        if (!habitats.containsKey(idHabitat)) {
            throw new UnknownHabitatKeyException();
        }
        Collection<Tree> habitatTrees = habitats.get(idHabitat).getTrees();

        for (Tree t : habitatTrees) {
            t.updateBiologicalCycle(currentSeason);
        }
        return Collections.unmodifiableCollection(habitatTrees);
    }

    /**
     * Read and import a species from input file.
     * 
     * @param fields
     * @throws DuplicateSpeciesNameException
     * @throws DuplicateSpeciesKeyException
     */
    public void importSpecies(String... fields) throws DuplicateSpeciesNameException, DuplicateSpeciesKeyException {
        registerSpecies(fields[1], fields[2]);
    }

    /**
     * Read and import a tree from input file.
     * 
     * @param fields
     * @throws DuplicateTreeKeyException
     */
    public void importTree(String... fields) throws DuplicateTreeKeyException {
        registerTree(fields[1], fields[2], Integer.parseInt(fields[3]), Integer.parseInt(fields[4]), fields[5]);
    }

    /**
     * Read and import a habitat from input file.
     * 
     * @param fields
     * @throws DuplicateHabitatKeyException
     */
    public void importHabitat(String... fields) throws DuplicateHabitatKeyException {
        registerHabitat(fields[1], fields[2], Integer.parseInt(fields[3]));
        Habitat h = habitats.get(fields[1]);
        String habitatTreesString = String.join(",", Arrays.copyOfRange(fields, 4, fields.length));
        if (!habitatTreesString.equals("")) {
            String[] habitatTrees = habitatTreesString.split(",");
            for (String s : habitatTrees) {
                h.addTree(trees.get(s));
            }
        }
    }

    /**
     * Read and import an animal from input file.
     * 
     * @param fields
     * @throws DuplicateAnimalKeyException
     * @throws UnknownSpeciesKeyException
     */
    public void importAnimal(String... fields)
            throws DuplicateAnimalKeyException, UnknownSpeciesKeyException, UnknownHabitatKeyException {
        registerAnimal(fields[1], fields[2], fields[3], fields[4]);
    }

    /**
     * Read and import an employee from input file.
     * 
     * @param fields
     * @throws DuplicateEmployeKeyException
     */
    public void importEmployee(String... fields)
            throws DuplicateEmployeKeyException, NoResponsibilityException, UnknownEmployeKeyException {
        registerEmployee(fields[1], fields[2], fields[0]);
        // Buscar o employee com o id "fields[1]" e adicionar os habitats/Especies com
        // os possiveis id's nos restantes fields

        if (fields.length > 3) {

            for (String idResponsability : fields[3].replace(" ", "").split(",")) {
                addResponsabilityToEmployee(fields[1], idResponsability);
            }
        }
    }

    /**
     * Read and import a vaccine from input file.
     * 
     * @param fields
     * @throws DuplicateVaccineKeyException
     */
    public void importVaccine(String... fields) throws DuplicateVaccineKeyException, UnknownSpeciesKeyException {
        // Cria uma vacina com o 2o field e o 3o field (o id e o nome)
        Vaccine vaccine = new Vaccine(fields[1], fields[2]);
        vaccines.put(fields[1], vaccine);

        // Se houver 4o field (length > 3) entao sera uma string com os ids.
        // Isto adiciona manualmente cada especie à vacina, para cada id inputed vai
        // buscar a especie em si e adiciona à vacina
        if (fields.length > 3) {
            for (String idSpecies : fields[3].replace(" ", "").split(",")) {
                vaccine.addSpecies(species.get(idSpecies));
            }
        }
    }

    /**
     * Create objects from import
     * 
     * @param fields
     * @throws UnrecognizedEntryException
     * @throws DuplicateSpeciesNameException
     * @throws IOException
     * @throws DuplicateSpeciesKeyException
     * @throws DuplicateTreeKeyException
     * @throws DuplicateHabitatKeyException
     * @throws DuplicateAnimalKeyException
     * @throws DuplicateEmployeKeyException
     * @throws DuplicateVaccineKeyException
     * @throws UnknownSpeciesKeyException
     */
    public void registerEntry(String... fields)
            throws UnrecognizedEntryException, DuplicateSpeciesNameException, IOException,
            DuplicateSpeciesKeyException, DuplicateTreeKeyException, DuplicateHabitatKeyException,
            DuplicateAnimalKeyException, UnknownHabitatKeyException, DuplicateEmployeKeyException,
            DuplicateVaccineKeyException,
            UnknownSpeciesKeyException, NoResponsibilityException, UnknownEmployeKeyException

    {
        switch (fields[0]) {
            case "ESPÉCIE" -> importSpecies(fields);
            case "ÁRVORE" -> importTree(fields);
            case "HABITAT" -> importHabitat(fields);
            case "ANIMAL" -> importAnimal(fields);
            case "TRATADOR", "VETERINÁRIO" -> importEmployee(fields);
            case "VACINA" -> importVaccine(fields);
            default -> throw new UnrecognizedEntryException(fields[0]);
        }
    }

    /**
     * Read text input file and create domain entities.
     *
     * @param filename name of the text input file
     * @throws ImportFileException
     */
    public void importFile(String filename) throws ImportFileException {
        try (BufferedReader reader = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = reader.readLine()) != null) {
                String[] fields = line.split("\\|");
                try {
                    registerEntry(fields);
                } catch (UnrecognizedEntryException | DuplicateSpeciesNameException | IOException
                        | DuplicateSpeciesKeyException | DuplicateTreeKeyException | DuplicateHabitatKeyException
                        | DuplicateAnimalKeyException | DuplicateEmployeKeyException | DuplicateVaccineKeyException
                        | UnknownSpeciesKeyException | UnknownHabitatKeyException | NoResponsibilityException
                        | UnknownEmployeKeyException e) {
                    throw new ImportFileException(filename, e);
                }
            }
        } catch (IOException e) {
            throw new ImportFileException(filename, e);
        }

    }

    /**
     * @return changed value
     */
    public boolean getChanged() {
        return changed;
    }

    /**
     * @param state to be changed
     */
    public void setChanged(boolean state) {
        changed = state;
    }

}
