package pt.tecnico.blockchainist.sequencer;

import static io.grpc.Status.ABORTED;
import static io.grpc.Status.INTERNAL;
import static io.grpc.Status.INVALID_ARGUMENT;

import java.security.NoSuchAlgorithmException;
import java.security.SignatureException;
import java.security.InvalidKeyException;
import java.util.ArrayList;
import java.util.List;

import com.google.protobuf.ByteString;

import io.grpc.stub.StreamObserver;
import pt.tecnico.blockchainist.common.Debug;
import pt.tecnico.blockchainist.common.Encryption;
import pt.tecnico.blockchainist.contract.BroadcastRequest;
import pt.tecnico.blockchainist.contract.BroadcastResponse;
import pt.tecnico.blockchainist.contract.DeliverBlockRequest;
import pt.tecnico.blockchainist.contract.DeliverBlockResponse;
import pt.tecnico.blockchainist.contract.PullBlockchainRequest;
import pt.tecnico.blockchainist.contract.PullBlockchainResponse;
import pt.tecnico.blockchainist.contract.SequencerServiceGrpc;
import pt.tecnico.blockchainist.contract.Signature;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.contract.SignedDeliverBlockResponse;

// Class that implements the Sequencer's server functionalities (such as listening for requests)
/**
 * Class that represents the Sequencer's server side functionalities, such as
 * listening for requests
 * Has methods for available sequencer functionalities
 */
public class SequencerServiceImpl extends SequencerServiceGrpc.SequencerServiceImplBase {
    // Since grpc calls are handled by a pool of threads, we need to ensure that the
    // list of transactions is thread-safe.
    private Block currentBlock = null;
    private final List<Block> blockchain = new ArrayList<>();

    private static int n;
    private static int t;

    private static final Encryption encryption = new Encryption();

    private java.security.PrivateKey privateKey;            // not set as final because the initialization can fail

    public SequencerServiceImpl(int n, int t) {
        this.n = n;
        this.t = t;

        try {
            this.privateKey = encryption.loadPrivateKey("Seq.priv");
            Debug.log("Private Key from sequencer loaded successfully");
        } catch(Exception e) {
            Debug.log("Failed to load sequencer's private key " + e.getMessage());
        }
    }

    private Signature signBlock(DeliverBlockResponse response) {
        try {
            java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
            sig.initSign(this.privateKey);
            sig.update(response.toByteArray());
            byte[] signatureBytes = sig.sign();

            Signature signature = Signature.newBuilder()
                    .setSignerIdentifier("Seq")
                    .setSignatureValue(ByteString.copyFrom(signatureBytes))
                    .build();

            return signature;
        } catch(NoSuchAlgorithmException e) {
            Debug.log("Signature getInstance failed " + e.getMessage());
            return null;
        } catch(InvalidKeyException e) {
            Debug.log("Signature initialization failed " + e.getMessage());
            return null;
        } catch(SignatureException e) {
            Debug.log("Signature failed " + e.getMessage());
            return null;
        }
    }

    /**
     * @param commandNumber
     * @return true if the transaction with commandNumber has already been processed
     */
    private boolean transactionExists(long commandNumber) {
        for (int blockNumber = 0; blockNumber < this.blockchain.size(); blockNumber++) {
            for (Transaction t : this.blockchain.get(blockNumber).getTransactions()) {
                if (t.getCommandNumber() == commandNumber) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * Method that receives a request from a node, adds its transaction to the list
     * and responds back to the node
     */
    @Override
    public synchronized void broadcast(BroadcastRequest request, StreamObserver<BroadcastResponse> responseObserver) {

        try {
            if (request.getTransaction() == null) {
                responseObserver
                        .onError(INVALID_ARGUMENT.withDescription("Transaction cannot be null").asRuntimeException());
                return;
            }

            BroadcastResponse response;

            // transaction already handled
            if (transactionExists(request.getCommandNumber())) {
                response = BroadcastResponse.newBuilder()
                        .setSequenceNumber(-1)
                        .build();
            } else {
                // Add the transaction to the block
                // verify if we have a block opened, if not, open a new block
                if (currentBlock == null || !currentBlock.getIsOpen()) {
                    currentBlock = new Block(request.getTransaction(), n, t);
                    blockchain.add(currentBlock);
                } else {
                    currentBlock.addTransaction(request.getTransaction(), n);
                }

                response = BroadcastResponse.newBuilder()
                        .setSequenceNumber(this.blockchain.size())
                        .build();
            }

            // Send a single response through the stream.
            responseObserver.onNext(response);
            // Notify the node that the operation has been completed.
            responseObserver.onCompleted();

            Debug.log("Received transaction: " + request.getTransaction());
        } catch (IllegalArgumentException e) {
            responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        } catch (Exception e) {
            responseObserver.onError(
                    INTERNAL.withDescription("Failed to broadcast transaction" + e.getMessage()).asRuntimeException());
        }
    }

    /**
     * Sends the requested block to the node
     */
    @Override
    public void deliverBlock(DeliverBlockRequest request, StreamObserver<SignedDeliverBlockResponse> responseObserver) {

        try {
            int seqNum = request.getSequenceNumber();
            Debug.log("Received sequence number " + seqNum);

            if (seqNum < 0 || seqNum > blockchain.size()) {
                responseObserver
                        .onError(INVALID_ARGUMENT.withDescription("Invalid sequence number").asRuntimeException());
                return;
            }

            // Get the block corresponding to the sequence number, which is the order in
            // blockchain
            Block block = blockchain.get(seqNum - 1); // indexed to 0

            synchronized (block) {
                while (block.getIsOpen()) {
                    Debug.log("Waiting to close block...");
                    block.wait(); // Wait until the block is closed
                }
            }

            Debug.log("Requested block has closed, returning block " + seqNum);

            // Block is closed, we can return the transactions
            List<Transaction> transactions = block.getTransactions();
            DeliverBlockResponse response = DeliverBlockResponse.newBuilder()
                    .addAllTransactions(transactions)
                    .setSequenceNumber(seqNum)
                    .build();

            Signature signature = signBlock(response);

            SignedDeliverBlockResponse signedResponse = SignedDeliverBlockResponse.newBuilder()
                    .setBlockResponse(response)
                    .setSignature(signature)
                    .build();

            // Send a single response through the stream.
            responseObserver.onNext(signedResponse);
            // Notify the node that the operation has been completed.
            responseObserver.onCompleted();
        } catch (IllegalArgumentException e) {
            responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        } catch (InterruptedException e) {
            responseObserver.onError(ABORTED.withDescription(e.getMessage()).asRuntimeException());
        }
    }

    /**
     * Sends the sequencer's entire blockchain over to the node that requested it
     */
    @Override
    public void pullBlockchain(PullBlockchainRequest request, StreamObserver<PullBlockchainResponse> responseObserver) {
        try {
            PullBlockchainResponse response = PullBlockchainResponse.newBuilder()
                    .addAllBlocks(blockchain.stream()
                            .map(block -> DeliverBlockResponse.newBuilder().addAllTransactions(block.getTransactions())
                                    .build())
                            .toList())
                    .build();

            Debug.log("Received pull request from node");

            responseObserver.onNext(response);
            responseObserver.onCompleted();
        } catch (IllegalArgumentException e) {
            responseObserver.onError(INVALID_ARGUMENT.withDescription(e.getMessage()).asRuntimeException());
        }
    }

}