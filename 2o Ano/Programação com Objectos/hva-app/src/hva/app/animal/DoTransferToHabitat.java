package hva.app.animal;

import hva.Hotel;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
import hva.app.exceptions.UnknownAnimalKeyException;
import hva.app.exceptions.UnknownHabitatKeyException;

class DoTransferToHabitat extends Command<Hotel> {

    DoTransferToHabitat(Hotel hotel) {
        super(Label.TRANSFER_ANIMAL_TO_HABITAT, hotel);
        addStringField("idAnimal", Prompt.animalKey());
        addStringField("idHabitat", hva.app.habitat.Prompt.habitatKey());
    }

    @Override
    protected final void execute() throws CommandException {
        String idAnimal = stringField("idAnimal");
        String idHabitat = stringField("idHabitat");

        try{
            _receiver.transferAnimalToHabitat(idAnimal, idHabitat);
        
        } catch(hva.exceptions.UnknownAnimalKeyException e1) {
            throw new UnknownAnimalKeyException(idAnimal);
        
        } catch(hva.exceptions.UnknownHabitatKeyException e2) {
            throw new UnknownHabitatKeyException(idHabitat);
        }
    }

}
