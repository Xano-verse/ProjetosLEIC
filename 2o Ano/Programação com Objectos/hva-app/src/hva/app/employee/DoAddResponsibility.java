package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.NoResponsibilityException;
import hva.app.exceptions.UnknownEmployeeKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoAddResponsibility extends Command<Hotel> {

    DoAddResponsibility(Hotel receiver) {
        super(Label.ADD_RESPONSABILITY, receiver);
        addStringField("id", Prompt.employeeKey());
        addStringField("respKey", Prompt.responsibilityKey());
    }

    @Override
    protected void execute() throws CommandException {
        String id = stringField("id");
        String respKey = stringField("respKey");
        try {
            _receiver.addResponsabilityToEmployee(id, respKey);
        } catch (hva.exceptions.NoResponsibilityException e) {
            throw new NoResponsibilityException(id, respKey);
        } catch (hva.exceptions.UnknownEmployeKeyException e1) {
            throw new UnknownEmployeeKeyException(id);
        }
    }

}
