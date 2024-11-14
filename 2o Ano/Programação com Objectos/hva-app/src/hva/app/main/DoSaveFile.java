package hva.app.main;

import hva.HotelManager;
import hva.exceptions.MissingFileAssociationException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import java.io.IOException;
import java.io.FileNotFoundException;

//FIXME import other classes if needed

class DoSaveFile extends Command<HotelManager> {
    DoSaveFile(HotelManager receiver) {
        super(Label.SAVE_FILE, receiver, r -> r.getHotel() != null);
    }

    @Override
    protected final void execute() {
        if(_receiver.changed() == true) {
            try{
              _receiver.save();

            }	catch(MissingFileAssociationException e1) {
                try{
                  //Prompt.newSaveAs só dá return dos string para dar print, requestString é que pede o input e recebe o input prompt como argumento
                _receiver.saveAs(Form.requestString(Prompt.newSaveAs()));

               } catch(FileNotFoundException e2) {e2.printStackTrace();}
                catch(MissingFileAssociationException e2) {e2.printStackTrace();}
                catch(IOException e2) {e2.printStackTrace();}
            
            } catch(FileNotFoundException e1) {e1.printStackTrace();}
            catch(IOException e1) {e1.printStackTrace();}

      //FIXME implement command using a local form
        }
    }
}
