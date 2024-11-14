package hva.app.animal;

import hva.Hotel;
import hva.app.exceptions.UnknownAnimalKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoShowSatisfactionOfAnimal extends Command<Hotel> {

    DoShowSatisfactionOfAnimal(Hotel receiver) {
        super(Label.SHOW_SATISFACTION_OF_ANIMAL, receiver);
        //FIXME add command fields if needed
        addStringField("id", Prompt.animalKey());
    }

    @Override
    protected final void execute() throws CommandException {
        //FIXME implement command
        String id = stringField("id");
        try{
        _display.popup(_receiver.getAnimalSatisfaction(id));
        } catch(hva.exceptions.UnknownAnimalKeyException e6) {
                throw new UnknownAnimalKeyException(id); 
            }
    }

}
