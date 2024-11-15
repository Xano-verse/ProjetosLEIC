package hva.app.exceptions;

import pt.tecnico.uilib.menus.CommandException;

import java.io.Serial;

public class DuplicateSpeciesNameException extends CommandException {
	@Serial
	private static final long serialVersionUID = 202407081733L;

	public DuplicateSpeciesNameException(String name) {
		super("A espécie '" + name + "' já existe.");
	}
}
