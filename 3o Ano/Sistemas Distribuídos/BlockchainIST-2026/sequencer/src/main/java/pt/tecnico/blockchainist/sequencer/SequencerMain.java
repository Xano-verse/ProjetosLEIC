package pt.tecnico.blockchainist.sequencer;

import io.grpc.BindableService;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import pt.tecnico.blockchainist.common.Debug;

/**
 * Class that represents the sequencer
 * This class provides a main method and a method to print helpful information
 * about the command usage
 */
public class SequencerMain {
    private static final int N = 4; // max number of transactions per block
    private static final int T = 5; // time limit for block creation in seconds

    public static int getN() {
        return N;
    }

    public static int getT() {
        return T;
    }

    /**
     * Main method, first method to be called when the java application is executed
     * 
     * @param args list of arguments provided through the command line
     * @throws Exception
     */
    public static void main(String[] args) throws Exception {
        // check arguments
        if (args.length < 1) {
            System.err.println("Argument(s) missing!");
            printUsage();
            return;
        }

        // parse arguments - port is the node's origin port
        int port = -1;
        try {
            port = Integer.parseInt(args[0]);
        } catch (NumberFormatException e) {
            System.err.println("Invalid port (" + args[0] + ") in argument 0");
            printUsage();
            return;
        }
        if (port > 65535 || port < 0) {
            System.err.println("Port number out of range (0-65535): " + port);
            printUsage();
            return;
        }

        final BindableService impl = new SequencerServiceImpl(N, T);
        // Create a new server to listen on port.
        Server server = ServerBuilder.forPort(port).addService(impl).build();
        // Start the server.
        server.start();

        // Server threads are running in the background.
        System.out.println("Sequencer server started");
        Debug.log("Server listening on " + port);
        // Do not exit the main thread. Wait until server is terminated.
        server.awaitTermination();
    }

    /**
     * Method to print helpful information about the usage of the application
     */
    private static void printUsage() {
        System.err.println("Usage: mvn exec:java -Dexec.args=\"<port> [<port> ...]\"");
    }
}
