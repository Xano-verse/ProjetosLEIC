package pt.tecnico.blockchainist.node;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.SignatureException;
import java.util.List;
import java.util.concurrent.TimeUnit;

import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
import io.grpc.StatusRuntimeException;
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
import pt.tecnico.blockchainist.contract.SignedDeliverBlockResponse;
import pt.tecnico.blockchainist.contract.Transaction;
import pt.tecnico.blockchainist.node.domain.NodeState;

/**
 * Class that represents the Node-Sequencer communications
 * It implements the Node's client side functionalities
 */
public class NodeSequencerService {
    private final NodeState node;
    private ManagedChannel channel; // not final because if an error occurs it wont be set
    private SequencerServiceGrpc.SequencerServiceBlockingStub stub;

    private final Encryption encryption = new Encryption();
    private java.security.PublicKey publicKey = null;

    /**
     * Builder for this class
     * 
     * @param host address of the sequencer
     */
    public NodeSequencerService(String host, NodeState node) {
        String target = host;
        this.node = node;
        try {
            this.channel = ManagedChannelBuilder.forTarget(target).usePlaintext().build();
            this.stub = SequencerServiceGrpc.newBlockingStub(channel);
            pullBlockchain();
        } catch (StatusRuntimeException e1) {
            System.out.println("Provided sequencer unavailable");
            Debug.log(e1.getStatus().toString());
            System.exit(1);
        }
        
        try {
            publicKey = encryption.loadPublicKey("seq/Seq.pub");
            Debug.log("Public Key from server loaded successfully");
        } catch (Exception e2) {
            Debug.log("Failed to load the sequencer's public key " + e2.getMessage());
        }
    }

    /**
     * Getter for the stub
     * 
     * @return the stub used in the Node-Sequencer communications
     */
    public SequencerServiceGrpc.SequencerServiceBlockingStub getStub() {
        return this.stub;
    }

    /**
     * Shutdown all communications with the sequencer gracefully
     */
    public void shutdown() {
        if (this.channel != null && !this.channel.isShutdown()) {
            this.channel.shutdown();
            try {
                if (!this.channel.awaitTermination(5, TimeUnit.SECONDS)) {
                    this.channel.shutdownNow();
                }
            } catch (InterruptedException e) {
                this.channel.shutdownNow();
                Thread.currentThread().interrupt();
            }
        }
    }

    /**
     * Send a transaction to the sequencer through a broadcast request and receive
     * the broadcast response which will be the sequence number of the sent
     * transaction
     * 
     * @param transaction to send to the sequencer
     * @return the broadcast response, which may or may not contain an error
     */
    public BroadcastResponse broadcast(Transaction transaction, long commandNumber) {
        BroadcastRequest request = BroadcastRequest.newBuilder().setTransaction(transaction)
                .setCommandNumber(commandNumber).build();
        Debug.log("Broadcast request " + transaction.toString() + "sent to Sequencer");

        try {
            return this.stub.broadcast(request);
        } catch (StatusRuntimeException e) {
            // Set sequence number to -1 to indicate failure
            return BroadcastResponse.newBuilder().setSequenceNumber(-1).build();
        }
    }

    private boolean checkSequencerSignature(SignedDeliverBlockResponse sequencerResponse) {

        Signature receivedSignature = sequencerResponse.getSignature();
        String signerId = receivedSignature.getSignerIdentifier();
        try {

            java.security.Signature sig = java.security.Signature.getInstance("SHA256withRSA");
            sig.initVerify(publicKey);
                
            sig.update(sequencerResponse.getBlockResponse().toByteArray());
            boolean isValid = sig.verify(receivedSignature.getSignatureValue().toByteArray());
            
            return isValid;
        } catch (NoSuchAlgorithmException e) {
            Debug.log("Signature getInstance failed " + e.getMessage());
            return false;
        } catch(SignatureException e) {
            Debug.log("Signature failed " + e.getMessage());
            return false;
        } catch(InvalidKeyException e) {
            Debug.log("Signature initialization failed " + e.getMessage());
            return false;
        }

    }

    /**
     * Request a block from the sequencer and return it
     * 
     * @param sequenceNumber the number identifying the block
     * @return the delivered block
     */
    public DeliverBlockResponse deliverBlock(int sequenceNumber) {
        DeliverBlockRequest request = DeliverBlockRequest.newBuilder().setSequenceNumber(sequenceNumber).build();
        Debug.log("Deliver block request for sequence number " + sequenceNumber + " sent to Sequencer");

        try {
            // Check the signature in the block received and returns it
            SignedDeliverBlockResponse signedDeliverBlockResponse = this.stub.deliverBlock(request);
            if (!checkSequencerSignature(signedDeliverBlockResponse)) {
                Debug.log("Signature verification failed for block with sequence number " + sequenceNumber);
                return DeliverBlockResponse.newBuilder().build();
            } else {
                Debug.log("Signature verification successful for block with sequence number " + sequenceNumber);
                return signedDeliverBlockResponse.getBlockResponse();
            }

        } catch (StatusRuntimeException e) {
            // Set transaction to null to indicate failture
            return DeliverBlockResponse.newBuilder().build();
            // TODO EXCEPTION
        }
    }

    /**
     * Ask the sequencer for its current blockchain and make it this node's current
     * blockchain
     */
    public void pullBlockchain() {
        PullBlockchainRequest request = PullBlockchainRequest.newBuilder().build();
        PullBlockchainResponse blockchainResponse = this.stub.pullBlockchain(request);
        Debug.log("Blockchain pulled with " + blockchainResponse.getBlocksCount() + " blocks");

        node.setBlockchain(
                blockchainResponse.getBlocksList().stream().map(response -> response.getTransactionsList()).toList());

        // After pulling and setting blockchain, "apply" each transaction
        int numBlocks = blockchainResponse.getBlocksCount();
        List<List<Transaction>> blockchain = node.getBlockchain();
        for (int block = 0; block < numBlocks; block++) {
            for (Transaction t : blockchain.get(block)) {
                node.parseTransaction(t);
            }
        }

        Debug.log("Finished updating local blockchain after pull");
    }
}