package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import hva.app.exceptions.UnknownHabitatKeyException;

class DoShowAnimalsInHabitat extends Command<Hotel> {

    DoShowAnimalsInHabitat(Hotel receiver) {
        super(Label.ANIMALS_IN_HABITAT, receiver); 
	      addStringField("idHabitat", hva.app.habitat.Prompt.habitatKey());
    }

    @Override
    protected void execute() throws CommandException {
        String idHabitat = stringField("idHabitat");
    
        try {
            _display.popup(_receiver.showAnimalsInHabitat(idHabitat));
        
        } catch(hva.exceptions.UnknownHabitatKeyException e) {
            throw new UnknownHabitatKeyException(idHabitat);
        }
    }

}
