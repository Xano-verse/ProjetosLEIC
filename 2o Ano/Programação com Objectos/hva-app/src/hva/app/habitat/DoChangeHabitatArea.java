package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoChangeHabitatArea extends Command<Hotel> {

    DoChangeHabitatArea(Hotel receiver) {
        super(Label.CHANGE_HABITAT_AREA, receiver);
        // FIXME add command fields if needed
        addStringField("id", Prompt.habitatKey());
        addIntegerField("area", Prompt.habitatArea());
    }

    @Override
    protected void execute() throws CommandException {
        // FIXME implement command
        String id = stringField("id");
        int area = integerField("area");
        try {
            _receiver.changeHabitatArea(id, area);
        } catch (hva.exceptions.UnknownHabitatKeyException e) {
            throw new UnknownHabitatKeyException(id);
        }
    }

}
