package hva.app.employee;

import hva.Hotel;
import hva.app.exceptions.DuplicateEmployeeKeyException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoRegisterEmployee extends Command<Hotel> {

    DoRegisterEmployee(Hotel receiver) {
        super(Label.REGISTER_EMPLOYEE, receiver);
        addStringField("id", Prompt.employeeKey());
        addStringField("name", Prompt.employeeName());
        addOptionField("type", Prompt.employeeType(), "VET", "TRT");
    }
    
    @Override
    protected void execute() throws CommandException {
        String id = stringField("id");
        String name = stringField("name");
        String type = stringField("type");
        try {
                _receiver.registerEmployee(id, name, type);
            } catch (hva.exceptions.DuplicateEmployeKeyException e) {
                throw new DuplicateEmployeeKeyException(id);
            }
        //FIXME implement command
    }

}
