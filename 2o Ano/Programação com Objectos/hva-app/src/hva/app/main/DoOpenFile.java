package hva.app.main;

import hva.HotelManager;
import hva.app.exceptions.FileOpenFailedException;
import hva.exceptions.UnavailableFileException;
import pt.tecnico.uilib.forms.Form;
import pt.tecnico.uilib.menus.Command;
import pt.tecnico.uilib.menus.CommandException;

import java.io.IOException;

//FIXME import other classes if needed

class DoOpenFile extends Command<HotelManager> {
    DoOpenFile(HotelManager receiver) {
        super(Label.OPEN_FILE, receiver);
    }

    @Override
    protected final void execute() throws CommandException {
        boolean doLastSave = false;

        try {
            if (_receiver.changed() == true) {
                doLastSave = Form.confirm(Prompt.saveBeforeExit());
            }
            if (doLastSave == true) {
                // estas duas linhas é só para fazer um DoSaveFile.execute() só que é preciso
                // uma variável
                DoSaveFile cmd = new DoSaveFile(_receiver);
                cmd.execute();
            }

            _receiver.load(Form.requestString(Prompt.openFile()));

        } catch (UnavailableFileException e) {
            throw new FileOpenFailedException(e);
        }
    }
}
