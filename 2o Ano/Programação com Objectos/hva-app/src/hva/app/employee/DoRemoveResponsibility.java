package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.NoResponsibilityException;
import hva.app.exceptions.UnknownEmployeeKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoRemoveResponsibility extends Command<Hotel> {

    DoRemoveResponsibility(Hotel receiver) {
        super(Label.REMOVE_RESPONSABILITY, receiver);
        addStringField("id", Prompt.employeeKey());
        addStringField("respKey", Prompt.responsibilityKey());
        //FIXME add command fields if needed
    }

    @Override
    protected void execute() throws CommandException {
        String id = stringField("id");
        String respKey = stringField("respKey");
        try {
            _receiver.removeResponsabilityToEmployee(id, respKey);
        } catch (hva.exceptions.NoResponsibilityException e) {
            throw new NoResponsibilityException(id, respKey);
        } catch (hva.exceptions.UnknownEmployeKeyException e1) {
            throw new UnknownEmployeeKeyException(id);
        }
        //FIXME implement command
    }

}
