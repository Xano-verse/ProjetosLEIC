package pt.tecnico.blockchainist.common;

import java.io.IOException;
import java.io.InputStream;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;

public class Encryption {

    // VER EXCECOES

    private byte[] readResource(String path) throws IllegalArgumentException {
        try (InputStream is = getClass().getClassLoader().getResourceAsStream(path)) {
            if (is == null) {
                throw new IllegalArgumentException("Ficheiro não encontrado: " + path);
            }
            return is.readAllBytes();
        } catch (IOException e) {
            throw new IllegalArgumentException("Erro ao ler o ficheiro: " + path, e);
        }
    }

    public PrivateKey loadPrivateKey(String resourcePath) throws Exception {
        byte[] keyBytes = this.readResource(resourcePath);
        PKCS8EncodedKeySpec spec = new PKCS8EncodedKeySpec(keyBytes);
        KeyFactory kf = KeyFactory.getInstance("RSA");
        return kf.generatePrivate(spec);
    }
    
    public PublicKey loadPublicKey(String resourcePath) throws Exception {
        byte[] keyBytes = readResource(resourcePath);
        X509EncodedKeySpec spec = new X509EncodedKeySpec(keyBytes);
        KeyFactory kf = KeyFactory.getInstance("RSA");
        return kf.generatePublic(spec);
    }

}
