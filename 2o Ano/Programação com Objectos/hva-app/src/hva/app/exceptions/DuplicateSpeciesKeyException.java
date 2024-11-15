package hva.app.exceptions;

import pt.tecnico.uilib.menus.CommandException;

import java.io.Serial;

public class DuplicateSpeciesKeyException extends CommandException {
	@Serial
	private static final long serialVersionUID = 202407081733L;

	public DuplicateSpeciesKeyException(String key) {
		super("A espécie '" + key + "' já existe.");
	}
}
