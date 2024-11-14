package hva.vaccine;

import hva.animal.Species;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;
import java.util.Collection;
import java.io.Serializable;

public class Vaccine implements Serializable{
   private String id;
   private String name;
   //Mapa das espécies que a vacina pode vacinar
   private Map<String, Species> species = new TreeMap<>();
   //Registo de vacinacoes, pedido pelo enunciado - "Tem ainda o registo de aplicações a cada animal pelos veterinários, por ordem de vacinação." 
   private List<Vaccination> vaccinations = new ArrayList<>();
   //Necessário para a apresentação das vacinas 
   private int numberOfApplications;

   public Vaccine(String id, String name, Map<String, Species> species) {
      this.id = id;
      this.name = name;
      this.species = species;
   }

   public Vaccine(String id, String name) {
      this.id = id;
      this.name = name;
   }

   
   public void addSpecies(Species s) {
      species.put(s.getId(), s);
   }
   
   public void addVaccination(Vaccination v) {
      vaccinations.add(v);
   }
   
   public void incrementNumberOfApplications() {
      numberOfApplications++;
   }
   
   public String getId() {
      return id;
   }

   public String getName() {
      return name;
   }
   
   public Collection<Species> getSpecies() {
      return species.values();
   }
   
   public List<Vaccination> getVaccinations() {
      return vaccinations;
   }

   public int getNumberOfApplications() {
      return numberOfApplications;
   }

   @Override
   public String toString() {
      if (species.isEmpty()) {
         return "VACINA|" + id + "|" + name + "|" + numberOfApplications;
      
      } else {
         
         String speciesIds = "";
         for(String s: species.keySet()) {
            speciesIds += s + ","; 
         }
          //retirar a ultima virgula
          speciesIds = speciesIds.substring(0, speciesIds.length() - 1);

         return "VACINA|" + id + "|" + name + "|" + numberOfApplications + "|" + speciesIds;

      }
   }

}
