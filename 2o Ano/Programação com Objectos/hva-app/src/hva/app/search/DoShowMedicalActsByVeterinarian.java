package hva.app.search;

import hva.Hotel;
import hva.app.exceptions.UnknownVeterinarianKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;


class DoShowMedicalActsByVeterinarian extends Command<Hotel> {

    DoShowMedicalActsByVeterinarian(Hotel receiver) {
        super(Label.MEDICAL_ACTS_BY_VET, receiver);
        addStringField("idVeterinarian", hva.app.employee.Prompt.employeeKey());
    }

    @Override
    protected void execute() throws CommandException {
        String idVeterinarian = stringField("idVeterinarian");
        
        try{
            _display.popup(_receiver.showMedicalActsByVeterinarian(idVeterinarian));
    
        } catch(hva.exceptions.UnknownVeterinarianKeyException e) {
            throw new UnknownVeterinarianKeyException(e.getId());
        }
    }
}
