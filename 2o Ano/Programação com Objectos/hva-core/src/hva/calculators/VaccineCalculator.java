package hva.calculators;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import hva.animal.Species;

public class VaccineCalculator {

    public double damage(boolean goodVaccine, Species s1, Collection<Species> species) {
        int max = 0;

        if (goodVaccine) {
            return 0;
        }

        //dano é só o tamanho do nome da especie pois é o maior
        if (species.isEmpty()) {
            return s1.getName().length();
        }
  
        //iteramos sobre todas as especies e aplicamos a formula
        for (Species s : species) {
            int damage = nameSize(s1.getName(), s.getName()) - commonCharacters(s1.getName(), s.getName());
            //garantir que temos sempre o maximo, ir atualizando
            if (damage > max) {
                max = damage;
            }
        }
        return max;
    }

    public int commonCharacters(String s1, String s2) {
        Map<Character, Integer> stringCharacters = new HashMap<>();
        int commonCharacters = 0;

        s1 = s1.toLowerCase();
        s2 = s2.toLowerCase();

        //iterar pela 1a string e adicionar cada caracter ao mapa com o respetivo count
        for (int i = 0; i < s1.length(); i++) {
            char c = s1.charAt(i);
            //getOrDefault se o match da chave for null, poe zero, se nao adiciona 1
            stringCharacters.put(c, stringCharacters.getOrDefault(c, 0) + 1);
        }
        
        //iterar pela 2a string
        for (int i = 0; i < s2.length(); i++) {
            char c = s2.charAt(i);
            //para cada char da string2 vê se existe no mapa (logo existe na 1a string) e se houver pelo menos 1 (count > 0) incrementar commonCharacters 
            if (stringCharacters.containsKey(c) && stringCharacters.get(c) > 0) {
                commonCharacters++;
                //remove 1 no count para se aparecer um caracter repetido 
                stringCharacters.put(c, stringCharacters.get(c) - 1);
            }
        }
        return commonCharacters;
    }

    public int nameSize(String s1, String s2) {
        return Math.max(s1.length(), s2.length());
    }
}
