package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.DuplicateTreeKeyException;
import hva.app.exceptions.UnknownHabitatKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoAddTreeToHabitat extends Command<Hotel> {

    DoAddTreeToHabitat(Hotel receiver) {
        super(Label.ADD_TREE_TO_HABITAT, receiver);
        // FIXME add command fields if needed
        addStringField("id", Prompt.habitatKey());
        addStringField("treeId", Prompt.treeKey());
        addStringField("treeName", Prompt.treeName());
        addRealField("treeAge", Prompt.treeAge());
        addIntegerField("treeDifficulty", Prompt.treeDifficulty());
        addOptionField("treeType", Prompt.treeType(), "PERENE", "CADUCA");
    }

    @Override
    protected void execute() throws CommandException {
        // FIXME implement command
        String id = stringField("id");
        String treeId = stringField("treeId");
        String treeName = stringField("treeName");
        Double treeAge = realField("treeAge");
        int treeDifficulty = integerField("treeDifficulty");
        String treeType = stringField("treeType");;

        try {
            _display.popup(_receiver.plantTreeInHabitat(id, treeId, treeName, treeAge, treeDifficulty, treeType));
        } catch (hva.exceptions.DuplicateTreeKeyException e) {
            throw new DuplicateTreeKeyException(treeId);
        } catch (hva.exceptions.UnknownHabitatKeyException e) {
            throw new UnknownHabitatKeyException(id);
        }
    }

}
