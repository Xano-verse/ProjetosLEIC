package pt.tecnico.blockchainist.common;

/**
 * Class that represents a Debugging object
 * Useful because all three modules (client, node and sequencer) will need to log debugging messages
 */
public final class Debug {

    // Set flag to true to print debug messages.
    // The flag can be set using the -Ddebug command line option.
    public static final boolean DEBUG_FLAG = (System.getProperty("debug") != null);

    /**
     * Method that prints debug messages to standard error
     * @param message to output
     */
    public static void log(String message) {
        if (DEBUG_FLAG) {
            System.err.println("[DEBUG] " + message);
        }
    }

    /**
     * Method that prints debug messages and error stack traces to standard error
     * @param message to output
     * @param t error with stack trace
     */
    public static void log(String message, Throwable t) {
        if (DEBUG_FLAG) {
            System.err.println("[DEBUG] " + message);
            t.printStackTrace(System.err);
        }
    }
}
