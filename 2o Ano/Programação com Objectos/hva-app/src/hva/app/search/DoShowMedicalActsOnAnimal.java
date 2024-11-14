package hva.app.search;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import hva.app.exceptions.UnknownAnimalKeyException;

class DoShowMedicalActsOnAnimal extends Command<Hotel> {

    DoShowMedicalActsOnAnimal(Hotel receiver) {
        super(Label.MEDICAL_ACTS_ON_ANIMAL, receiver);
        addStringField("idAnimal", hva.app.animal.Prompt.animalKey());

    }

    @Override
    protected void execute() throws CommandException {
        String idAnimal = stringField("idAnimal");

        try {
            _display.popup(_receiver.showMedicalActsOnAnimal(idAnimal));
        
        } catch(hva.exceptions.UnknownAnimalKeyException e) {
            throw new UnknownAnimalKeyException(idAnimal);
        }
    }

}
