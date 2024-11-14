package hva.exceptions;

import java.io.Serial;

public class UnknownVeterinarianKeyException extends Exception {
    @Serial
    private static final long serialVersionUID = 202407081733L;

    private String id;

    public UnknownVeterinarianKeyException(String id) {
        this.id = id;
    }

    public String getId() {
        return id;
    }

}
