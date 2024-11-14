package hva.app.animal;

import hva.Hotel;
import hva.app.exceptions.DuplicateAnimalKeyException;
import hva.app.exceptions.DuplicateSpeciesKeyException;
import hva.app.exceptions.DuplicateSpeciesNameException;
import hva.app.exceptions.UnknownSpeciesKeyException;
import hva.app.exceptions.UnknownHabitatKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoRegisterAnimal extends Command<Hotel> {

    DoRegisterAnimal(Hotel receiver) {
        super(Label.REGISTER_ANIMAL, receiver);
        addStringField("id", Prompt.animalKey());
        addStringField("name", Prompt.animalName());
        addStringField("idSpecies", Prompt.speciesKey());
        addStringField("idHabitat", hva.app.habitat.Prompt.habitatKey()); 
    }

    @Override
    protected final void execute() throws CommandException {
        String id = stringField("id");
        String name = stringField("name");
        String idSpecies = stringField("idSpecies");
        String idHabitat = stringField("idHabitat");
        String speciesName = null;

        try {
            _receiver.registerAnimal(id, name, idSpecies, idHabitat);

        } catch(hva.exceptions.DuplicateAnimalKeyException e1) {
            throw new DuplicateAnimalKeyException(id);
        } catch(hva.exceptions.UnknownHabitatKeyException e7) {
            throw new UnknownHabitatKeyException(idHabitat); 
        }
        catch (hva.exceptions.UnknownSpeciesKeyException e2) {
            
            // está fora do try para o catch poder aceder ao novo valor
            speciesName = Form.requestString(Prompt.speciesName());
            
            try{
                _receiver.registerSpecies(idSpecies, speciesName);
            
            } catch(hva.exceptions.DuplicateSpeciesNameException e3){
                throw new DuplicateSpeciesNameException(speciesName);
            
            } catch(hva.exceptions.DuplicateSpeciesKeyException e4) {
                throw new DuplicateSpeciesKeyException(idSpecies);
            }
          
            try {
                _receiver.registerAnimal(id, name, idSpecies, idHabitat);
            
            } catch(hva.exceptions.DuplicateAnimalKeyException e5) {
                throw new DuplicateAnimalKeyException(id);
            
            } catch(hva.exceptions.UnknownSpeciesKeyException e6) {
                throw new UnknownSpeciesKeyException(idSpecies); 
            }  catch(hva.exceptions.UnknownHabitatKeyException e8) {
                throw new UnknownHabitatKeyException(idHabitat); 
            }
            } 
        }
    }
