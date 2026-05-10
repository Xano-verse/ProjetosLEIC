package pt.tecnico.blockchainist.client.grpc;

import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.common.Debug;
import pt.tecnico.blockchainist.contract.TransactionResponse;

/**
 * Observer class used to handle asynchronous responses from gRPC transaction requests.
 * 
 * This class implements StreamObserver to process server responses for
 * transaction-related operations. It prints a confirmation message when a response
 * is received and logs the successful execution of the command.
 */
public class TransactionObserver implements StreamObserver<TransactionResponse> {

    /** Identifier for the command associated with this observer. */
    private long commandNumber;

    /** Type of the command (e.g., transfer, balance, etc.). */
    private String commandType;

    /**
     * Constructs a TransactionObserver with a specific command identifier and type.
     *
     * @param commandNumber the unique number of the command
     * @param commandType the type of command being executed
     */
    public TransactionObserver(long commandNumber, String commandType) {
        this.commandNumber = commandNumber;
        this.commandType = commandType;
    }

    /**
     * Called when a new TransactionResponse is received from the server.
     *
     * Prints a success message to the standard output and logs the successful
     * completion of the command.
     *
     * @param r the transaction response received from the server
     */
    @Override
    public void onNext(TransactionResponse r) {
        System.out.println("\nOK " + commandNumber);
        Debug.log(commandType + " command completed successfully");
        System.out.flush();
    }

    /**
     * Called when an error occurs during the RPC call.
     * @param throwable the error that occurred
     */
    @Override
    public void onError(Throwable throwable) {
    }

    /**
     * Called when the server has finished sending responses.
     */
    @Override
    public void onCompleted() {
    }
}