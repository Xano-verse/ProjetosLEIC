package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.DuplicateHabitatKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoRegisterHabitat extends Command<Hotel> {

    DoRegisterHabitat(Hotel receiver) {
        super(Label.REGISTER_HABITAT, receiver);
        addStringField("id", Prompt.habitatKey());
        addStringField("name", Prompt.habitatName());
        addIntegerField("area", Prompt.habitatArea());
        //FIXME add command fields if needed
    }

    @Override
    protected void execute() throws CommandException {
        //FIXME implement command
        String id = stringField("id");
        String name = stringField("name");
        int area = integerField("area");

        try {
                _receiver.registerHabitat(id, name, area);
            } catch (hva.exceptions.DuplicateHabitatKeyException e) {
                throw new DuplicateHabitatKeyException(id);
            }
    }

}
