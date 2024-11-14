package hva.exceptions;

import java.io.Serial;

public class VeterinarianNotAuthorizedException extends Exception {

    @Serial
	  private static final long serialVersionUID = 202407081733L;
    
    private String idVet;
    private String idSpecies;

    public VeterinarianNotAuthorizedException(String idVet, String idSpecies) {
        this.idVet = idVet;
        this.idSpecies = idSpecies;
    }

    public String getIdVet() {
        return idVet;
    }

    public String getIdSpecies() {
        return idSpecies;
    }
}
