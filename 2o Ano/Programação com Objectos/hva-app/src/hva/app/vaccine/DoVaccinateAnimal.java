package hva.app.vaccine;

import hva.Hotel;
import hva.app.exceptions.UnknownVeterinarianKeyException;
import hva.app.exceptions.VeterinarianNotAuthorizedException;
import hva.app.exceptions.UnknownVaccineKeyException;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoVaccinateAnimal extends Command<Hotel> {

    DoVaccinateAnimal(Hotel receiver) {
        super(Label.VACCINATE_ANIMAL, receiver);
        addStringField("id", Prompt.vaccineKey());
        addStringField("idVeterinarian", Prompt.veterinarianKey());
        addStringField("idAnimal", hva.app.animal.Prompt.animalKey());
    }

    @Override
    protected final void execute() throws CommandException {
        String id = stringField("id");
        String idVeterinarian = stringField("idVeterinarian");
        String idAnimal = stringField("idAnimal");

        try{
            
            // se for false, ent a vacina é má logo dou print da mensagem
            if(!_receiver.vaccinateAnimal(id, idVeterinarian, idAnimal))
                _display.popup(Message.wrongVaccine(id, idAnimal));
                


        } catch(hva.exceptions.UnknownVeterinarianKeyException e1) {
            throw new UnknownVeterinarianKeyException(e1.getId());
        
        } catch(hva.exceptions.VeterinarianNotAuthorizedException e2) {
            throw new VeterinarianNotAuthorizedException(e2.getIdVet(), e2.getIdSpecies());
        
        } catch(hva.exceptions.UnknownVaccineKeyException e3) {
            throw new UnknownVaccineKeyException(id);
        }
    }

}
