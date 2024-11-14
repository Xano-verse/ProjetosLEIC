package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoShowAllTreesInHabitat extends Command<Hotel> {

    DoShowAllTreesInHabitat(Hotel receiver) {
        super(Label.SHOW_TREES_IN_HABITAT, receiver);
        //FIXME add command fields if needed
    }

    @Override
    protected void execute() throws CommandException {
        String idHabitat = Form.requestString(Prompt.habitatKey());
        try{
        _display.popup(_receiver.showAllTreesInHabitat(idHabitat));
        } catch (hva.exceptions.UnknownHabitatKeyException e) {
            throw new UnknownHabitatKeyException(idHabitat);
        }
        //FIXME implement command
    }

}
