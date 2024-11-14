package hva.app.habitat;

import hva.Hotel;
import hva.app.exceptions.UnknownHabitatKeyException;
import hva.app.exceptions.UnknownSpeciesKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoChangeHabitatInfluence extends Command<Hotel> {

    DoChangeHabitatInfluence(Hotel receiver) {
        super(Label.CHANGE_HABITAT_INFLUENCE, receiver);
        // FIXME add command fields if needed
        addStringField("idHabitat", Prompt.habitatKey());
        addStringField("idSpecies", hva.app.animal.Prompt.speciesKey());
        addOptionField("influence", Prompt.habitatInfluence(), "POS", "NEU", "NEG");
    }

    @Override
    protected void execute() throws CommandException {
        // FIXME implement command
        String idHabitat = stringField("idHabitat");
        String idSpecies = stringField("idSpecies");
        String influence = stringField("influence");
        try {
            _receiver.changehabitatInfluence(idHabitat, idSpecies, influence);
        } catch (hva.exceptions.UnknownHabitatKeyException e) {
            throw new UnknownHabitatKeyException(idHabitat);
        } catch (hva.exceptions.UnknownSpeciesKeyException e1) {
            throw new UnknownSpeciesKeyException(idSpecies);
        }
    }
}
