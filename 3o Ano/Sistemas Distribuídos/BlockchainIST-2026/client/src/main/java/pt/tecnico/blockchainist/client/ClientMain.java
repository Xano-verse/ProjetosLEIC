package pt.tecnico.blockchainist.client;

import java.util.ArrayList;

import pt.tecnico.blockchainist.client.grpc.ClientNodeService;
import pt.tecnico.blockchainist.common.Debug;

/**
 * Represents a client
 * This class provides a main method and a method to print helpful information about the command usage
 */
public class ClientMain {

    /**
     * Main method, first method to be called when the java application is executed
     * @param args list of arguments provided through the command line
     */
    public static void main(String[] args) {

        Debug.log("Debug mode ON");
        Debug.log("Client starting...");

        System.out.println(ClientMain.class.getSimpleName());

        // check arguments
        if (args.length < 1) {
            System.err.println("Argument(s) missing!");
            printUsage();
            return;
        }

        // parse arguments
        ArrayList<ClientNodeService> nodes = new ArrayList<>(args.length);
        Debug.log("Parsing command-line arguments...");
        for (String arg : args) {
            String[] split = arg.split(":");
            if (split.length != 3) {
                System.err.println("Invalid argument: " + arg);
                printUsage();
                return;
            }
            String host = split[0];
            int port = -1;
            try {
                port = Integer.parseInt(split[1]);
            } catch (NumberFormatException e) {
                System.err.println("Invalid port (" + split[1] + ") in argument: " + arg);
                printUsage();
                return;
            }
            if (port > 65535 || port < 0) {
                System.err.println("Port number out of range (0-65535): " + port);
                printUsage();
                return;
            }
            String organization = split[2];

            nodes.add(new ClientNodeService(host, port, organization));
            Debug.log("Added node: " + host + ":" + port + " (" + organization + ") to client node list");
        }
        Debug.log("Successfully parsed arguments");

        CommandProcessor processor = new CommandProcessor(nodes);
        processor.userInputLoop();

        // Shutdown all channels before exiting
        for (ClientNodeService node : nodes) {
            Debug.log("Shutting down channels...");
            node.shutdown();
        }
    }

    /**
     * Method to print helpful information about the usage of the application
     */
    private static void printUsage() {
        System.err.println(
                "Usage: mvn exec:java -Dexec.args=\"<host>:<port>:<organization> [<host>:<port>:<organization> ...]\"");
    }
}
