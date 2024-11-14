package hva.app.vaccine;

import hva.Hotel;
import hva.app.exceptions.DuplicateVaccineKeyException;
import hva.app.exceptions.UnknownSpeciesKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import java.util.ArrayList;
import java.util.Arrays;
//FIXME import other classes if needed

class DoRegisterVaccine extends Command<Hotel> {

    DoRegisterVaccine(Hotel receiver) {
        super(Label.REGISTER_VACCINE, receiver);
	      addStringField("id", Prompt.vaccineKey());
        addStringField("name", Prompt.vaccineName());
        addStringField("species", Prompt.listOfSpeciesKeys());
    }

    @Override
    protected final void execute() throws CommandException {
        String id = stringField("id");
        String name = stringField("name");
        ArrayList<String> species = new ArrayList(Arrays.asList( stringField("species").replace(" ", "").split(",") ));
        
        try{ 
            _receiver.registerVaccine(id, name, species); 
        
        } catch(hva.exceptions.DuplicateVaccineKeyException e1) {
            throw new DuplicateVaccineKeyException(id);
        
        } catch(hva.exceptions.UnknownSpeciesKeyException e2) {
            throw new UnknownSpeciesKeyException(e2.getId());
        }
    

    }

}
