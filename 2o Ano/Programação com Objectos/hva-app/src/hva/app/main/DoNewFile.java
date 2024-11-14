package hva.app.main;

import hva.HotelManager;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;
//FIXME import other classes if needed

class DoNewFile extends Command<HotelManager> {
    DoNewFile(HotelManager receiver) {
        super(Label.NEW_FILE, receiver);
    }

    @Override
    protected final void execute() throws CommandException {
        boolean doLastSave = false;  
        // Antes de criar uma nova aplicacao vazia vemos se querem guardar a antiga
        if(_receiver.changed() == true) {
            doLastSave = Form.confirm(Prompt.saveBeforeExit());
        }
        if(doLastSave == true) {
            // estas duas linhas é só para fazer um DoSaveFile.execute() só que é preciso uma variável
            DoSaveFile cmd = new DoSaveFile(_receiver);
            cmd.execute();
        }
        
        // Criamos aplicacao vazia (nao associada a nenhum ficheiro)
        _receiver.reset();

    }
}
