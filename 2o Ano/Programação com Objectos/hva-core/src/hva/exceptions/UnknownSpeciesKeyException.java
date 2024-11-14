package hva.exceptions;

import java.io.Serial;

public class UnknownSpeciesKeyException extends Exception {

	  @Serial
	  private static final long serialVersionUID = 202407081733L;
    
    private String id;

    public UnknownSpeciesKeyException() {}

    public UnknownSpeciesKeyException(String id){
        this.id = id;
    }

    public String getId() {
        return id;
    }

}
