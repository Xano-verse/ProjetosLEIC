package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.UnknownEmployeeKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoShowSatisfactionOfEmployee extends Command<Hotel> {

    DoShowSatisfactionOfEmployee(Hotel receiver) {
        super(Label.SHOW_SATISFACTION_OF_EMPLOYEE, receiver);
        //FIXME add command fields if needed
        addStringField("id", Prompt.employeeKey());
    }

    @Override
    protected final void execute() throws CommandException {
        //FIXME implement command
        String id = stringField("id");
        try{
        _display.popup(_receiver.getEmployeeSatisfaction(id));
        } catch (hva.exceptions.UnknownEmployeKeyException e) {
            throw new UnknownEmployeeKeyException(id);
        }
    }

}
