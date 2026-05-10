package pt.tecnico.blockchainist.node;

import io.grpc.BindableService;
import io.grpc.Server;
import io.grpc.ServerBuilder;
import io.grpc.ServerInterceptors;
import pt.tecnico.blockchainist.common.Debug;

/**
 * Class that represents a Node in the network
 * This class provides a main method and a method that prints helpful information about the application's usage
 */
public class NodeMain {

    /**
     * Main method, first method that is called when the application is run
     * @param args list of arguments provided by the user
     * @throws Exception
     */
    public static void main(String[] args) throws Exception {
        // check arguments
        if (args.length < 1) {
            System.err.println("Argument(s) missing!");
            printUsage();
            return;
        }

        // parse arguments - host is target host (includes target port) and port is the node's origin port
        int port = -1;
        try {
            port = Integer.parseInt(args[0]);
        } catch (NumberFormatException e) {
            System.err.println("Invalid port (" + args[0] + ") in argument 1");
            printUsage();
            return;
        }
        if (port > 65535 || port < 0) {
            System.err.println("Port number out of range (0-65535): " + port);
            printUsage();
            return;
        }
        String organization = args[1];
        String host = args[2];
   
        final BindableService impl = new NodeServiceImpl(host, organization);
        
        // Create a new server to listen on port.
        Server server = ServerBuilder.forPort(port)
                .addService(ServerInterceptors.intercept(impl, new DelayInterceptor()))
                .build();
		// Start the server.
		server.start();

        // Server threads are running in the background.
		System.out.println("Node server started");
        Debug.log("Server listening on " + port);

		// Do not exit the main thread. Wait until server is terminated.
		server.awaitTermination();
    }
    
    /**
     * Method to print helpful information about the usage of the application
     */
    private static void printUsage() {
        System.err.println("Usage: mvn exec:java -Dexec.args=\"<node port> <organization> <host>:<host port>\"");
    }
}
